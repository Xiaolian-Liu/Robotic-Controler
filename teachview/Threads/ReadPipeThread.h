#ifndef READPIPETHREAD_H
#define READPIPETHREAD_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <QtCore>
//#include "Position.h"
#include <stdint.h>
#include <QVector>
#include <QMutex>
#include <QFile>
#include <QThread>

#define NUMSL   6
typedef struct
{
    uint16_t    ControlWord[NUMSL];
    int32_t     TargetPosition[NUMSL];
    int32_t     TargetVelocity[NUMSL];
    int16_t     TargetTorque[NUMSL];
    int8_t      ModeOperation[NUMSL];

    uint16_t    StatusWord[NUMSL];
    int32_t     ActualPosition[NUMSL];
    int32_t     ActualVelocity[NUMSL];
    int16_t     ActualTorque[NUMSL];
    int8_t      CurrentMode[NUMSL];
    int32_t     ActualFollowErr[NUMSL];
    
}driverdata_t;

extern QVector<double> tarposition[6];
extern QVector<double> runtime;
extern QMutex posMut;
class ReadpipeThread : public QThread
{
    Q_OBJECT

public:
    ReadpipeThread();
    ~ReadpipeThread();
/* 
signals:
    void Readsucceed(const Position &);
 */

protected:
    void run();

private:
    int pipe_fd;
    QFile pipe_file;
    driverdata_t data;
    // Position pos;
    int datacount;
    bool bstop;
};

#endif
