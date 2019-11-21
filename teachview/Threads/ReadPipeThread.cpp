#include "ReadPipeThread.h"
#include <fstream>

QVector<double> tarposition[6];
QVector<double> poscount;
QMutex posMut;

ReadpipeThread::ReadpipeThread()
{
    bstop = false;
#ifdef LINUX
    pipe_fd = open("/dev/rtp0",O_RDWR);/* open will block until the file can't be opened */
    if(pipe_fd < 0){
        perror("pipe open fail");
    }/* open will block until the file can be opened */
    else{
        start();
    }

#else
    pipe_file.setFileName("/dev/rtp0");
    if(pipe_file.open(QFile::ReadWrite)){
        start();
    }
    else{
        perror("pipe open fail");
    }

#endif
}

ReadpipeThread::~ReadpipeThread()
{
    bstop = true;
    wait();
    close(pipe_fd);
}

void ReadpipeThread::run()
{
    datacount = 0;
    std::ofstream dataout("targetposition.txt");
    while(!bstop)
    {

#ifdef LINUX

        int err = read(pipe_fd, &data, sizeof(driverdata_t));
        if(err < 0){
            perror("pipe read fail");
            continue;
        }
#else
        int len = pipe_file.readData(&data, sizeof(driverdata_t));
        if(len < 0){
            perror("pipe read fail");
            continue;
        }
        datacount++;
        posMut.lock();
        for(int i=0; i<6; i++){
            // pos.update(data.TargetPosition);
            tarposition[i].push_back(data.TargetPosition[i]);
            dataout << data.TargetPosition[i] << '\t';
        }
        dataout << std::endl;
        poscount.push_back(datacount);
        posMut.unlock();
        // emit Readsucceed(pos);
    }
}

