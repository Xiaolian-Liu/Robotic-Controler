#include "mainwindow.h"
#include <QApplication>
#include <Threads/ReadPipeThread.h>
#include <QObject>

typedef std::vector<double> vectord;


vectord julipt(double x0, double xf, double a, double v, int f)
{
    double xl = abs(xf - x0) - v * v / a;  //the linear segment
    if (xl > 0)
    {
        double t1 = v / a;
        double t2 = t1 + xl / v;
        double tf = t2 + v / a;
        double T = 1.0 / f;
        unsigned long N = tf * f;
        vectord x(N);
        for (unsigned long i = 0; i < N; i++)
        {
            double t = i * T;
            if (t < t1)
            {
                x[i] = (a*t*t / 2)/(xf-x0);
            }
            else if ((t >= t1) && (t < t2))
            {
                x[i] = (v*v / a / 2 + v * (t - t1))/(xf-x0);
            }
            else
            {
                x[i] = (abs(xf-x0) - a*(tf - t)*(tf - t)/2)/(xf -x0);
            }
        }
        return x;
    }
    else
    {
        double t1 = sqrt(abs(xf - x0) / a);
        double tf = 2 * t1;
        double T = 1.0 / f;
        unsigned long N = tf * f;
        vectord x(N);
        for (unsigned long i = 0; i < N; i++)
        {
            double t = i * T;
            if (t < t1)
            {
                x[i] = (a*t*t / 2)/(xf - x0);
            }
            else
            {
                x[i] = (abs(xf-x0) - a*(tf - t)*(tf - t)/2)/(xf-x0);
            }
        }
        return x;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    ReadpipeThread pipethread;
    QObject::connect(&w, SIGNAL(windowClosed()), &pipethread, SLOT(terminate()));
    QObject::connect(&w, SIGNAL(windowClosed()), &pipethread, SLOT(quit()));
    w.show();

    return a.exec();
}
