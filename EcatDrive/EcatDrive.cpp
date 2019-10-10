#include <iostream>
#include "ecrt.h"
#include "EcatDrive.h"
using std::cout;
using std::endl;

long cycletime = PERIOD_NS;
RT_TASK my_task;
int run = 1;
int istest = 0;
unsigned short N = 0;

uint8_t ALstate = 0;
uint16_t StatusWord[6] = {0}, ControlWord[6] = {0};
int32_t ActualPosition[6] = {0};
int32_t TargetPosition[6] = {0};
int8_t OperationMode[6] = { 0x08, 0x08, 0x08, 0x08, 0x08, 0x08 };
int32_t *off_TarPosition = NULL;

static ec_master_t *master = NULL;
//static ec_master_state_t master_state = {};

static ec_domain_t *domain = NULL;
//static ec_domain_state_t domain_state = {};

static ec_slave_config_t *sc_CDR[6] = {NULL};

// process data point
static uint8_t *domain_pd = NULL;

// offsets for PDO entries
static unsigned int off_cntlwd[6];
static unsigned int off_tarpos[6];
static unsigned int off_tarvel[6];
static unsigned int off_tartor[6];
static unsigned int off_modopr[6];
static unsigned int off_dubyt1[6];
static unsigned int off_toprfu[6];

static unsigned int off_stawrd[6];
static unsigned int off_actpos[6];
static unsigned int off_actvel[6];
static unsigned int off_acttor[6];
static unsigned int off_moddis[6];
static unsigned int off_dubyt2[6];
static unsigned int off_floerr[6];
static unsigned int off_diginp[6];
static unsigned int off_toprst[6];
static unsigned int off_toprpo[6];

//domain registration, you have to register domain so that you can send and receive PDO data
static ec_pdo_entry_reg_t domain_regs[102];

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


void endsignal(int sig)
{
	run = 0;
	usleep(5e5);
	rt_task_delete(&my_task);
	exit(1);
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
    }
    if (ds.wc_state != domain_state.wc_state) {
        rt_printf("domain: State %u.\n", ds.wc_state);
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
    }
    if (ms.al_states != master_state.al_states) {
        rt_printf("AL states: 0x%02X.\n", ms.al_states);
        ALstate = ms.al_states;
    }
    if (ms.link_up != master_state.link_up) {
        rt_printf("Link is %s.\n", ms.link_up ? "up" : "down");
    }
    master_state = ms;
}

double sync_master_clock(void)
{
    static int disregard = 6;
    static uint64_t dc_time_ns = 0, prev_dc_time_ns = 0;
    static uint64_t mastertime = 0;
    static int64_t err_mastertime = 0;
    static uint32_t ref_time = 0, prev_reftime = 0;
    static uint64_t reftime = 0;
    static int64_t err_reftime = 0;

    prev_dc_time_ns = dc_time_ns;
    dc_time_ns = rt_timer_read();
    ecrt_master_application_time(master,dc_time_ns);
    if(!disregard){
        mastertime += dc_time_ns-prev_dc_time_ns;
        err_mastertime += dc_time_ns-prev_dc_time_ns - cycletime;
    }

    prev_reftime = ref_time;
    ecrt_master_reference_clock_time(master, &ref_time);
    if(!disregard)
    {
        if( (ref_time-prev_reftime) < 0 )
        {
            reftime += ref_time + 0xffffffff - prev_reftime ;
            err_reftime += ref_time + 0xffffffff - prev_reftime - cycletime;
        }
        else
        {
            reftime += ref_time - prev_reftime;
            err_reftime += ref_time - prev_reftime - cycletime;
        }
    }
    else{
        disregard--;
    }
    /*
    rt_printf("%llu\t\t",mastertime);
    rt_printf("%lld\t\t",err_mastertime);
    rt_printf("%llu\t\t",reftime);
    rt_printf("%lld\n",err_reftime);
    */
    return Kp*err_reftime;
}


int ecat_init(void)
{
    for(uint16_t i=0; i<6; i++)
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

    rt_print_auto_init(1);
    signal(SIGTERM, endsignal);
	signal( SIGINT , endsignal );

	/*for the real time application, use the mlockall function */

	if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1)
    {
		perror("mlockall failed");
		return -1;
    }

	master = ecrt_request_master(0);
	printf("ecrt_request_master is called \n");
	if (!master)
		return -1;

    domain = ecrt_master_create_domain(master);
	if(!domain)
		return -1;

    for(int i=0;i<6;i++)
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
    printf("PDO entry registration succeed!");
    
    // configure SYNC signals for this slave
    for(int i=0; i<6; i++)
        ecrt_slave_config_dc(sc_CDR[i], 0x0300, PERIOD_NS,cycletime/2, 0, 0);

	printf("Activating master...\n");
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
    if(-1 == ecat_init())
        printf("ethercat init err!");
    RTIME wakeupTime;                   // get current time
	wakeupTime = rt_timer_read();       // use to get the clock time
    printf("Starting cyclic function.\n");
    istest = 0;
    unsigned short j =0;
    int k = 0;
	while(run)
	{
		wakeupTime += (RTIME)cycletime;
		rt_task_sleep_until(wakeupTime);
		ecrt_master_receive(master);
		ecrt_domain_process(domain);
        rt_check_domain_state(domain);
        for(int i=0; i<6; i++)
        {
            StatusWord[i] = EC_READ_U16(domain_pd + off_stawrd[i]);
    //        cout << std::hex << StatusWord[i] << endl;
            ActualPosition[i] = EC_READ_S32(domain_pd + off_actpos[i]);
    //        cout << Actualposition[i] << endl;
            EC_WRITE_U16(domain_pd+off_cntlwd[i], ControlWord[i]);
            EC_WRITE_S8(domain_pd+off_modopr[i], OperationMode[i]); 
            if(istest)
            {
                int32_t tarpos = TargetPosition[i] + off_TarPosition[j];
                EC_WRITE_S32(domain_pd+off_tarpos[i],tarpos);
                if(0 == i)
                cout << "j: " << j << '\t' << off_TarPosition[j] << endl;
            }
            else
            {
                EC_WRITE_S32(domain_pd+off_tarpos[i],ActualPosition[i]);
            }
        }
        if(istest) 
        {
            j++;
            if(j == N) 
            {
                j = 0;
                k++;
            }
            if(3 == k)
            {
                istest = 0;
            }
        }
        rt_check_master_state(master);
        wakeupTime -= sync_master_clock();
        ecrt_master_sync_slave_clocks(master);
        ecrt_domain_process(domain);
		ecrt_domain_queue(domain); //queue all the data
		ecrt_master_send(master); //send all data to ethercat slave
	}
    ecrt_release_master(master);
}