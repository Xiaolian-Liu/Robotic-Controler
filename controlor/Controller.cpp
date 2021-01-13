#include "Controller.hpp"  
#include "commu/ReceiveData.hpp"
#include "commu/TargetData.hpp"
#include "commu/PositionQueue.hpp"
#include <iostream>
#include <fstream>
#include "commu/TeachViewData.hpp"
#include "server.hpp"
#include "cmath"
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_NAME "/tmp/my_fifo"

#define ENTITY 



const double qMax[6] =
    {
        90,
        64,
        165,
        178,
        132,
        180
    };

const double qMin[6] =
    {
        -90,
        -142,
        -73,
        -178,
        -132,
        -180
    };

// #define MEASURE_TIMING
// #define FILE_OUT
#define POSITION_QUEUE
// #define FIFO_OUT
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
    if(-1 == init())
    {
        exit();
    }


#ifdef ENTITY
    master.active();
#endif

    int counter = 0;
    Time wakeupTime, time;

    State lastState = Error;
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


    clock_gettime(CLOCK_MONOTONIC, &wakeupTime);
    while(isRun)
    {
        wakeupTime.addNanoSec(cycleTime);

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &wakeupTime, NULL);
#ifdef ENTITY
        master.setApplicationTime(wakeupTime.totalNanoSec());
#endif

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

#ifdef ENTITY
        master.receive();
#else
        for (int i = 0; i < 6; i++)
        {
            master.recvData.actualPosition[i] = master.targData.targetPosition[i];
        }

#endif

        clock_gettime(CLOCK_MONOTONIC, &time);


#ifdef ENTITY
        master.sync(time.totalNanoSec());
#endif

        switch (state)
        {

        case Error:
            error();
            break;

        case Inactive:
            inactive();
            break;

        case Active:
            active();
            break;

        case Moving:
            switch (moveMode)
            {
            case ManualJoint:
                jogJoint();
                break;

            case Auto:
                autoMoving();
                break;

            default:
                break;
            }
            lastState = Moving;
            break;

        default:
            break;
        }
        
        if(master.state.isErrExist)
        {
            state = Error;
        }



#ifdef  ENTITY
        master.send();
#endif

        for (int i = 0; i < 6; i++)
        {
            Server::sendData.actualPosition[i] = master.recvData.actualPosition[i];
        }

        if(lastState != state)
        {
            if(Inactive == state){
                printf("Inactive\n");
            }
            else if(Active == state){
                printf("Active\n");
            }
            else if(Moving == state){
                printf("Moving\n");
            }
            else if(Error == state){
                printf("Error\n");
            }
            lastState = state;
        }


#ifdef FIFO_OUT
                fifoData fifodata;
        for (int i = 0; i < 6; i++)
        {
            fifodata.tarpos[i] = master.targData.targetPosition[i];
            fifodata.joiangles[i] = qNext[i];
        }
        write(pipe_fd, &fifodata, sizeof(fifoData));
#endif
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

void Controller::error() 
{
    if(Server::recvData.faultReset)
    {
        // cout << "FaultRest" << endl;

#ifdef ENTITY

        master.resetSlaveFault();

#else

        master.state.isErrExist = false;

#endif

    }

    if(!master.state.isErrExist)
    {
        state = Inactive;
    }
}

void Controller::inactive() 
{

    if(Server::recvData.enable || enableFlag)
    {
        master.enableSlave();
        enableFlag = true;
    }
    if(master.state.isEnable)
    {
        enableFlag = false;
        state = Active;
    }
    for (int i = 0; i < 6; i++)
    {
        master.targData.targetPosition[i] = master.recvData.actualPosition[i];
    }
    
}

void Controller::active() 
{
    if(Server::recvData.motionMode != moveMode)
    {
        if(0 == Server::recvData.motionMode){
            moveMode = ManualJoint;
        }
        else if(1 == Server::recvData.motionMode){
            moveMode = ManualCart;
        }
        else if(2 == Server::recvData.motionMode){
            moveMode = Auto;
        }
        printMoveMode(moveMode);
    }



    switch (moveMode)
    {
    case ManualJoint:
        if(Server::recvData.jogButton != 0)
        {
            incpos_t incPos;
            for (int i = 0; i < 6; i++)
            {
                incPos.inc[i] = master.recvData.actualPosition[i];
            }
            joinpos_t joiPos = increment2jointangle(incPos);
            // cout << "joiPos: " << joiPos.joi[0] << endl;
            qNext = increment2jointangle(master.recvData.actualPosition);
            // cout << "actualPosition_: " << master.recvData.actualPosition[0];
            // cout << "qNext_Inactive: " << qNext[0] << endl;
            // vNext = increVel2jointVel(master.recvData.actualVelocity);
            vNext << 0, 0, 0, 0, 0, 0;
            // cout << "vNext_Inactive_Inc: " << master.recvData.actualVelocity[0] << endl;
            // cout << "vNext_Inactive: " << vNext[0] << endl;
            state = Moving;
        }
        else
        {
            for (int i = 0; i < 6; i++)
            {
                master.targData.targetPosition[i] = master.recvData.actualPosition[i];
            }
        }
        break;

    case Auto:

        for (int i = 0; i < 6; i++)
        {
            master.targData.targetPosition[i] = master.recvData.actualPosition[i];
        }
       
        if(Server::recvData.startSignal)
        {
            state = Moving;
        }
       
        break;

    default:
        break;
    }
    if(Server::recvData.shutDown)
    {
        master.shutDownSlave();
        state = Inactive;
    }

    lastState = Active;
}

void Controller::jogJoint() 
{
    int jogButton = Server::recvData.jogButton;
    int jogIndex = 0;

    Vmax = (velPercent/100) * VThreshold;
    acc = Vmax/2;
    dec = Vmax * 2;

    if (jogButton > 0)
    {
        jogIndex = jogButton - 1;
        vLast[jogIndex] = vNext[jogIndex];
        vNext[jogIndex] += acc * deltaT;
        if(vNext[jogIndex] > Vmax)
        {
            vNext[jogIndex] = Vmax;
        }
    }
    else if(jogButton < 0)
    {
        jogIndex = -jogButton - 1;
        vLast[jogIndex] = vNext[jogIndex];
        vNext[jogIndex] -= acc * deltaT;
        if(vNext[jogIndex] < -Vmax)
        {
            vNext[jogIndex] = -Vmax;
        }
    }
    else
    {
        vLast = vNext;
        int stopNum = 0;
        for (int i = 0; i < 6; i++)
        {
            vNext[i] -= vNext[i] * dec * deltaT / abs(vNext[i]);
            if(abs(vNext[i]) < 1.5*dec*deltaT)
            {
                vNext[i] = 0;
                stopNum++;
            }
            if(6 == stopNum)
            {
                state = Active;
            }
        }
    }
   

    qNext += 0.5 * deltaT * (vLast + vNext);
    for (int i = 0; i < 6; i++)
    {
        if(qNext[i] > qMax[i]){
            qNext[i] = qMax[i];
        }
        if(qNext[i] < qMin[i]){
            qNext[i] = qMin[i];
        }
    }

    jointangle2increment(master.targData.targetPosition, qNext);

    // q = increment2jointangle(master.recvData.actualPosition);
    // v = increVel2jointVel(master.recvData.actualVelocity);
    // torque = increTor2jointTor(master.recvData.actualTorque);
    // qBuff.push(q);
    // qdBuff.push(v);
    // TorqueBuff.push(torque);



}

void Controller::autoMoving() 
{
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
        // for (int i = 0; i < 6;i++)
        // {
        //     master.targData.targetPosition[i] = pos.targetPosition[i];
        // }

        master.targData.targetPosition[0] = pos.targetPosition[0];
        

        q = increment2jointangle(master.recvData.actualPosition);
        v = increVel2jointVel(master.recvData.actualVelocity);
        torque = increTor2jointTor(master.recvData.actualTorque);
        qBuff.push(q);
        qdBuff.push(v);
        TorqueBuff.push(torque);

        // state = Active;
    }
    if(Server::recvData.stopSignal){
        state = Active;
    }
}

int Controller::init() 
{

#ifdef ENTITY

    if (-1 == master.init())
    {
        cout << "controller init master failed" << endl;
        state = Error;
        return -1;
    }

#endif

    if(-1 == posQueue.init()){
        state = Error;
        cout << "controller init posQueue failed" << endl;
        return -1;
    }

    if(!mkfifo(FIFO_NAME, 0664))
    {
        perror("fifo make failed: ");
    }
    pipe_fd  = open(FIFO_NAME , O_RDWR|O_NONBLOCK); 
    if(-1 == pipe_fd)
    {
        perror("pipe open failed: ");
    }


    
    master.state.isErrExist = false;

    velPercent = 20;

    Vmax = (velPercent/100) * VThreshold;
    acc = Vmax/2;
    dec = Vmax * 2;
    deltaT = 1.0 / frequency;
    q << 0, 0, 0, 0, 0, 0;
    qNext << 0, 0, 0, 0, 0, 0;
    v << 0, 0, 0, 0, 0, 0;
    vNext << 0, 0, 0, 0, 0, 0;

    jointangle2increment(master.recvData.actualPosition, qNext);
    jointangle2increment(master.targData.targetPosition, qNext);

    state = Active;
    enableFlag = false;
    moveMode = ManualJoint;
}