#include "ReadPipeThread.h"
#include <fstream>
#include <er20.h>
#define FIFO_NAME "/tmp/my_fifo"

QVector<double> tarposition[6];
QVector<double> runtime;
QMutex posMut;

ReadpipeThread::ReadpipeThread()
{
    bstop = false;
    start();
}


//    pipe_file.setFileName("/dev/rtp0");
//    if(pipe_file.open(QFile::ReadWrite)){
//        start();
//    }
//    else{
//        perror("pipe open fail");
//    }


ReadpipeThread::~ReadpipeThread()
{
    bstop = true;
    wait();
    close(pipe_fd);
}

void ReadpipeThread::run()
{
    pipe_fd = open(FIFO_NAME,O_RDONLY);/* open will block until the file can't be opened */
    if(pipe_fd < 0){
        perror("pipe open fail");
    }/* open will block until the file can be opened */

    datacount = 0;
    incpos_t  inctarpos;
    joinpos_t joitarpos;
//    std::ofstream dataout("targetposition.txt");
    while(!bstop)
    {

        int err = read(pipe_fd, &data, sizeof(driverdata_t));
        if(err < 0){
            perror("pipe read fail");
            break;
        }
        else
        {
            for(int i=0; i<6; i++){
//                inctarpos.inc[i] = data.tarpos[i];
                joitarpos.joi[i] = data.joiangles[i];
            }
//            joitarpos = increment2jointangle(inctarpos);
            datacount++;
            posMut.lock();
            for(int i=0; i<6; i++){
                // pos.update(data.TargetPosition);
                tarposition[i].push_back(double(joitarpos.joi[i]));
    //            dataout << data.TargetPosition[i] << '\t';
            }
    //        dataout << std::endl;
            runtime.push_back(datacount*5);
            posMut.unlock();        // emit Readsucceed(pos);
        }

    }
}

