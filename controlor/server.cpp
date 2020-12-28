#include "server.hpp"
#include <mutex>
#include <stdio.h>
#include "commu/TeachViewData.hpp"

using std::mutex;

server_sendData_t Server::sendData = {0};
server_recvData_t Server::recvData = {};
queue<motionCommand> Server::commandQueue;

void * Server::to_uiclient(void *data)
{
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("Hello\n");
//    mutex mtx;

//    Server *myServer = (Server *)data;
    //myServer->receiveData.init();
//    int client_sockfd = myServer->client_sockfd;
    int client_sockfd = *(int *)data;
    printf("socketfd: %d\n", client_sockfd);

    //axis_wdata.actualPosition[0] = 0;


//    FILE *fp = fopen("/home/ecat/data3.txt", "r");
//    if(!fp)
//    {
//        printf("error\n");
//        printf("error\n");
//        printf("error\n");
//        printf("error\n");
//        printf("error\n");
//        printf("error\n");
//        printf("error\n");
//        printf("error\n");
//        printf("error\n");
//        //return;
//    }

    server_recvData_t recvTemp;

    while (1)
    {
//        usleep(10000);


        /*
         * send actual position
         */
        //mtx.lock();
#if 1

        read(client_sockfd, (char *)&recvTemp, sizeof(recvData));
        if(1 == recvTemp.client_id)
        {
            recvData = recvTemp;
            if(recvData.motionCommandSize > 0)
            {
                for (int i = 0; i < recvData.motionCommandSize; i++)
                {
                    motionCommand command;
                    read(client_sockfd, (char *)&command, sizeof(command));
                    commandQueue.push(command);
                }
            }
        }

//        printf("axis_wdata.actualPosition[4] = %d\n", axis_wdata.actualPosition[4]);
        //printf("%d : actpos = %d\t\n", client_sockfd, axis_wdata.actualposition);
//        axis_wdata.actualPosition[4] = -1;
//        axis_wdata.actualPosition[5] = -2;
        //mtx.unlock();

        write(client_sockfd, (char *)&sendData, sizeof(sendData));

        //axis_wdata.actualPosition[0]++;
        //printf("%d server send\n", d);
        //printf("axis_wdata.actualPosition[0] = %d\n", axis_wdata.actualPosition[0]);
#endif

#if 0
        //axis_wdata.actualposition = act_Pos_Socket;
//        axis_wdata.actualPosition[0] = myServer->receiveData.getData().actualPosition[0];
//        axis_wdata.actualPosition[1] = myServer->receiveData.getData().actualPosition[1];
//        axis_wdata.actualPosition[2] = myServer->receiveData.getData().actualPosition[2];
//        axis_wdata.actualPosition[0] = 0;
//        axis_wdata.actualPosition[1] = 0;
//        axis_wdata.actualPosition[2] = 0;
//        axis_wdata.actualPosition[3] = 0;
//        axis_wdata.actualPosition[4] = 0;
//        axis_wdata.actualPosition[5] = -807869;
//        printf("axis_wdata.actualPosition[4] = %d\n", axis_wdata.actualPosition[4]);
        //printf("%d : actpos = %d\t\n", client_sockfd, axis_wdata.actualposition);
        fscanf(fp, "%d%d%d%d%d%d", &axis_wdata.actualPosition[0], &axis_wdata.actualPosition[1], &axis_wdata.actualPosition[2], &axis_wdata.actualPosition[3], &axis_wdata.actualPosition[4], &axis_wdata.actualPosition[5] );
        //mtx.unlock();
        //axis_wdata.actualPosition[0] = 0;
        printf("%d : actpos = %d\t\n", client_sockfd, axis_wdata.actualPosition[0]);
        printf("%d : actpos = %d\t\n", client_sockfd, axis_wdata.actualPosition[1]);
        printf("%d : actpos = %d\t\n", client_sockfd, axis_wdata.actualPosition[2]);
        int d = write(client_sockfd, (char *)&axis_wdata, sizeof(axis_wdata));
        //printf("%d server send\n", d);
        //printf("axis_wdata.actualPosition[0] = %d\n", axis_wdata.actualPosition[0]);
#endif
        /*
         * recv target position
         */
        //read(myServer->client_sockfd, (char *)&myServer->axis_rdata, sizeof(myServer->axis_rdata));

//        mtx.lock();
//        if(axis_rdata.targetposition != 0x7fffffff)
//            offset_user = axis_rdata.targetposition;
//        printf("%d : offset_user = %d\n", client_sockfd, offset_user);

//        destinePosition = offset_user;
//        printf("%d : destinePosition = %d\n", client_sockfd, destinePosition);
//        //offset_Socket = offset_user;

//        mtx.unlock();

    }
    return NULL;
}

void Server::printMotionCommand(motionCommand * m)
{
    string str;
    switch (m->mode)
    {
    case 0:
        str = "PTP";
        break;
    case 1:
        str = "Lin";
        break;
    case 2:
        str = "Cir";
        break;
    default:
        break;
    }
    printf("MotionType:  %s\n", str.c_str());
    printf("MiddlePos:   %f  %f  %f  %f  %f  %f\n",
     m->PosMid[0], m->PosMid[1], m->PosMid[2],
     m->PosMid[3], m->PosMid[4], m->PosMid[5]);

    printf("FinalPos:    %lf  %lf  %lf  %lf  %lf  %lf\n",
     m->PosFin[0], m->PosFin[1], m->PosFin[2],
     m->PosFin[3], m->PosFin[4], m->PosFin[5]);

    printf("Vel Percent: %d\n", m->velPercent);

    printf("Acc Percent: %d\n", m->accPercent);

}

Server::Server()
{

}

Server::Server(int freq, int portnum) : Thread("Server", 80),
                                        frequency(freq),
                                        port(portnum)
{
    printf("Socket server has been created/n");
//    axis_wdata = {0, 0, 0, 0, 0, 0};
}

Server::~Server()
{

}

void Server::run()
{
    server_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);
    server_len = sizeof(server_address);
    int d = bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
    printf("%d\n", d);
    printf("%d\n", d);
    printf("%d\n", d);
    printf("%d\n", d);
    printf("%d\n", d);
    printf("%d\n", d);
    printf("%d\n", d);

    listen(server_sockfd, 5);

    //receiveData.init();
    //targetData.init();
    //statData.init();

    int i = 0;

    while (isRun)
    {
        usleep(100);
        //Block
        //Waiting for new client connect
        client_len = sizeof(client_address);
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);

        printf("%d\n", d);
        printf("%d\n", d);
        printf("%d\n", d);
        printf("%d\n", d);
        printf("%d\n", d);
        printf("%d\n", d);
        printf("%d\n", d);
        sleep(1);
        //If a new client connects
        //then create a new SocketToUi thread
        printf("A new client connected.\n");

        // pthread_t toUi;
        pthread_create(ui + i, NULL, to_uiclient, (void *)&client_sockfd);

        //        struct sched_param param3;
        //        pthread_attr_t attr3;
        //        pthread_t thread3;
        //        int ret3;

        //        if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1)
        //        {
        //            perror("mlockall failed");
        //            return;
        //        }

        //        ret3 = pthread_attr_init(&attr3);
        //        if(ret3)
        //        {
        //            perror("init pthread attributes failed\n");
        //            //exit(EXIT_FAILURE);
        //        }

        //        ret3 = pthread_attr_setstacksize(&attr3, PTHREAD_STACK_MIN);
        //        if (ret3) {
        //            printf("pthread setstacksize failed\n");
        //            goto out;
        //        }
        //        ret3 = pthread_attr_setschedpolicy(&attr3, SCHED_FIFO);
        //        if (ret3) {
        //            printf("pthread setschedpolicy failed\n");
        //            goto out;
        //        }

        //        param3.sched_priority = 90;
        //        ret3 = pthread_attr_setschedparam(&attr3, &param3);
        //        if (ret3) {
        //            printf("pthread setschedparam failed\n");
        //            goto out;
        //        }

        //        ret3 = pthread_attr_setinheritsched(&attr3, PTHREAD_EXPLICIT_SCHED);
        //        if (ret3) {
        //            printf("pthread setinheritsched failed\n");
        //            goto out;
        //        }

        //        //ret3 = pthread_create(&thread3, &attr3, to_uiclient, (void *)&(this->client_sockfd));
        //        ret3 = pthread_create(&thread3, &attr3, to_uiclient, (void *)this);
        //        if (ret3) {
        //            printf("create pthread failed\n");
        //            goto out;
        //        }
        //    out:
        //        //close(latency_target_fd);
        //        munlockall();
    }
    //return NULL;
}

//void Server::addClient()
//{
//    struct sched_param param3;
//    pthread_attr_t attr3;
//    pthread_t thread3;
//    int ret3;

//    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1)
//    {
//        perror("mlockall failed");
//        return;
//    }

//    ret3 = pthread_attr_init(&attr3);
//    if(ret3)
//    {
//        perror("init pthread attributes failed\n");
//        //exit(EXIT_FAILURE);
//    }

//    ret3 = pthread_attr_setstacksize(&attr3, PTHREAD_STACK_MIN);
//    if (ret3) {
//        printf("pthread setstacksize failed\n");
//        goto out;
//    }
//    ret3 = pthread_attr_setschedpolicy(&attr3, SCHED_FIFO);
//    if (ret3) {
//        printf("pthread setschedpolicy failed\n");
//        goto out;
//    }

//    param3.sched_priority = 90;
//    ret3 = pthread_attr_setschedparam(&attr3, &param3);
//    if (ret3) {
//        printf("pthread setschedparam failed\n");
//        goto out;
//    }

//    ret3 = pthread_attr_setinheritsched(&attr3, PTHREAD_EXPLICIT_SCHED);
//    if (ret3) {
//        printf("pthread setinheritsched failed\n");
//        goto out;
//    }

//    ret3 = pthread_create(&thread3, &attr3, this->to_uiclient, (void *)&client_sockfd);
//    if (ret3) {
//        printf("create pthread failed\n");
//        goto out;
//    }
//out:
//    //close(latency_target_fd);
//    munlockall();
//}

void Server::terminate() 
{
    pthread_cancel(ui[0]);
    pthread_cancel(ui[1]);
    pthread_cancel(threadId);
}

void Server::wait() 
{
    pthread_join(ui[0], nullptr);
    pthread_join(ui[1], nullptr);
    pthread_join(threadId, nullptr);
}
