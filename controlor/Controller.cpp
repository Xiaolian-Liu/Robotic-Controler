#include "Controller.hpp"  
#include "commu/ReceiveData.hpp"
#include "commu/TargetData.hpp"
#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <fstream>

// #define MEASURE_TIMING
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
    // if(-1 == master.init()){
    //     cout << "controller init master failed" << endl;
    //     exit();
    // }
    // master.active();
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

    receiveData_t d = {0, 0, 0, 0, 0, 0};
    receiveData.writeData(d);
    stateData_t st;
    st.al_states = 0x08;
    st.isEnable = 1;
    statData.writeData(st);

    ofstream of;
    of.open("data.txt");

    clock_gettime(CLOCK_MONOTONIC, &wakeupTime);
    while(isRun)
    {
        wakeupTime.addNanoSec(cycleTime);

        // cout << "wakeupTime:" << wakeupTime << endl;
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &wakeupTime, NULL);
        // master.setApplicationTime(wakeupTime.totalNanoSec());
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

    //    receiveData_t recvdata = master.refreshData(receiveData);
//        receiveData_t recvdata = receiveData.getData();
//        recvdata.actualPosition[0]++;
//        receiveData.writeData(recvdata);

        clock_gettime(CLOCK_MONOTONIC, &time);
//        master.sync(time.totalNanoSec());

//        targetData_t tardata = targetData.getData();
        incPos_t pos;
        int bytes = posQueue.getPosition(&pos);
        if(bytes == -1)
        {
            // perror("posQueue read failed");
            for (int i = 0; i < 6; i++){
                pos.targetPosition[i] = d.actualPosition[i];
            }
        }

        // cout << "EtherCAT master thread, target position: " << pos.targetPosition[0] << endl;
        for (int i = 0; i < 6;i++)
        {
            d.actualPosition[i] = pos.targetPosition[i];
            of << pos.targetPosition[i] << " ";
        }
        of << endl;
        //        for (int i = 0; i < 6; i++)
        //        {

        //            tardata.targetPosition[i] = recvdata.actualPosition[i];
        //            tardata.targetOperationMode[i] = 0x08;
        //        }

        //            master.sendData(tardata);
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
