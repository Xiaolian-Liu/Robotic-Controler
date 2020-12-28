#ifndef SERVER_HPP
#define SERVER_HPP

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/resource.h>
#include <iostream>

#include <sys/time.h>

/****************************************************************************/

//#include "ecrt.h"

#include <semaphore.h>

#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>
#include <queue>


#include "base/Thread.hpp"
#include "ecat/EthercatMaster.hpp"
#include "base/Time.hpp"
#include "commu/ReceiveData.hpp"
#include "commu/TargetData.hpp"
#include "commu/PositionQueue.hpp"
#include "commu/StateData.hpp"

using std::queue;

struct server_sendData_t
{
    uint16_t statusWrod[6];
    uint8_t actualOperationMode[6];
    uint8_t alState;
    int32_t cia402ErrorCode;
    uint8_t journeyPercent;     //percentage of PTP, Lin, Cir ......

    int32_t actualPosition[6];
    double jointAngles[6];
    double jointVelocity[6];
    double jointAcceleration[6];
    double jointTorque[6];
    double cartPosition[6];
};

struct server_recvData_t
{
    uint8_t client_id;
    int32_t targetPosition[6];
    int32_t targetVelocity[6];
    int16_t targetTorque[6];
    uint8_t targetOperationMode[6];

    uint8_t motionCommandSize = 0;

    bool startSignal;
    bool stopSignal;
    bool enable;
    bool shutDown;
    bool faultReset;

    int8_t motionMode;  // 0 manual_Joint_Space
                        // 1 manual_Cartesian_Space
                        // 2 continuous

    uint8_t velocityPercent;
    uint8_t accelerationPercent;

    int8_t jogButton;   //  1   Axis1 +
                        //  -1  Axis1 -
                        //  2   Axis2 +
                        //  -2  Axis2 -
                        //  3   Axis3 +
                        //  4   Axis3 -    

    //    Point endPoint;
    //    Point middlePoint;

    uint8_t ethercatStateMechine[6];
    int32_t ethercatErrorCode[6];
};

struct motionCommand
{
    uint8_t mode; // 0 -- PTP, 1 -- Lin, 2 -- Cir
    double PosMid[6];
    double PosFin[6];
    uint8_t velPercent;
    uint8_t accPercent;
};


class Server : public Thread
{
public:
    Server();
    Server(int freq, int portnum);
    ~Server();

    void terminate();
    void wait();
    
    static queue<motionCommand> commandQueue;
    static server_sendData_t sendData;
    static server_recvData_t recvData;
    
    static void *to_uiclient(void *data);
    static void printMotionCommand(motionCommand * m);

private:
    int frequency;
    int port;
    int server_sockfd, client_sockfd;
    socklen_t server_len, client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    pthread_t ui[2];

    virtual void run();

};

#endif // SERVER_HPP
