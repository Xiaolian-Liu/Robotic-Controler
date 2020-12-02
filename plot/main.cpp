#include "mainwindow.h"
#include <QApplication>
#include "qcustomplot.h"
#include <QWidget>
#include <QHBoxLayout>
#include <fstream>
using std::endl;
using std::ifstream;

void set(QCustomPlot * p1)
{
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(0, 0, 0));
    plotGradient.setColorAt(1, QColor(0, 0, 0));
    p1->setBackground(plotGradient);
    QColor TextColor(255, 255, 255);
    qreal TextWidth = 2;
    p1->yAxis->setLabelColor(TextColor);
    p1->xAxis->setTickLabelColor(TextColor);
    p1->yAxis->setTickLabelColor(TextColor);
    p1->xAxis->setBasePen(QPen(TextColor, TextWidth));
    p1->yAxis->setBasePen(QPen(TextColor, TextWidth));

    p1->xAxis->setTickPen(QPen(TextColor, TextWidth));

    p1->yAxis->setTickPen(QPen(TextColor, TextWidth));

    p1->xAxis->setSubTickPen(QPen(TextColor, TextWidth));

    p1->yAxis->setSubTickPen(QPen(TextColor, TextWidth));
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
//    QCustomPlot mplot1();
//    mplot1.show();
    QWidget *w1 = new QWidget;
    w1->resize(1366, 768);

    QCustomPlot *p1 = new QCustomPlot;
//    QLinearGradient plotGradient;
//    plotGradient.setStart(0, 0);
//    plotGradient.setFinalStop(0, 350);
//    plotGradient.setColorAt(0, QColor(0, 0, 0));
//    plotGradient.setColorAt(1, QColor(0, 0, 0));
//    p1->setBackground(plotGradient);
//    QColor TextColor(255, 255, 255);
//    qreal TextWidth = 1.2;
//    p1->yAxis->setLabelColor(TextColor);
//    p1->xAxis->setTickLabelColor(TextColor);
//    p1->yAxis->setTickLabelColor(TextColor);
//    p1->xAxis->setBasePen(QPen(TextColor, TextWidth));
//    p1->yAxis->setBasePen(QPen(TextColor, TextWidth));

//    p1->xAxis->setTickPen(QPen(TextColor, TextWidth));

//    p1->yAxis->setTickPen(QPen(TextColor, TextWidth));

//    p1->xAxis->setSubTickPen(QPen(TextColor, TextWidth));

//    p1->yAxis->setSubTickPen(QPen(TextColor, TextWidth));
    set(p1);
    
    QPointer<QCPGraph> mGraph1[6];

    for(int i=0; i<6; i++)
    {
        mGraph1[i] = p1->addGraph(p1->xAxis, p1->yAxis);
    }
    mGraph1[0]->setPen(QPen(QColor(255, 0, 0)));     //red
    //    mGraph[1]->setPen(QPen(QColor(255, 165, 0)));   //orange
    mGraph1[1]->setPen(QPen(QColor(255, 255, 0)));
    mGraph1[2]->setPen(QPen(QColor(0, 255, 0)));     //GREE
    mGraph1[3]->setPen(QPen(QColor(0, 255, 255)));   //cyan1
    mGraph1[4]->setPen(QPen(QColor(0, 0, 255)));     //blue
    mGraph1[5]->setPen(QPen(QColor(255, 0, 255)));   // purple, magenta

    ifstream if1;		//
    if1.open("dataPos.txt");//
    QVector<double> incVec[6], incNum[6];
    double inc[6];
    int num = 0;
    while (if1 >> inc[0])
    {
        incNum[0].push_back(num);
        incVec[0].push_back(inc[0]);
        for (int i = 1; i < 6; i++)
        {
            if1 >> inc[i];
            incVec[i].push_back(inc[i]);
            incNum[i].push_back(num);
        }
        num++;
    }
    if1.close();
    for (int i = 0; i < 6; i++)
    {
        mGraph1[i]->addData(incNum[i], incVec[i]);
    }
    p1->rescaleAxes(true);
    p1->legend->setVisible(true);


    QCustomPlot *p2 = new QCustomPlot;
    set(p2);

    QPointer<QCPGraph> mGraph2[6];

    for(int i=0; i<6; i++)
    {
        mGraph2[i] = p2->addGraph(p2->xAxis, p2->yAxis);
    }
    mGraph2[0]->setPen(QPen(QColor(255, 0, 0)));     //red
    //    mGraph[1]->setPen(QPen(QColor(255, 165, 0)));   //orange
    mGraph2[1]->setPen(QPen(QColor(255, 255, 0)));
    mGraph2[2]->setPen(QPen(QColor(0, 255, 0)));     //GREE
    mGraph2[3]->setPen(QPen(QColor(0, 255, 255)));   //cyan1
    mGraph2[4]->setPen(QPen(QColor(0, 0, 255)));     //blue
    mGraph2[5]->setPen(QPen(QColor(255, 0, 255)));   // purple, magenta

    ifstream if2;
    if2.open("angles.txt");
    QVector<double> anglesVec[6], anglesNum[6];
    double angle[6];
    num = 0;
    while(if2 >> angle[0])
    {
        anglesNum[0].push_back(num);
        anglesVec[0].push_back(angle[0]);
        for(int i=1; i<6; i++)
        {
            if2 >> angle[i];
            anglesVec[i].push_back(angle[i]);
            anglesNum[i].push_back(num);
        }
        num++;
    }
    if2.close();


    for(int i = 0; i<6; i++)
    {
        mGraph2[i]->addData(anglesNum[i], anglesVec[i]);
    }
    p2->rescaleAxes(true);
    p2->legend->setVisible(true);

    QHBoxLayout *Hlayout = new QHBoxLayout;
    Hlayout->addWidget(p1);
    Hlayout->addWidget(p2);
    w1->setLayout(Hlayout);
    
    w1->show();
    return a.exec();
}
