#include "Controller.hpp"  
#include "commu/ReceiveData.hpp"
#include "commu/TargetData.hpp"
#include "commu/PositionQueue.hpp"
#include <iostream>
#include <fstream>
#include "commu/TeachViewData.hpp"
#include "server.hpp"
#include "cmath"

// #define MEASURE_TIMING
#define FILE_OUT
#define POSITION_QUEUE
using std::cout;
using std::endl;
using std::ofstream;

Controller::Controller(int freq) : Thread("Controller", 98), 
                                    frequency(freq),
                                    cycleTime(Time::NANO_PER_SEC / freq), 
                                    master(Time::NANO_PER_SEC/freq,0){


}

EthercatMaster * Controller::getMaster() 
{
    return &master;
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

#ifdef FILE_OUT
    ofstream ofPos, ofVel;
    ofPos.open("dataPos.txt");
    ofVel.open("dataVel.txt");
#endif

    posQueue.init();

    // double acc = 0.5*pow(2,17);
    // double dec = 1*pow(2,17);
    // double deltaT = 1.0/frequency;
    // double Vmax = 1*pow(2,17);

    // int32_t x0;
    // unsigned long n1 = (Vmax/acc)*frequency;
    // int32_t x1 = x0 + (Vmax*Vmax)/(2*acc);
    // u_int64_t n = 0;

    // int32_t qlast[6];
    // int32_t vnext[6];
    // int32_t qdelta[6];

    for(int i = 0; i < 6; i++)
    {
        master.recvData.actualPosition[i] = 0;
    }


    clock_gettime(CLOCK_MONOTONIC, &wakeupTime);
    while(isRun)
    {
        wakeupTime.addNanoSec(cycleTime);

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &wakeupTime, NULL);
        master.setApplicationTime(wakeupTime.totalNanoSec());

#ifdef MEASURE_TIMING
        clock_gettime(CLOCK_MONOTONIC, &startTime);
        latency_ns = Time::diffNanoSec(startTime, wakeupTime);

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


        // receiveData_t recvdata;
        // stateData_t statedata;
        master.receive();

//        printf("%d\n", master.recvData.actualPosition[0]);
//        cout << "pos: " << master.recvData.actualPosition[0] << endl;
//        cout << "vel: " << master.recvData.actualVelocity[0] << endl;
        for (int i = 0; i < 6; i++)
        {
            Server::sendData.actualPosition[i] = master.recvData.actualPosition[i];
        }

        clock_gettime(CLOCK_MONOTONIC, &time);
        master.sync(time.totalNanoSec());

        // targetData_t tardata = TargetData::getData();
//        for (int i = 0; i < 6; i++)
//        {
////            targetData.targetPosition[i] = receiveData.actualPosition[i];
//            targetData.targetOperationMode[i] = 0x08;
//        }


        // printf("%d\n", Server::recvData.jogButton);

/*
         if(master.state.isEnable)
         {
             int jogButton = Server::recvData.jogButton;
             if(jogButton > 0)
             {
                 cout << "forward: " << jogButton << endl;
                 vnext[jogButton - 1] += acc * deltaT;
                 if(vnext[jogButton - 1] > Vmax)
                 {
                     vnext[jogButton - 1] = Vmax;
                 }
//                 cout << "vmax: " << Vmax << endl;
//                 cout << "vnext: " << vnext[jogButton - 1] << endl;
//                 cout << "vactual: " << master.recvData.actualVelocity[jogButton - 1];
             }
             else if(jogButton < 0)
             {
                 cout << "backward: " << jogButton << endl;
                 vnext[jogButton - 1] -= acc * deltaT;
                 if(vnext[jogButton - 1] < -Vmax)
                 {
                     vnext[jogButton - 1] = -Vmax;
                 }
             }
             else
             {
                 cout << "stop: " << jogButton << endl;
                 for (int i = 0; i < 6; i++)
                 {
                     vnext[i] -= vnext[i] * dec * deltaT / abs(vnext[i]);
                     if(abs(vnext[i]) < dec*deltaT)
                     {
                         vnext[i] = 0;
//                         master.recvData.actualVelocity[i] = 0;
                     }
                 }
//                 cout << "actualVelocity: " << master.recvData.actualVelocity[0] << endl;
//                 cout << "vnext: " << vnext[0] << endl;
             }

             for (int i = 0; i < 6; i++)
             {
//                 master.targData.targetPosition[i] = master.recvData.actualPosition[i]+ (master.recvData.actualVelocity[i] + vnext[i]) * deltaT / 2;

                 master.targData.targetPosition[i] = master.targData.targetPosition[i] + vnext[i] * deltaT;
                 qdelta[i] = (master.targData.targetPosition[i] - qlast[i])*frequency;
                 qlast[i] = master.targData.targetPosition[i];


//                 master.targData.targetPosition[i] = master.recvData.actualPosition[i] + vnext[i] * deltaT;
 //                master.targData.targetPosition[i] = master.recvData.actualPosition[i] + 1000;
             }
//             cout << "actualpos: " << master.recvData.actualPosition[0] << endl;
//             cout << "tarpos: " << master.targData.targetPosition[0] << endl;


         }
*/

#ifdef MANUAL

         if(master.state.isEnable)
         {
             n++;
             if(n < n1)
             {
                 master.targData.targetPosition[0] = x0 + 0.5*acc*(n*deltaT)*(n*deltaT);
             }
             else
             {
                 master.targData.targetPosition[0] = x1 + Vmax*(n-n1)*deltaT;
             }
             cout << "targetPosition: " << master.targData.targetPosition[0] << endl;
         }

         else
         {
             x0 = master.recvData.actualPosition[0];
             n = 0;
             x1 = x0 + (Vmax*Vmax)/(2*acc);
             for (int i = 0; i < 6; i++)
             {
                 master.targData.targetPosition[i] = master.recvData.actualPosition[i];
                 vnext[i] = master.recvData.actualVelocity[i];
             }
         }

#endif
        

#ifdef POSITION_QUEUE

        incPos_t pos;
        int bytes = posQueue.getPosition(&pos, 500);
        if(bytes == -1)
        {
            for (int i = 0; i < 6; i++)
            {
                master.targData.targetPosition[i] = master.recvData.actualPosition[i];
            }
        }
        else
        {
            for (int i = 0; i < 6;i++)
            {
                master.targData.targetPosition[i] = pos.targetPosition[i];
                // tardata.targetPosition[i] = recvdata.actualPosition[i];
            }
        }







#endif
 
        if(Server::recvData.shutDown)
        {
            master.shutDownSlave();
        }

        if(Server::recvData.enable)
        {
            printf("enable\n");
            master.enableSlave();
        }

        if(Server::recvData.faultReset)
        {
            master.resetSlaveFault();
        }


        // cout << "0x" << std::hex << tardata.controlWord[0] << endl;
//        for (int i = 0; i < 6; i++)
//        {
//            tardata.targetPosition[i] = recvdata.actualPosition[i];
//        }
        // master.sendData(tardata);
        master.send();


#ifdef FILE_OUT

        for (int i = 0; i < 6; i++)
        {
            ofPos << master.targData.targetPosition[0] << " ";
        }
        ofPos << endl;

#endif

        if(master.state.isEnable)
        {

//#ifdef FILE_OUT
//            for (int i = 0; i < 3;i++)
//            {
//            // ofPos << master.recvData.actualPosition[0] << " ";
//            ofPos << master.targData.targetPosition[0] << " ";
//            ofPos << master.targData.targetPosition[0] << " ";
//            // ofVel << qdelta[0] << " ";
//            // ofVel << qdelta[0] << " ";
//            }
//            ofPos << endl;
//        // ofVel << endl;
//#endif

        }

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

int Controller::init() 
{
    if(-1 == master.init()){
        cout << "controller init master failed" << endl;
        exit();
    }
    posQueue.init();
    
}
