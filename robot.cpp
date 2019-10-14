#include <sys/time.h>
#include <unistd.h>
#include <signal.h>     /* where signal() is included */
#include <rtdm/rtdm.h>
#include <native/task.h>
#include <native/sem.h>
#include <native/mutex.h>
#include <native/timer.h>
#include <rtdk.h>
#include <pthread.h>

#include "EcatDrive/EcatDrive.h"
#include "EcatDrive/drive.h"
#include "kinematics/motion.h"

RT_TASK my_task;

void endsignal(int sig)
{
	run = 0;
}

int main(void)
{
    int ret = rt_task_create(&my_task,"The ecat task",0,99,T_FPU|T_CPU(0));
    if(ret < 0)
    {
        fprintf(stderr, "Failed to create ecat_task: %s\n", strerror(-ret));
        return -1;
    }
    ret = rt_task_start(&my_task, &ecat_task,NULL);
    if(ret < 0)
    {
        fprintf(stderr, "Failed to start ecat_task: %s\n", strerror(-ret));
        return -1;
    }
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
    test(3000000, 1500000, 1500000);
    shutdown();
    usleep(2000000);
	rt_task_delete(&my_task);
    return 0;
}