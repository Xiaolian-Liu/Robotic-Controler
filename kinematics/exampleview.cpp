/*#include <QApplication>
#include <QMainWindow>
#include <qcustomplot.h>
#include "kinematics.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow window;

    // setup customPlot as central widget of window:
    QCustomPlot customPlot;
    window.setCentralWidget(&customPlot);


    vectord p = julipt(150,700,20,300);
    int N = p.size();

    // create plot (from quadratic plot example):
    QVector<double> x(N), y(N);
    for (int i=0; i<N; i++)
    {
        x[i] = i;
        y[i] =p[i];
    }
    customPlot.addGraph();
    customPlot.graph(0)->setData(x, y);
    customPlot.xAxis->setLabel("x");
    customPlot.yAxis->setLabel("y");
    customPlot.rescaleAxes();

    window.setGeometry(100, 100, 500, 400);
    window.show();
    return a.exec();
}
*/
