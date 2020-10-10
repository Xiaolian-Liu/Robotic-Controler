#include "Controller.hpp"  
#include <iostream>

#define MEASURE_TIMING
#define DIFF_NS(A, B) (((B).tv_sec - (A).tv_sec) * NSEC_PER_SEC + \
(B).tv_nsec - (A).tv_nsec)

using std::cout;
using std::endl;

Controller::Controller(int freq) : Thread("Controller", 98), 
                                    frequency(freq),
                                    cycleTime(Time::NANO_PER_SEC / freq), 
                                    master(Time::NANO_PER_SEC/freq,0){


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
    int counter = 0;
    Time wakeupTime, time;

#ifdef MEASURE_TIMING
	Time startTime, endTime, lastStartTime;
	uint32_t period_ns = 0, exec_ns = 0, latency_ns = 0,
             latency_min_ns = 0, latency_max_ns = 0,
             period_min_ns = 0, period_max_ns = 0,
             exec_min_ns = 0, exec_max_ns = 0;
#endif // MEASURE_TIMING

    clock_gettime(CLOCK_MONOTONIC, (timespec *)&wakeupTime);
    while(isRun)
    {
        wakeupTime += cycleTime;
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, (timespec *)&wakeupTime, NULL);
        // master.setApplicationTime(wakeupTime.totalNanoSec());

#ifdef MEASURE_TIMING
        clock_gettime(CLOCK_MONOTONIC, (timespec *)&startTime);
        latency_ns = (wakeupTime - startTime).totalNanoSec();
        exec_ns = (lastStartTime - endTime).totalNanoSec();
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

        if(counter){
            counter--;
        }else
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
