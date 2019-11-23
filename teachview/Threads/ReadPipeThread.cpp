#include "ReadPipeThread.h"
#include <fstream>
#include <kinematics/er20.h>

QVector<double> tarposition[6];
QVector<double> poscount;
QMutex posMut;

ReadpipeThread::ReadpipeThread()
{
    bstop = false;

    pipe_fd = open("/dev/rtp0",O_RDWR);/* open will block until the file can't be opened */
    if(pipe_fd < 0){
        perror("pipe open fail");
    }/* open will block until the file can be opened */
    else{
        start();
    }


//    pipe_file.setFileName("/dev/rtp0");
//    if(pipe_file.open(QFile::ReadWrite)){
//        start();
//    }
//    else{
//        perror("pipe open fail");
//    }
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
    incpos_t  inctarpos;
    joinpos_t joitarpos;
    std::ofstream dataout("targetposition.txt");
    while(!bstop)
    {

        int err = read(pipe_fd, &data, sizeof(driverdata_t));
        if(err < 0){
            perror("pipe read fail");
            break;
        }
        for(int i=0; i<6; i++){
            inctarpos.inc[i] = data.TargetPosition[i];
        }
        joitarpos = increment2jointangle(inctarpos);
        datacount++;
        posMut.lock();
        for(int i=0; i<6; i++){
            // pos.update(data.TargetPosition);
            tarposition[i].push_back(joitarpos.joi[i]);
            dataout << data.TargetPosition[i] << '\t';
        }
        dataout << std::endl;
        poscount.push_back(datacount);
        posMut.unlock();
        // emit Readsucceed(pos);
    }
}

