#include "mainwindow.h"
#include <QApplication>
#include <Threads/ReadPipeThread.h>
#include <QObject>
#include "kinematics/trajectory.h"
typedef std::vector<double> vectord;


int main(int argc, char *argv[])
{
//    vectorangle angle;
//    joinpos_t angle0 = {{0,0,0,0,90,0}};
//    cartpos_t pos0;
//    forkine(angle0.joi, &pos0);
//    cartpos_t posf;
//    posf.pe[0] = pos0.pe[0];
//    posf.pe[1] = -1000;
//    posf.pe[2] = pos0.pe[2];
//    posf.Rx0 = pos0.Rx0;
//    posf.Ry0 = pos0.Ry0;
//    posf.Rz0 = pos0.Rz0;
//    lintraj(angle, angle0, posf, 100, 100);
    QApplication a(argc, argv);
    MainWindow w;
    ReadpipeThread pipethread;
    QObject::connect(&w, SIGNAL(windowClosed()), &pipethread, SLOT(terminate()));
    QObject::connect(&w, SIGNAL(windowClosed()), &pipethread, SLOT(quit()));
    w.show();

    return a.exec();
}
