#include <sys/time.h>
#include <unistd.h>
#include "EcatDrive/EcatDrive.h"
#include "EcatDrive/drive.h"
#include "kinematics/motion.h"

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
    
    faultreset();
    enable();
    usleep(2000000);
    test(2000000, 800000, 800000);
    usleep(100000000);

    return 0;
}