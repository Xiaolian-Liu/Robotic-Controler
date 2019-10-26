// #include <iostream>    in this place, can't use iostream 
// beacuse the in xenomai space, use the system call will
// cause xenomai kernel into linux kernel
// use rt_printf() instead of printf()

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>

#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <malloc.h>
#include <math.h>
#include <pthread.h>

#include <rtdm/rtdm.h>
#include <rtdk.h>
#include <native/syscall.h>
#include <native/task.h>
#include <native/sem.h>
#include <native/mutex.h>
#include <native/timer.h>
#include <native/heap.h>
#include <native/queue.h>

#include <commu.h>
#include "ecrt.h"
#include "EcatDrive.h"

// int run = 1;
int istest = 0;
unsigned short N = 0;


/****************************************************************************/

// EtherCAT distributed clock variables
const int       cycle_ns = PERIOD_NS; /* 2 ms */
const int64_t   f = FREQUENCY;
#define DC_FILTER_CNT          512

static uint64_t dc_start_time_ns = 0LL;
static uint64_t dc_time_ns = 0;

#if SYNC_MASTER_TO_REF
static uint8_t  dc_ignored = 1;
static int32_t  dc_diff_ns = 0;
static int32_t  normal_dc_diff_ns = 0;
static int32_t  shift_time = 1000000;
static int32_t  err_sync_ns[FILT_CNT] = {0};
static int32_t  err_ns = 0;
static int32_t  prev_err_ns = 0;
static int32_t  ierr = 0;
static int32_t  derr = 0;
static int32_t  dc_index = 0;
static int32_t  prev_dc_diff_ns = 0;
// static int64_t  dc_diff_total_ns = 0LL;
static int64_t  dc_delta_total_ns = 0LL;
static int      dc_filter_idx = 0;
static int64_t  dc_adjust_ns;
#endif
static int64_t  system_time_base = 0LL;
static uint64_t wakeup_time = 0LL;
static uint64_t overruns = 0LL;

/****************************************************************************/
/* use static for not the same vaule err when link */
static RT_HEAP         data_heap;
static RT_HEAP         stat_heap;
static RT_QUEUE        tarpos_queue;
static driverdata_t *  data;
static driverstate_t * state;
static void * data_sharm;
static void * stat_sharm;

const int32_t maxacc = 32292536;

static ec_master_t *master = NULL;
//static ec_master_state_t master_state = {};

static ec_domain_t *domain = NULL;
//static ec_domain_state_t domain_state = {};

static ec_slave_config_t *sc_CDR[NUMSL] = {NULL};

// process data point
static uint8_t *domain_pd = NULL;

// offsets for PDO entries
static unsigned int off_cntlwd[NUMSL];
static unsigned int off_tarpos[NUMSL];
static unsigned int off_tarvel[NUMSL];
static unsigned int off_tartor[NUMSL];
static unsigned int off_modopr[NUMSL];
static unsigned int off_dubyt1[NUMSL];
static unsigned int off_toprfu[NUMSL];

static unsigned int off_stawrd[NUMSL];
static unsigned int off_actpos[NUMSL];
static unsigned int off_actvel[NUMSL];
static unsigned int off_acttor[NUMSL];
static unsigned int off_moddis[NUMSL];
static unsigned int off_dubyt2[NUMSL];
static unsigned int off_floerr[NUMSL];
static unsigned int off_diginp[NUMSL];
static unsigned int off_toprst[NUMSL];
static unsigned int off_toprpo[NUMSL];


/* the sign function */
int sign(int32_t a)
{
    if(a > 0)
    {
        return 1;
    }
    else if (a < 0)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

void exit_ecat(rt_sigset_t sig)
{
    run = 0;
}

//domain registration, you have to register domain so that you can send and receive PDO data
static ec_pdo_entry_reg_t domain_regs[17*NUMSL];

ec_pdo_entry_info_t slave_pdo_entries[] = {
    {0x6040, 0x00, 16}, /* Control Word                 Unsigned16 */
    {0x607a, 0x00, 32}, /* Target Position              Integer32  */
    {0x60ff, 0x00, 32}, /* Target Velocity */
    {0x6071, 0x00, 16}, /* Target torque */
    {0x6060, 0x00, 8}, /* Modes of Operation            Integer8*/
    {0x5ffe, 0x00, 8}, /* Dummy Byte 1 */
    {0x60b8, 0x00, 16}, /* Touch probe function */
    {0x6041, 0x00, 16}, /* Status Word                  Unsigned16 */
    {0x6064, 0x00, 32}, /* Position Actual Value        Integer32  */
    {0x606c, 0x00, 32}, /* Velocity Actual Value */
    {0x6077, 0x00, 16}, /* Torque Actual Value */
    {0x6061, 0x00, 8}, /* Modes of Operation Display */
    {0x5fff, 0x00, 8}, /* Dummy Byte 2 */
    {0x60f4, 0x00, 32}, /* Following Error Actual Value */
    {0x60fd, 0x00, 32}, /* Digital Inputs */
    {0x60b9, 0x00, 16}, /* Touch probe status           Unsigned16 */
    {0x60ba, 0x00, 32}, /* Touch probe pos 1 pos value  Integer32  */
};
ec_pdo_info_t slave_pdos[] = {
    {0x1600, 7, slave_pdo_entries + 0}, /* Position Control RxPDO  */
    {0x1a00, 10, slave_pdo_entries + 7}, /* Position Control TxPDO */
};
ec_sync_info_t slave_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},
    {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
    {2, EC_DIR_OUTPUT, 1, slave_pdos + 0, EC_WD_ENABLE},
    {3, EC_DIR_INPUT, 1, slave_pdos + 1, EC_WD_DISABLE},
    {0xff}
};

/*****************************************************************************
 * Realtime task
 ****************************************************************************/

/** Get the time in ns for the current cpu, adjusted by system_time_base.
 *
 * \attention Rather than calling rt_get_time_ns() directly, all application
 * time calls should use this method instead.
 *
 * \ret The time in ns.
 */
uint64_t system_time_ns(void)
{
    RTIME time = rt_timer_read();

    if (system_time_base > int64_t(time)) {
        rt_printf("%s() error: system_time_base greater than"
                " system time (system_time_base: %lld, time: %llu\n",
                __func__, system_time_base, time);
        return time;
    }
    else {
        return time - system_time_base;
    }
}

/****************************************************************************/

/** Convert system time to Xenomai time (via the system_time_base).
 */
RTIME system2rtime(uint64_t time)
{
    RTIME ret;
    if ((system_time_base < 0) &&
            ((uint64_t) (-system_time_base) > time)) {
        rt_printf("%s() error: system_time_base less than"
                " system time (system_time_base: %lld, time: %llu\n",
                __func__, system_time_base, time);
        ret = time;
    }
    else {
        ret = time + system_time_base;
    }

    return ret;
}

/*****************************************************************************/

/** Synchronise the distributed clocks
 */
void sync_distributed_clocks(void)
{
#if SYNC_MASTER_TO_REF
    uint32_t ref_time = 0;
    uint64_t prev_app_time = dc_time_ns;
#endif

    dc_time_ns = system_time_ns();

    // set master time in nano-seconds
    ecrt_master_application_time(master, dc_time_ns);

#if SYNC_MASTER_TO_REF
    // get reference clock time to synchronize master cycle
    ecrt_master_reference_clock_time(master, &ref_time);
    dc_diff_ns = (uint32_t) prev_app_time - ref_time;
#else
    // sync reference clock to master
    ecrt_master_sync_reference_clock(master);
#endif

    // call to sync slaves to ref slave
    ecrt_master_sync_slave_clocks(master);
}

/*****************************************************************************/

/** Update the master time based on ref slaves time diff
 *
 * called after the ethercat frame is sent to avoid time jitter in
 * sync_distributed_clocks()
 */
void update_master_clock(void)
{
#if SYNC_MASTER_TO_REF
    // calc drift (via un-normalised time diff)
    int32_t delta = dc_diff_ns - prev_dc_diff_ns;
    prev_dc_diff_ns = dc_diff_ns;
//    rt_printf("%d\n",dc_diff_ns);
    // normalise the time diff
    if(dc_diff_ns < 0)
    {
        normal_dc_diff_ns = dc_diff_ns % cycle_ns + cycle_ns;
    }
    else
    {
        normal_dc_diff_ns = dc_diff_ns % cycle_ns;
    }
/* 
    rt_printf("prev_dc_diff_us: %d\t", prev_dc_diff_ns/1000);
    rt_printf("normal_dc_diff_us: %d\t", normal_dc_diff_ns/1000);
     */
    err_sync_ns[dc_index] = normal_dc_diff_ns;
    dc_index++;
    if(FILT_CNT == dc_index)
        dc_index = 0;
    int32_t err = 0;
    for(int i=0; i<FILT_CNT; i++)
        err += err_sync_ns[i];
    err = err/FILT_CNT;
    // rt_printf("err ns: %d\t", err);
    err = err-shift_time;
    prev_err_ns = err;
    err_ns = err;
    // rt_printf("normal err ns; %d\n", err);
    if(!dc_ignored)
    {
        dc_delta_total_ns += delta;

        dc_filter_idx++;

        if (dc_filter_idx >= DC_FILTER_CNT) 
        {
            // add rounded delta average
            dc_adjust_ns +=
                (dc_delta_total_ns + (DC_FILTER_CNT / 2)) / DC_FILTER_CNT;
                 // reset
            dc_delta_total_ns = 0LL;

            dc_filter_idx = 0;
        }

        ierr +=err;
        derr = err_ns - prev_err_ns;
        system_time_base += dc_adjust_ns + Kp*err + ki*ierr + kd*derr;
        dc_adjust_ns = 0;
    }
    else
    {
        dc_ignored++;
        if(20 == dc_ignored)
            dc_ignored = 0;
    }

#endif
}

/*****************************************************************************
 * Realtime task
 ****************************************************************************/

void rt_check_domain_state(ec_domain_t *domain)
{
    static ec_domain_state_t domain_state = {}; 
    ec_domain_state_t ds = {};
	ecrt_domain_state(domain, &ds);
    if (ds.working_counter != domain_state.working_counter) {
        rt_printf("domain: WC %u.\n", ds.working_counter);
        state->WKC = ds.working_counter;
    }
    if (ds.wc_state != domain_state.wc_state) {
        rt_printf("domain: State %u.\n", ds.wc_state);
        state->WC_state = ds.wc_state;
    }
    domain_state = ds;
}

void rt_check_master_state(ec_master_t *master)
{
    static ec_master_state_t master_state = {};
    ec_master_state_t ms;
	ecrt_master_state(master, &ms);
    if (ms.slaves_responding != master_state.slaves_responding) {
        rt_printf("%u slave(s).\n", ms.slaves_responding);
        state->slaves_responding = ms.slaves_responding;
    }
    if (ms.al_states != master_state.al_states) {
        rt_printf("AL states: 0x%02X.\n", ms.al_states);
        state->al_states = ms.al_states;
    }
    if (ms.link_up != master_state.link_up) {
        rt_printf("Link is %s.\n", ms.link_up ? "up" : "down");
        state->link_up = ms.link_up;
    }
    master_state = ms;
}
void wait_period(void)
{
    while (1)
    {
        RTIME wakeup_count = system2rtime(wakeup_time);
        RTIME current_count = rt_timer_read();

        if ((wakeup_count < current_count)
                || (wakeup_count > current_count + (50 * cycle_ns))) {
            rt_printf("%s(): unexpected wake time!\n", __func__);
        }

        switch (rt_task_sleep_until(wakeup_count)) 
        {
            case EINTR:
                rt_printf("AWake before the sleep time has elapsed: EINTR\n");
                continue;
            case EWOULDBLOCK:
                rt_printf("system timer is inactive, and wakeup_count is valid\
                 but different from TM_INFINITE: EWOULDBLOCK\n");
                continue;
            case EPERM:
                rt_printf("cannot sleep: EPERM\n");
                continue;
            case ETIMEDOUT:
                rt_printf("date has already elapsed: ETIMEDOUT\n");
                overruns++;

                if (overruns % 100 == 0) {
                    // in case wake time is broken ensure other processes get
                    // some time slice (and error messages can get displayed)
                    rt_task_sleep(cycle_ns / 100);
                }
                break;

            default:
                break;
        }

        // done if we got to here
        break;
    }

    // calc next wake time (in sys time)
    wakeup_time += cycle_ns;
}


int ecat_init(void)
{
    for(uint16_t i=0; i<NUMSL; i++)
    {
        domain_regs[17*i+0]  = {0, i, CoolDrive, 0x6040, 0x00, off_cntlwd+i};// U16 0
        domain_regs[17*i+1]  = {0, i, CoolDrive, 0x607a, 0x00, off_tarpos+i}; // S32 2
        domain_regs[17*i+2]  = {0, i, CoolDrive, 0x60ff, 0x00, off_tarvel+i}; // S32 6
        domain_regs[17*i+3]  = {0, i, CoolDrive, 0x6071, 0x00, off_tartor+i};
        domain_regs[17*i+4]  = {0, i, CoolDrive, 0x6060, 0x00, off_modopr+i};
        domain_regs[17*i+5]  = {0, i, CoolDrive, 0x5ffe, 0x00, off_dubyt1+i};
        domain_regs[17*i+6]  = {0, i, CoolDrive, 0x60b8, 0x00, off_toprfu+i};
        domain_regs[17*i+7]  = {0, i, CoolDrive, 0x6041, 0x00, off_stawrd+i};// U16 17
        domain_regs[17*i+8]  = {0, i, CoolDrive, 0x6064, 0x00, off_actpos+i};// U16 19
        domain_regs[17*i+9]  = {0, i, CoolDrive, 0x606c, 0x00, off_actvel+i};// 23
        domain_regs[17*i+10] = {0, i, CoolDrive, 0x6077, 0x00, off_acttor+i};
        domain_regs[17*i+11] = {0, i, CoolDrive, 0x6061, 0x00, off_moddis+i};
        domain_regs[17*i+12] = {0, i, CoolDrive, 0x5fff, 0x00, off_dubyt2+i};
        domain_regs[17*i+13] = {0, i, CoolDrive, 0x60f4, 0x00, off_floerr+i};
        domain_regs[17*i+14] = {0, i, CoolDrive, 0x60fd, 0x00, off_diginp+i};
        domain_regs[17*i+15] = {0, i, CoolDrive, 0x60b9, 0x00, off_toprst+i};
        domain_regs[17*i+16] = {0, i, CoolDrive, 0x60ba, 0x00, off_toprpo+i};// 21
    }
/* 
    rt_print_auto_init(1);
	if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1)
    {
		perror("mlockall failed");
		return -1;
    }
 */
	master = ecrt_request_master(0);
	rt_printf("ecrt_request_master is called \n");
	if (!master)
		return -1;

    domain = ecrt_master_create_domain(master);
	if(!domain)
		return -1;

    for(int i=0;i<NUMSL;i++)
    {
        if(!(sc_CDR[i] = ecrt_master_slave_config(master, 0, i, CoolDrive)))
	    {
		    rt_fprintf(stderr, "Failed to get slave %d configuration. \n",i);
		    return -1;
	    }
        rt_printf("Configuring Slave %d PDOs...\n",i);
	    if (ecrt_slave_config_pdos(sc_CDR[i], EC_END, slave_syncs))
	    {
		    rt_fprintf(stderr, "Failed to configure Slave %d PDOs.\n",i);
		    return -1;
	    }
	    rt_printf("configureing Slave %d PDOs is completed!\n",i);
    }

    if( ecrt_domain_reg_pdo_entry_list(domain, domain_regs))
	{
		rt_fprintf(stderr, "PDO entty registration filed! \n");
		return -1;
	}
    rt_printf("PDO entry registration succeed!");
    

    /* Set the initial master time and select a slave to use as the DC
     * reference clock, otherwise pass NULL to auto select the first capable
     * slave. Note: This can be used whether the master or the ref slave will
     * be used as the systems master DC clock.
     */
    dc_start_time_ns = system_time_ns();
    dc_time_ns = dc_start_time_ns;

    // configure SYNC signals for this slave
    for(int i=0; i<NUMSL; i++)
        ecrt_slave_config_dc(sc_CDR[i], 0x0300, PERIOD_NS,0, 0, 0);

	rt_printf("Activating master...\n");
	if (ecrt_master_activate(master))
		return -1;

    if( !(domain_pd = ecrt_domain_data(domain)))
	{
		return -1;
	}

	return 0;
}


void ecat_task(void *arg)
{
    rt_print_auto_init(1);
    int err = rt_heap_bind(&data_heap,DRIVE_DATA_HEAP_NAME,1000000000);
    if(err <0){
        rt_fprintf(stderr, "driver dada heap bind fail in EcatDrive.cpp:ecat_init(): %d", err);
    }

    err = rt_heap_bind(&stat_heap,DRIVE_STATE_HEAP_NAME,1000000000);
    if(err <0){
        rt_fprintf(stderr, "driver stat bind fail in EcatDrive.cpp:ecat_init(): %d", err);
    }

    err = rt_queue_bind(&tarpos_queue, TARPOS_QUEUE_NAME, 1000000000);
    if(err <0){
        rt_fprintf(stderr, "target position queue bind fail in EcatDrive.cpp:ecat_init(): %d", err);
    }

    if(-1 == ecat_init())
        rt_printf("ethercat init err!");
/*        
    RTIME wakeupTime;                   // get current time
	wakeupTime = rt_timer_read();       // use to get the clock time
*/
    rt_printf("Starting cyclic function.\n");

    // int cycle_counter = 0;
    // oneshot mode to allow adjustable wake time
   // rt_timer_set_mode(TM_ONESHOT);
    
   // set first wake time in a few cycles
    wakeup_time = system_time_ns() + 10 * cycle_ns;

    istest = 0;
    // unsigned short j =0;
    // int k = 0;
	while(run)
	{
        // wait for next period (using adjustable system time)
        wait_period();

        err = rt_heap_alloc(&data_heap, 0, TM_NONBLOCK, &data_sharm);
        if(err < 0){
            rt_fprintf(stderr, "data heap alloc faile in EcatDrive.cpp : %d", err);
        }
        data = (driverdata_t *) data_sharm;

        err = rt_heap_alloc(&stat_heap, 0, TM_NONBLOCK, &stat_sharm);
        if(err < 0){
            rt_fprintf(stderr, "state heap alloc faile in EcatDrive.cpp : %d", err);
        }
        state = (driverstate_t *) stat_sharm;

		ecrt_master_receive(master);
		ecrt_domain_process(domain);
        rt_check_domain_state(domain);
    
        err = rt_queue_read(&tarpos_queue, data->TargetPosition, NUMSL*sizeof(int32_t), TM_NONBLOCK);
        if(err < 0){
            // rt_fprintf(stderr,"target position queue read fail: %d\n", err);
        }

        for(int i=0; i<NUMSL; i++)
        {
            data->StatusWord[i] = EC_READ_U16(domain_pd + off_stawrd[i]);
            data->ActualPosition[i] = EC_READ_S32(domain_pd + off_actpos[i]);
            data->ActualVelocity[i] = EC_READ_S32(domain_pd + off_actvel[i]);
            EC_WRITE_U16(domain_pd+off_cntlwd[i], data->ControlWord[i]);
            EC_WRITE_S8(domain_pd+off_modopr[i], data->ModeOperation[i]); 

            if((err < 0) && (abs(data->ActualVelocity[i]) < 10000) )
            {   // that meas the velocity is zero
                data->TargetPosition[i] = data->ActualPosition[i];
            }
/* 
            if(abs(2*(f*f*data.TargetPosition[i] - f*f*data.ActualPosition[i] - f*data.ActualVelocity[i])) >
                        maxacc)     // that means the next position is beyound 
            {
                data.TargetPosition[i] = data.ActualPosition[i] + data.ActualVelocity[i]/f +
            }
 */
            EC_WRITE_S32(domain_pd+off_tarpos[i], data->TargetPosition[i]);
        }

        rt_check_master_state(master);

        ecrt_domain_process(domain);
		ecrt_domain_queue(domain); //queue all the data
        // sync distributed clock just before master_send to set
        // most accurate master clock time
        sync_distributed_clocks();
		ecrt_master_send(master); //send all data to ethercat slave
        // update the master clock
        // Note: called after ecrt_master_send() to reduce time
        // jitter in the sync_distributed_clocks() call
        update_master_clock();

        err = rt_heap_free(&data_heap, data_sharm);
        if(err < 0)
            rt_fprintf(stderr, "data heap free fail in EcatDrive.cpp : %d", err);

        err = rt_heap_free(&stat_heap, stat_sharm);
        if(err < 0)
            rt_fprintf(stderr, "state heap free fail in EcatDrive.cpp : %d", err);
            
	}

    err = rt_heap_unbind(&data_heap);
    if(err <0){
        rt_fprintf(stderr, "driver dada heap bind fail in EcatDrive.cpp:ecat_init(): %d", err);
    }

    err = rt_heap_unbind(&stat_heap);
    if(err <0){
        rt_fprintf(stderr, "driver stat bind fail in EcatDrive.cpp:ecat_init(): %d", err);
    }

    err = rt_queue_unbind(&tarpos_queue);
    if(err <0){
        rt_fprintf(stderr, "target position queue bind fail in EcatDrive.cpp:ecat_init(): %d", err);
    }

    ecrt_release_master(master);
}