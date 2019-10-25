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
#include <pthread.h>

#include "EcatDrive/EcatDrive.h"
#include "EcatDrive/drive.h"
#include "kinematics/motion.h"

#include "commu.h"

int err = 0;    /* err is the vaule for return of Xenomai-native API */

RT_HEAP     driver_data_heap;
RT_HEAP     driver_stat_heap;
RT_QUEUE    tarpos_queue;
RT_PIPE     teachview_pipe;

RT_TASK     rt_main_task;
RT_TASK     rt_ecat_task;
RT_TASK     rt_drive_task;
RT_TASK     rt_PTP_task;
RT_TASK     rt_LIN_task;
RT_TASK     rt_CIR_task;

void endsignal(int sig)
{
	err = rt_heap_bind(&driver_stat_heap, DRIVE_STATE_HEAP_NAME, TM_INFINITE);
    if(err <0){
        rt_fprintf(stderr, "driver stat bind fail in robot.cpp:endsignal() : %d", err);
    }
    void * stat_block;
    err = rt_heap_alloc(&driver_stat_heap, 0, TM_INFINITE, &stat_block);
    if(err <0){
            rt_fprintf(stderr, "driver stat heap alloc fail in robot.cpp:endsignal() : %d", err);
    }
    driverstate_t * state = (driverstate_t *) stat_block;

    state->stop = 1;

    err = rt_heap_free(&driver_stat_heap, stat_block);
    if(err < 0)
        rt_fprintf(stderr, "stat heap free fail in robot.cpp:endsignal() : %d", err);
    err = rt_heap_unbind(&driver_stat_heap);
    if(err <0){
        rt_fprintf(stderr, "driver stat unbind fail in robot.cpp:endsignal() : %d", err);
    }
}

int main(void)
{
    err = mlockall(MCL_CURRENT|MCL_FUTURE);
    if(err < 0){
        fprintf(stderr,"mlock failed: %s\n", strerror(err));
        return -1;
    }

    err = rt_task_shadow(&rt_main_task,"main_task",50,0);
    if(err < 0){
        rt_fprintf(stderr,"main_task start failed: %s\n",strerror(err));
        return -1;
    }

    err = rt_heap_create(&driver_data_heap, DRIVE_DATA_HEAP_NAME, 
                            sizeof(driverdata_t), H_PRIO|H_SHARED);
    if(err < 0){
        rt_fprintf(stderr, "ceat driverdata heap create failed: %s\n", strerror(err));
        return -1;
    }

    err = rt_heap_create(&driver_stat_heap, DRIVE_DATA_HEAP_NAME,
                            sizeof(driverstate_t), H_PRIO|H_SHARED);
    if(err < 0){
        rt_fprintf(stderr, "ceat driverstat heap create failed: %s\n", strerror(err));
        return -1;
    }

    err = rt_queue_create(&tarpos_queue, TARPOS_QUEUE_NAME, 4*6*1024*1024, 
                            Q_UNLIMITED, Q_PRIO|Q_SHARED);
    if(err < 0){
        rt_fprintf(stderr, "create message queue create failed: %s\n", strerror(err));
        return -1;
    }

    err = rt_task_spawn(&rt_ecat_task,"ecat_task",0,99,T_FPU|T_CPU(0),
                        &ecat_task,NULL);
    if(err < 0){
        rt_fprintf(stderr,"ecat_task start failed: %s\n",strerror(err));
        return -1;
    }
    
/* 
    err = rt_task_spawn(&rt_drive_task,"drive_task",0,99,T_FPU|T_CPU(0),&ecat_task,NULL);
    if(err < 0){
        rt_fprintf(stderr,"ecat_task start failed: %s\n",strerror(err));
        return -1;
    }
 */


    
    signal(SIGTERM, endsignal);
	signal( SIGINT , endsignal );

/*     while(run)
    {
        usleep(100000000);
    }
     */

    faultreset();
    enable();
    usleep(2000000);
    // test(3000000, 1500000, 1500000);
    // shutdown();
    usleep(2000000);
	rt_task_delete(&rt_ecat_task);
    return 0;
}