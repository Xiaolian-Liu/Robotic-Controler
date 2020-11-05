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


#include "base/Thread.hpp"
#include "ecat/EthercatMaster.hpp"
#include "base/Time.hpp"
#include "commu/ReceiveData.hpp"
#include "commu/TargetData.hpp"
#include "commu/PositionQueue.hpp"
#include "commu/StateData.hpp"

class Server : public Thread
{
public:
    Server();
    Server(int freq, int portnum);
    ~Server();


private:
    int frequency;
    int port;
    int server_sockfd, client_sockfd;
    socklen_t server_len, client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    ReceiveData receiveData;
    TargetData targetData;
    StateData statData;
    receiveData_t axis_wdata;
    virtual void run();
    static void addClient();

    friend void *to_uiclient(void *data);

    //int client_sockfd;
};

#endif // SERVER_HPP
