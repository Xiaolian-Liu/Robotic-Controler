#include "commu.h"
#include <native/pipe.h>
#include <rtdk.h>
#include <native/heap.h>
/* 
void techview_link(void *cookie)
{
    RT_PIPE pipe_desc;
    rt_print_auto_init(1);
    int err = rt_pipe_create(&pipe_desc, TIMELY_DATA_PIPE_NAME, 0, TIMELY_DATA_PIPE_SIZE);
    if(err < 0){
        rt_fprintf(stderr, "pipe creat failed in commu.cpp:techview_link: %d\n", err);
    }

    RT_HEAP data_heap;
    err = rt_heap_bind(&data_heap, DRIVE_DATA_HEAP_NAME, TM_INFINITE);
    if(err < 0){
        rt_fprintf(stderr, "data heap bind failed in commu.cpp:techview_link: %d\n", err);
    }
    while(run)
    {
        
    }
}
 */