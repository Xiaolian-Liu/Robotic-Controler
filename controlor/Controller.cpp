#include "Controller.hpp"  
#include "commu/ReceiveData.hpp"
#include "commu/TargetData.hpp"
#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <fstream>

// #define MEASURE_TIMING
// #define FILE_OUT

using std::cout;
using std::endl;
using std::ofstream;

Controller::Controller(int freq) : Thread("Controller", 98), 
                                    frequency(freq),
                                    cycleTime(Time::NANO_PER_SEC / freq), 
                                    master(Time::NANO_PER_SEC/freq,0),
                                    posQueue(O_CREAT | O_RDWR | O_NONBLOCK){


}

Controller::~Controller()
{

}
void Controller::run() 
{
    if(-1 == master.init()){
        cout << "controller init master failed" << endl;
        exit();
    }
    master.active();
    int counter = 0;
    Time wakeupTime, time;

#ifdef MEASURE_TIMING
    Time startTime, endTime, lastStartTime;
	uint32_t period_ns = 0, exec_ns = 0, latency_ns = 0,
             latency_min_ns = 0, latency_max_ns = 0,
             period_min_ns = 0, period_max_ns = 0,
             exec_min_ns = 0, exec_max_ns = 0;
#endif // MEASURE_TIMING


    receiveData.init();
    targetData.init();
    posQueue.init();
    statData.init();

#ifdef FILE_OUT
    ofstream of;
    of.open("data.txt");
#endif

    clock_gettime(CLOCK_MONOTONIC, &wakeupTime);
    while(isRun)
    {
        wakeupTime.addNanoSec(cycleTime);

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &wakeupTime, NULL);
        master.setApplicationTime(wakeupTime.totalNanoSec());
#ifdef MEASURE_TIMING
        clock_gettime(CLOCK_MONOTONIC, &startTime);
        // cout << "statrTime:" << startTime << endl;
        latency_ns = Time::diffNanoSec(startTime, wakeupTime);

        // latency_ns = (wakeupTime - startTime).totalNanoSec();
        period_ns = (startTime - lastStartTime).totalNanoSec();
        exec_ns = (endTime - lastStartTime).totalNanoSec();
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

        // receiveData_t recvdata = master.refreshData(receiveData);
        receiveData_t recvdata;

        clock_gettime(CLOCK_MONOTONIC, &time);
        master.sync(time.totalNanoSec());

        // targetData_t tardata = targetData.getData();
        targetData_t tardata;

        incPos_t pos;
        int bytes = posQueue.getPosition(&pos);
        if(bytes == -1)
        {
            for (int i = 0; i < 6; i++)
            {
                tardata.targetPosition[i] = recvdata.actualPosition[i];
            }
        }else
        {
            for (int i = 0; i < 6;i++)
            {
                tardata.targetPosition[i] = pos.targetPosition[i];
            }
        }

#ifdef FILE_OUT
        for (int i = 0; i < 6;i++)
        {
            of << tardata.targetPosition[i] << " ";
        }
        of << endl;
#endif

        master.sendData(tardata);

        if (counter)
        {
            counter--;
        }
        else
        {
            /* code */
            counter = frequency;

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
        

#ifdef MEASURE_TIMING
        clock_gettime(CLOCK_MONOTONIC, (timespec *)&endTime);
#endif // MEASURE_TIMING
    }
}
