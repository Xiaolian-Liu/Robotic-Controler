#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <malloc.h>
#include <math.h>
#include <ecrt.h>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <malloc.h>
#include <sched.h> /* sched_setscheduler() */

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

#include <iostream>
using namespace std;

#include <commu.h>
#include <ecrt.h>
#include "ecat.h"

// int run = 1;
int istest = 0;
unsigned short N = 0;
extern int run;


//for getting time you have to use below parameters
// Application parameters
#define CLOCK_TO_USE CLOCK_MONOTONIC
//#define MEASURE_TIMING


/****************************************************************************/
#define NSEC_PER_SEC (1000000000L)
#define PERIOD_NS (NSEC_PER_SEC / FREQUENCY)
#define DIFF_NS(A, B) (((B).tv_sec - (A).tv_sec) * NSEC_PER_SEC + \
(B).tv_nsec - (A).tv_nsec)
#define TIMESPEC2NS(T) ((uint64_t) (T).tv_sec * NSEC_PER_SEC + (T).tv_nsec)


/****************************************************************************/

// EtherCAT distributed clock variables
const int       cycle_ns = PERIOD_NS; /* 2 ms */
const int64_t   f = FREQUENCY;
#define DC_FILTER_CNT          512

static unsigned int counter = 0;
static unsigned int sync_ref_counter = 0;
const struct timespec cycletime = {0, PERIOD_NS};
static unsigned int servo_flag = 0;
static int numOfSlaves = 0;

const int32_t maxacc = 32292536;

static ec_master_t *master = NULL;
static ec_master_state_t master_state = {};

static ec_domain_t *domain = NULL;
static ec_domain_state_t domain_state = {};

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



//domain registration, you have to register domain so that you can send and receive PDO data
static ec_pdo_entry_reg_t domain_regs[17*NUMSL];

//ec_pdo_entry_info_t slave_pdo_entries[] = {
//    {0x6040, 0x00, 16}, /* Control Word                 Unsigned16 */
//    {0x607a, 0x00, 32}, /* Target Position              Integer32  */
//    {0x60ff, 0x00, 32}, /* Target Velocity */
//    {0x6071, 0x00, 16}, /* Target torque */
//    {0x6060, 0x00, 8}, /* Modes of Operation            Integer8*/
//    {0x5ffe, 0x00, 8}, /* Dummy Byte 1 */
//    {0x60b8, 0x00, 16}, /* Touch probe function */
//    {0x6041, 0x00, 16}, /* Status Word                  Unsigned16 */
//    {0x6064, 0x00, 32}, /* Position Actual Value        Integer32  */
//    {0x606c, 0x00, 32}, /* Velocity Actual Value */
//    {0x6077, 0x00, 16}, /* Torque Actual Value */
//    {0x6061, 0x00, 8}, /* Modes of Operation Display */
//    {0x5fff, 0x00, 8}, /* Dummy Byte 2 */
//    {0x60f4, 0x00, 32}, /* Following Error Actual Value */
//    {0x60fd, 0x00, 32}, /* Digital Inputs */
//    {0x60b9, 0x00, 16}, /* Touch probe status           Unsigned16 */
//    {0x60ba, 0x00, 32}, /* Touch probe pos 1 pos value  Integer32  */
//};

ec_pdo_entry_info_t slave_pdo_entries[17];

//ec_pdo_info_t slave_pdos[] = {
//    {0x1600, 7, slave_pdo_entries + 0}, /* Position Control RxPDO  */
//    {0x1a00, 10, slave_pdo_entries + 7}, /* Position Control TxPDO */
//};

ec_pdo_info_t slave_pdos[2];

ec_sync_info_t slave_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},
    {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
    {2, EC_DIR_OUTPUT, 1, slave_pdos + 0, EC_WD_ENABLE},
    {3, EC_DIR_INPUT, 1, slave_pdos + 1, EC_WD_DISABLE},
    {0xff}
};

struct timespec timespec_add(struct timespec time1, struct timespec time2)
{
	struct timespec result;
	if ((time1.tv_nsec + time2.tv_nsec) >= NSEC_PER_SEC)
	{
		result.tv_sec = time1.tv_sec + time2.tv_sec + 1;
		result.tv_nsec = time1.tv_nsec + time2.tv_nsec - NSEC_PER_SEC;
	}
	else
	{
		result.tv_sec = time1.tv_sec + time2.tv_sec;
		result.tv_nsec = time1.tv_nsec + time2.tv_nsec;
	}
	return result;
}

void endsignal(int sig)
{
	servo_flag = 1;
	signal( SIGINT , SIG_DFL );

}

static void set_latency_target(void)
{
        struct stat s;
        int err;

        err = stat("/dev/cpu_dma_latency", &s);
        if (err == -1) {
                perror("WARN: stat /dev/cpu_dma_latency failed: ");
                return;
        }


        int latency_target_fd = open("/dev/cpu_dma_latency", O_RDWR);
        if (latency_target_fd == -1) {
                perror("WARN: open /dev/cpu_dma_latency: ");
                return;
        }

        int32_t latency_target_value;
        err = write(latency_target_fd, &latency_target_value, 4);
        if (err < 1) {
                printf("# error setting cpu_dma_latency to %d!", latency_target_value);
                close(latency_target_fd);
                return;
        }
        printf("# /dev/cpu_dma_latency set to %dus\n", latency_target_value);
}



/*****************************************************************************
 * Realtime task
 ****************************************************************************/

void check_domain_state()
{
    static ec_domain_state_t domain_state = {}; 
    ec_domain_state_t ds = {};
	ecrt_domain_state(domain, &ds);
    if (ds.working_counter != domain_state.working_counter) {
        printf("domain: WC %u.\n", ds.working_counter);
        // state->WKC = ds.working_counter;
    }
    if (ds.wc_state != domain_state.wc_state) {
        printf("domain: State %u.\n", ds.wc_state);
        // state->WC_state = ds.wc_state;
    }
    domain_state = ds;
}

void check_master_state()
{
    static ec_master_state_t master_state = {};
    ec_master_state_t ms;
	ecrt_master_state(master, &ms);
    if (ms.slaves_responding != master_state.slaves_responding) {
        printf("%u slave(s).\n", ms.slaves_responding);
        // state->slaves_responding = ms.slaves_responding;
    }
    if (ms.al_states != master_state.al_states) {
        printf("AL states: 0x%02X.\n", ms.al_states);
        // state->al_states = ms.al_states;
    }
    if (ms.link_up != master_state.link_up) {
        printf("Link is %s.\n", ms.link_up ? "up" : "down");
        // state->link_up = ms.link_up;
    }
    master_state = ms;
}

int ecat_init(void)
{
	if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1)
    {
		perror("mlockall failed");
		return -1;
    }

    set_latency_target();

	master = ecrt_request_master(0);
	printf("ecrt_request_master is called \n");
	if (!master)
		return -1;

    ecrt_master_state(master, &master_state);
    cout << "num of slaves: " << master_state.slaves_responding << endl;
    cout << "slaves states: " << master_state.al_states << endl;

    ec_slave_info_t slaveInfo;
    for(int i = 0; i < master_state.slaves_responding; i++)
    {
        ecrt_master_get_slave(master, i, &slaveInfo);
        cout << "Slave " << i << ":" << endl
            << "Alias: " << slaveInfo.alias << endl
            << "Position: " << slaveInfo.position << endl
            << "Vendor id: " << slaveInfo.vendor_id << endl
            << "Product code: " << slaveInfo.product_code << endl
            << "name: " << slaveInfo.name << endl <<endl;
    }



    return -1;

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


    domain = ecrt_master_create_domain(master);
	if(!domain)
		return -1;

    for(int i=0;i<NUMSL;i++)
    {
        if(!(sc_CDR[i] = ecrt_master_slave_config(master, 0, i, CoolDrive)))
	    {
		    fprintf(stderr, "Failed to get slave %d configuration. \n",i);
		    return -1;
	    }
        printf("Configuring Slave %d PDOs...\n",i);
	    if (ecrt_slave_config_pdos(sc_CDR[i], EC_END, slave_syncs))
	    {
		    fprintf(stderr, "Failed to configure Slave %d PDOs.\n",i);
		    return -1;
	    }
	    printf("configureing Slave %d PDOs is completed!\n",i);
    }

    if( ecrt_domain_reg_pdo_entry_list(domain, domain_regs))
	{
		fprintf(stderr, "PDO entty registration filed! \n");
		return -1;
	}
    printf("PDO entry registration succeed!\n");
    
    // configure SYNC signals for this slave
    for(int i=0; i<NUMSL; i++)
        ecrt_slave_config_dc(sc_CDR[i], 0x0300, PERIOD_NS, cycletime.tv_nsec/2, 0, 0);

	printf("Activating master...\n");
	if (ecrt_master_activate(master))
		return -1;

    if( !(domain_pd = ecrt_domain_data(domain)))
	{
		return -1;
	}

	return 0;
}


void * ecat_task(void *arg)
{
    if(-1 == ecat_init())
        printf("ethercat init err!\n");

    printf("Starting cyclic function.\n");

    struct timespec wakeupTime, time;


#ifdef MEASURE_TIMING
	struct timespec startTime, endTime, lastStartTime = {};
	uint32_t period_ns = 0, exec_ns = 0, latency_ns = 0,
             latency_min_ns = 0, latency_max_ns = 0,
             period_min_ns = 0, period_max_ns = 0,
             exec_min_ns = 0, exec_max_ns = 0;
#endif // MEASURE_TIMING

	clock_gettime(CLOCK_TO_USE, &wakeupTime); // use to get the clock time

	while(run)
	{
        wakeupTime = timespec_add(wakeupTime, cycletime);
        clock_nanosleep(CLOCK_TO_USE, TIMER_ABSTIME, &wakeupTime, NULL);
        ecrt_master_application_time(master, TIMESPEC2NS(wakeupTime));

#ifdef MEASURE_TIMING
        clock_gettime(CLOCK_TO_USE, &startTime);
        latency_ns = DIFF_NS(wakeupTime, startTime);
        period_ns = DIFF_NS(lastStartTime, startTime);
        exec_ns = DIFF_NS(lastStartTime, endTime);
        lastStartTime = startTime;

        if (latency_ns > latency_max_ns) {
            latency_max_ns = latency_ns;
        }
        if (latency_ns < latency_min_ns) {
            latency_min_ns = latency_ns;
        }
        if (period_ns > period_max_ns) {
            period_max_ns = period_ns;
        }
        if (period_ns < period_min_ns) {
            period_min_ns = period_ns;
        }
        if (exec_ns > exec_max_ns) {
            exec_max_ns = exec_ns;
        }
        if (exec_ns < exec_min_ns) {
            exec_min_ns = exec_ns;
        }
#endif


		ecrt_master_receive(master);
        ecrt_domain_process(domain);
        check_domain_state();

        if (counter) {
            counter--;
        } else { // do this at 1 Hz
            counter = FREQUENCY;

            // check for master state (optional)
            check_master_state();

#ifdef MEASURE_TIMING
            // output timing stats
            printf("period     %10u ... %10u\n",
                    period_min_ns, period_max_ns);
            printf("exec       %10u ... %10u\n",
                    exec_min_ns, exec_max_ns);
            printf("latency    %10u ... %10u\n",
                    latency_min_ns, latency_max_ns);
            period_max_ns = 0;
            period_min_ns = 0xffffffff;
            exec_max_ns = 0;
            exec_min_ns = 0xffffffff;
            latency_max_ns = 0;
            latency_min_ns = 0xffffffff;
#endif
        }

        for(int i=0; i<NUMSL; i++)
        {
            
            /* data->StatusWord[i] = EC_READ_U16(domain_pd + off_stawrd[i]);
            data->ActualPosition[i] = EC_READ_S32(domain_pd + off_actpos[i]);
            data->ActualVelocity[i] = EC_READ_S32(domain_pd + off_actvel[i]);
            EC_WRITE_U16(domain_pd+off_cntlwd[i], data->ControlWord[i]);
            EC_WRITE_S8(domain_pd+off_modopr[i], data->ModeOperation[i]);  */
            EC_READ_U16(domain_pd + off_stawrd[i]);
            EC_WRITE_U16(domain_pd+off_cntlwd[i], 0x0007);

/*            if((err < 0))
            // || (0 == state->isEnable))
            {   // that meas the velocity is zero
                data->TargetPosition[i] = data->ActualPosition[i];
                for(int i=0; i<6; i++){
                    EC_WRITE_S32(domain_pd+off_tarpos[i], data->TargetPosition[i]);
                }
            }
            
            EC_WRITE_S32(domain_pd+off_tarpos[i], data->TargetPosition[i]);
*/
        }
            clock_gettime(CLOCK_TO_USE, &time); //this command should be use for sync dc,
            ecrt_master_application_time(master, TIMESPEC2NS(time)); //sync ethrcat master with current time
            if (sync_ref_counter)
            {
                sync_ref_counter--;
            }
            else
            {
                sync_ref_counter = 1; // sync every cycle

                clock_gettime(CLOCK_TO_USE, &time);
                ecrt_master_sync_reference_clock_to(master, TIMESPEC2NS(time));
            }
            ecrt_master_sync_slave_clocks(master);

            // ecrt_domain_process(domain);
            ecrt_domain_queue(domain); //queue all the data
            ecrt_master_send(master); //send all data to ethercat slave
     
#ifdef MEASURE_TIMING
            clock_gettime(CLOCK_TO_USE, &endTime);
#endif // MEASURE_TIMING

    }
    ecrt_release_master(master);
    // usleep(1000000);
}
