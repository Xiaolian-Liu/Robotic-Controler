#include <stdio.h>  /* while printf is included */
#include <unistd.h> /* while sleep() and uslepp() is include */
#include <math.h>
#include <iostream>


#ifdef XENOMAI

#include <native/task.h>
#include <native/heap.h>
#include <rtdk.h>

#endif


#include <commu.h>
#include "drive.h"

using std::cout;
using std::endl;

#ifdef XENOMAI

RT_HEAP stat_heap;
void * stat_block = NULL;
// driverstate_t * state;
RT_HEAP data_heap;
void * data_block = NULL;
// driverdata_t * data;


int bind_heap(void)
{
    int err = rt_heap_bind(&stat_heap, DRIVE_STATE_HEAP_NAME, TM_INFINITE);
    if(err <0){
        rt_fprintf(stderr, "driver stat bind fail in drive.cpp : %d", err);
    }

    err = rt_heap_bind(&data_heap,DRIVE_DATA_HEAP_NAME,TM_INFINITE);
    if(err < 0){
        rt_fprintf(stderr, "driver dada heap bind fail in drive.cpp: %d", err);
    }
    return err;
}

int unbind_heap(void)
{
    int err = rt_heap_unbind(&stat_heap);
    if(err <0){
        rt_fprintf(stderr, "driver stat unbind fail in drive.cpp : %d", err);
    }

    err = rt_heap_unbind(&data_heap);
    if(err <0){
        rt_fprintf(stderr, "driver dada heap unbind fail in drive.cpp: %d", err);
    }
    return err;
}

driverstate_t * stat_alloc(void)
{
    int err = rt_heap_alloc(&stat_heap, 0, TM_INFINITE, &stat_block);
    if(err <0){
            rt_fprintf(stderr, "driver stat heap alloc fail in drive.cpp : %d", err);
    }
    return (driverstate_t *) stat_block;
}

int stat_free(void)
{
    int err = rt_heap_free(&stat_heap, stat_block);
    if(err < 0)
        rt_fprintf(stderr, "stat heap free fail in EcatDrive.cpp : %d", err);
    return err;
}

driverdata_t * data_alloc(void)
{
    int err = rt_heap_alloc(&data_heap, 0, TM_INFINITE, &data_block);
    if(err <0){
            rt_fprintf(stderr, "driver data alloc fail in drive.cpp : %d", err);
    }
    return (driverdata_t *) data_block;
}

int data_free(void)
{
    int err = rt_heap_free(&data_heap, data_block);
    if(err < 0)
        rt_fprintf(stderr, "data heap free fail in EcatDrive.cpp : %d", err);
    return err;
}



void faultreset(void)
{
    bind_heap();
    while(run)
    {
        driverstate_t * state;
        state = stat_alloc();
        // rt_printf("al_states: %d",state->al_states);
        if(0x08 == (state->al_states & 0x08)) 
        {
            stat_free();
            break;
        }   
        rt_printf("Waiting for Ethercat communication to complete......\n");
        stat_free();
        rt_task_sleep(1000000000);
    }

    while(run)
    {   
        driverdata_t * data;
        data = data_alloc();
        if( ((data->StatusWord[0] & 0x004f) == 0x0040) && ((data->StatusWord[1] & 0x004f) == 0x0040) &&
            ((data->StatusWord[2] & 0x004f) == 0x0040) && ((data->StatusWord[3] & 0x004f) == 0x0040) &&
            ((data->StatusWord[4] & 0x004f) == 0x0040) && ((data->StatusWord[5] & 0x004f) == 0x0040) )
        {
            data_free();
            break;
        }
        for(int i=0; i<6; i++)
        {
            // rt_printf("slave %d status: %d\n", i, data->StatusWord[i]);
            if( (data->StatusWord[i] & 0x0008) == 0x0008 )   ////fault
                data->ControlWord[i] = FAULT_RESET;
        }
        data_free();
        rt_task_sleep(2000000);
    } 
    unbind_heap();
}


void enable(void)
{
    bind_heap();
    while(run)
    {
        driverstate_t * state;
        state = stat_alloc();
        if(0x08 == (state->al_states & 0x08))   
        {
            stat_free();
            break;
        } 
        rt_printf("Waiting for Ethercat communication to complete......\n");
        stat_free();
        rt_task_sleep(1000000000);
    }

    driverdata_t * data;
    data = data_alloc();

    int isfault = 0;
    for(int i=0; i<6; i++)
    {
        if(FAULT == (data->StatusWord[i] & FAULT))
        {
            isfault = 1;
            rt_printf("There are faults in drivers %d, can not enabled\n");
            break;    
        }   
    }
    data_free();

    int enable[6]={0,0,0,0,0,0};
    int enableN = 0;
    while( (!isfault) && run )
    {
        data = data_alloc();
        for(int i=0; i<6; i++)
        {
            data->ModeOperation[i] = CSP;
            if( (data->StatusWord[i] & 0x006f) == 0x0027 )    // opreation enable
            {
                if(0 == enable[i])
                {
                    enable[i] = 1;
                    enableN += 1;
                   rt_printf("Driver %d is enabled\n",i);
                }
            }
            else if( (data->StatusWord[i] & 0x004f) == 0x0040  ) //switch on disable
                data->ControlWord[i] = SHUDOWN;
            else if( (data->StatusWord[i] & 0x006f) == 0x0021  ) // read to switch on
                data->ControlWord[i] = SWITCH_ON;
            else if( (data->StatusWord[i] & 0x006f) == 0x0023  )   //switch on enable
                data->ControlWord[i] = ENABLE_OPERATION;
        } 
        if(6 == enableN)
        {
            data_free();
            rt_printf("6 Drivers are enabled\n");
            break;
        }
        data_free();
        rt_task_sleep(2000000);     
    }
    driverstate_t * state = stat_alloc();
    state->isEnable = 1;
    unbind_heap();
}

void shutdown(void * cookie)
{
    // rt_print_auto_init(1);
    bind_heap();
    while(1)
    {
        driverstate_t * state;
        state = stat_alloc();
        if(0x08 == (state->al_states & 0x08))   
        {
            stat_free();
            break;
        } 
        rt_printf("Waiting for Ethercat communication to complete......\n");
        stat_free();
        rt_task_sleep(1000000000);
    }

    while(1)
    {
        driverdata_t * data;
        data = data_alloc();
        if( ((data->StatusWord[0]&0x006f) == 0x0021) && ((data->StatusWord[1]&0x006f) == 0x0021) &&
            ((data->StatusWord[2]&0x006f) == 0x0021) && ((data->StatusWord[3]&0x006f) == 0x0021) &&
            ((data->StatusWord[4]&0x006f) == 0x0021) && ((data->StatusWord[5]&0x006f) == 0x0021) ) // ready to switch on
        {
            data_free();
            break;
        }    
        for(int i=0; i<6; i++)
        {
            data->ControlWord[i] = SHUDOWN;
        }
        data_free();
        rt_task_sleep(2000000);
    }
    unbind_heap();
}

void driveinit(void * cookie)
{
    rt_printf("driver init start......\n");
    faultreset();
    rt_printf("the fault of drive is reset\n");
    rt_task_sleep(1000000000);
    enable();
    rt_printf("init done!\n");
}
#endif
/*  void test(int32_t a, int32_t v, int32_t x)    
    In this place, double is first considered, because 
    when we caculate, when we multiply, when we divide,
    the double data is more suitable       */
// void test(double a, double v, double x)
// {
//     for(int j=0; j<6; j++)
//         TargetPosition[j] = ActualPosition[j];
//     double T = 1.0/FREQUENCY;
//     if(double(v*v)/a < x)
//     {
//         double t1 = double(v)/a;
//         double t2 = double(x)/v;
//         double t3 = t2 + 2*t1;
//         double t4 = t3 + 2*double(x)/v - double(v)/a;
//         double t5 = t4 + 2*double(v)/a;
//         double t6 = t5 + double(x)/v - double(v)/a;
//         double tf = t6 + double(v)/a;
//         N = tf*FREQUENCY+1;
//         off_TarPosition = new int32_t [N];
//         for(unsigned short i=0; i<N; i++)
//         {
//             double t = i*T;
//             if(t< t1)
//             {
//                 off_TarPosition[i] = a*t*t/2.0;
//             }
//             else if((t >= t1) && (t < t2))
//             {
//                 off_TarPosition[i] = (v*v/2.0)/a + v*(t - t1);
//                 /*  in this place, the (v*v/2.0) may shrink due to
//                 **  v*v storing in int type. if v*v is biger than 2147483647,
//                 we can't get the right value, so the best way is use the double
//                 type.
//                 */
//             }
//             else if((t >= t2) && (t < t3))
//             {
//                 off_TarPosition[i] = x - a*(t-(t2+t3)/2)*(t-(t2+t3)/2)/2;
//             }
//             else if((t >= t3) && (t < t4))
//             {
//                 off_TarPosition[i] = v * ((t3+t4)/2 - t);
//             }
//             else if((t >= t4) && (t < t5))
//             {
//                 off_TarPosition[i] = -x + a*(t-(t4+t5)/2)*(t-(t4+t5)/2)/2;
//             }
//             else if((t >= t5) && (t < t6))
//             {
//                 off_TarPosition[i] = -(v*v/2.0)/a + v*(t - t6);
//             }
//             else
//             {
//                 off_TarPosition[i] = -a*(t-tf)*(t-tf)/2.0;
//             }
//         }
//     }
//     else if((x<=double(v*v)/a) && (x>double(v*v/2)/a))
//     {
//         double t1 = sqrt(double(x)/a);
//         double t2 = 2*t1;
//         double t3 = t2 + double(v)/a;
//         double t4 = t3 + 2*(double(x)- double(v*v/2)/a) / v;
//         double t5 = t4 + double(v)/a;
//         double t6 = t5 + sqrt(double(x)/a);
//         double tf = t6 + sqrt(double(x)/a);
//         N = tf*FREQUENCY+1;
//         off_TarPosition = new int32_t [N];
//         for(unsigned short i=0; i<N; i++)
//         {
//             double t = i*T;
//             if(t< t1)
//             {
//                 off_TarPosition[i] = a*t*t/2.0;
//             }
//             else if((t >= t1) && (t < t2))
//             {
//                 off_TarPosition[i] = x - a*(t2-t)*(t2-t)/2.0;
//             }
//             else if((t >= t2) && (t < t3))
//             {
//                 off_TarPosition[i] = x - a*(t-t2)*(t-t2)/2.0;
//             }
//             else if((t >= t3) && (t < t4))
//             {
//                 off_TarPosition[i] = v * ((t3+t4)/2 - t);
//             }
//             else if((t >= t4) && (t < t5))
//             {
//                 off_TarPosition[i] = -x + a*(t5-t)*(t5-t)/2;
//             }
//             else if((t >= t5) && (t < t6))
//             {
//                 off_TarPosition[i] = -x + a*(t-t5)*(t-t5)/2;
//             }
//             else
//             {
//                 off_TarPosition[i] = -a*(tf-t)*(tf-t)/2.0;
//             }
//         }
//     }
//     else
//     {
//         double t1 = sqrt(double(x)/a);
//         double t2 = 2*t1;
//         double t3 = t2 + sqrt(double(2*x)/a);
//         double t4 = t3 + sqrt(double(2*x)/a);
//         double t5 = t4 + sqrt(double(x)/a);
//         double tf = t5 + sqrt(double(x)/a);
//         N = tf*FREQUENCY+1;
//         off_TarPosition = new int32_t [N];
//         for(unsigned short i=0; i<N; i++)
//         {
//             double t = i*T;
//             if(t< t1)
//             {
//                 off_TarPosition[i] = a*t*t/2.0;
//             }
//             else if((t >= t1) && (t < t2))
//             {
//                 off_TarPosition[i] = x - a*(t2-t)*(t2-t)/2.0;
//             }
//             else if((t >= t2) && (t < t3))
//             {
//                 off_TarPosition[i] = x - a*(t-t2)*(t-t2)/2.0;
//             }
//             else if((t >= t3) && (t < t4))
//             {
//                 off_TarPosition[i] = -x + a*(t4-t)*(t4-t)/2;
//             }
//             else if((t >= t4) && (t < t5))
//             {
//                 off_TarPosition[i] = -x + a*(t-t4)*(t-t4)/2;
//             }
//             else
//             {
//                 off_TarPosition[i] = -a*(tf-t)*(tf-t)/2;
//             }
//         }
//     }
//     istest = 1;
//     // cout << istest << std::endl;
//     sleep(20);
//     istest = 0;
//     usleep(2*cyclens/1000);
//     delete [] off_TarPosition;
// }
