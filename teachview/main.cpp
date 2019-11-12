#include "QcustomPlot/qcustomplot.h"

#include <queue>
#include <QApplication>
// #include <QMainWindow>

#include "mainwindow.h"
#include "ReadPipeThread.h"
#include <QMessageBox>
#include <QObject>

int main(int argc, char *argv[])
{   
    QApplication a(argc, argv);
    qRegisterMetaType<Position>("Position");
    qRegisterMetaType<Position>("Position &");
    ReadpipeThread pipethread;
    MainWindow w;
    QObject::connect(&w, SIGNAL(WindowClose()), &pipethread, SLOT(terminate()));
    w.show();
    return a.exec();
}
