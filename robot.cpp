#include <sys/time.h>
#include <unistd.h>
#include <signal.h>     /* where signal() is included */
#include <sys/mman.h>   /* where mlockall() is include */
#include <rtdk.h>
#include <rtdm/rtdm.h>
#include <native/task.h>
#include <native/sem.h>
#include <native/mutex.h>
#include <native/timer.h>
#include <native/heap.h>    /* where rt_heap_creat is included */
#include <native/queue.h>
#include <native/pipe.h>
#include <native/event.h>
#include <pthread.h>

#include "EcatDrive/EcatDrive.h"
#include "EcatDrive/drive.h"
#include <kinematics/motion.h>

#include "commu.h"


int run = 1;
int err = 0;    /* err is the vaule for return of Xenomai-native API */

RT_HEAP     driver_data_heap;
RT_HEAP     driver_stat_heap;
RT_QUEUE    tarpos_queue;
// RT_PIPE     teachview_pipe;
RT_EVENT    state_event;

// RT_TASK     rt_main_task;
RT_TASK     rt_ecat_task;
RT_TASK     rt_driveinit_task;
RT_TASK     rt_shoutdown_task;
RT_TASK     rt_PTP_task;
RT_TASK     rt_LIN_task;
RT_TASK     rt_CIR_task;

rt_sigset_t sig = SIGTERM|SIGINT;
void endsignal(int sig)
{
    err = rt_task_spawn(&rt_shoutdown_task,"shoutdown_task", 0, 98, 0,
                        &shutdown,NULL);
    if(err < 0){
        fprintf(stderr,"shoutdown start failed: %s\n",strerror(err));
    }
    usleep(200000);
    run = 0;
}
int init()
{
    err = rt_heap_create(&driver_data_heap, DRIVE_DATA_HEAP_NAME, 
                            sizeof(driverdata_t), H_PRIO|H_SHARED);
    if(err < 0){
        fprintf(stderr, "ceat driverdata heap create failed: %s\n", strerror(err));
        return -1;
    }

    err = rt_heap_create(&driver_stat_heap, DRIVE_STATE_HEAP_NAME,
                            sizeof(driverstate_t), H_PRIO|H_SHARED);
    if(err < 0){
        fprintf(stderr, "create driverstat heap create failed: %s\n", strerror(err));
        return -1;
    }

    void * stat_sharm;
    err = rt_heap_alloc(&driver_stat_heap, 0, TM_INFINITE, &stat_sharm);
    if(err < 0){
        fprintf(stderr, "state heap alloc faile in init : %d", err);
        return -1;
    }
    // memset(&stat_sharm, 0, sizeof(driverstate_t));
    driverstate_t * st = (driverstate_t *)stat_sharm;
    memset(st, 0, sizeof(driverstate_t));
    err = rt_heap_free(&driver_stat_heap, stat_sharm);
    if(err < 0){
        fprintf(stderr, "state heap free faile in init : %d", err);
        return -1;
    }
    
    err = rt_queue_create(&tarpos_queue, TARPOS_QUEUE_NAME, 4*6*1024*1024, 
                            Q_UNLIMITED, Q_PRIO|Q_SHARED);
    if(err < 0){
        fprintf(stderr, "create message queue create failed: %s\n", strerror(err));
        return -1;
    }
    
    return 0;
}

int main(void)
{
    signal(SIGTERM, endsignal);
	signal( SIGINT , endsignal );

    // rt_print_auto_init(1);
    err = mlockall(MCL_CURRENT|MCL_FUTURE);
    if(err < 0){
        fprintf(stderr,"mlock failed: %s\n", strerror(err));
        return -1;
    }

    if(init() < 0){
        perror("init fail in main: \n");
        return -1;
    }

    err = rt_task_spawn(&rt_ecat_task,"ecat_task",0,99,T_FPU|T_CPU(0),
                        &ecat_task,NULL);
    if(err < 0){
        fprintf(stderr,"ecat_task start failed: %s\n",strerror(err));
        return -1;
    }
/*     
    incpos_t ip1 = {0, 0, 0, 0, 0, 0};
    err = rt_queue_write(&tarpos_queue, &ip1, sizeof(incpos_t), Q_NORMAL);
		if(err < 0){
			rt_fprintf(stderr, "target position queue write failed in motion.cpp:ptp() : %d", err);
		}
    ip1 = {10, 20, 30, 40, 50, 60};
    err = rt_queue_write(&tarpos_queue, &ip1, sizeof(incpos_t), Q_NORMAL);
		if(err < 0){
			rt_fprintf(stderr, "target position queue write failed in motion.cpp:ptp() : %d", err);
		}
 */

    err = rt_task_spawn(&rt_driveinit_task,"drive_task", 0, 89, 0,
                        &driveinit,NULL);
    if(err < 0){
        fprintf(stderr,"drive_init start failed: %s\n",strerror(err));
        return -1;
    }

    err = rt_task_spawn(&rt_PTP_task,"PTP_task", 0, 90, 0,
                        &PTP,NULL);
    if(err < 0){
        fprintf(stderr,"PTP() start failed: %s\n",strerror(err));
        return -1;
    }    

    while(run)
    {
        sleep(2);
    }

    rt_heap_delete(&driver_data_heap);
    rt_heap_delete(&driver_stat_heap);
    rt_queue_delete(&tarpos_queue);
	rt_task_delete(&rt_ecat_task);
    rt_task_delete(&rt_driveinit_task);
    // rt_task_delete(&rt_main_task);
    return 0;
}