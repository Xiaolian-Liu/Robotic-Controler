#include "plotwindow.h"
#include "ui_plotwindow.h"
#include <Threads/ReadPipeThread.h>
#include <QString>

PlotWindow::PlotWindow(QWidget * parent, int index) :
    QMainWindow(parent),
    ui(new Ui::PlotWindow),
    mPlot(0),
    axisi(index)
{
    ui->setupUi(this);
    mPlot = new QCustomPlot(this);
    ui->gridLayout->addWidget(mPlot, 0, 1, 1, 1);

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(0, 0, 0));
    plotGradient.setColorAt(1, QColor(0, 0, 0));
    mPlot->setBackground(plotGradient);

    QColor TextColor(255, 255, 255);
    qreal TextWidth = 0.6;
    mPlot->yAxis->setLabelColor(TextColor);
    mPlot->xAxis->setTickLabelColor(TextColor);
    mPlot->yAxis->setTickLabelColor(TextColor);
    mPlot->xAxis->setBasePen(QPen(TextColor, TextWidth));
    mPlot->yAxis->setBasePen(QPen(TextColor, TextWidth));

    mPlot->xAxis->setTickPen(QPen(TextColor, TextWidth));

    mPlot->yAxis->setTickPen(QPen(TextColor, TextWidth));

    mPlot->xAxis->setSubTickPen(QPen(TextColor, TextWidth));

    mPlot->yAxis->setSubTickPen(QPen(TextColor, TextWidth));

    mPlot->yAxis->setRange(-200000.0, 4000000.0);
    if('a' == axisi)
    {
        for(int i=0; i<6; i++)
        {
            mGraph[i] = mPlot->addGraph(mPlot->xAxis, mPlot->yAxis);
        }
        mGraph[0]->setPen(QPen(QColor(255, 0, 0)));     //red
        //    mGraph[1]->setPen(QPen(QColor(255, 165, 0)));   //orange
        mGraph[1]->setPen(QPen(QColor(255, 255, 0)));
        mGraph[2]->setPen(QPen(QColor(0, 255, 0)));     //GREE
        mGraph[3]->setPen(QPen(QColor(0, 255, 255)));   //cyan1
        mGraph[4]->setPen(QPen(QColor(0, 0, 255)));     //blue
        mGraph[5]->setPen(QPen(QColor(255, 0, 255)));   // purple, magenta
    }
    else
    {
        mGraph[axisi] = mPlot->addGraph(mPlot->xAxis, mPlot->yAxis);
        mGraph[axisi]->setPen(QPen(QColor(255, 0, 0)));
        QString title;
        title.sprintf("Axis %d", axisi);
        setWindowTitle(title);
    }
    connect(&mDataTimer, SIGNAL(timeout()), this, SLOT(timerSlot()));
    mDataTimer.start(100);  /* the cycle time is 100ms */
}

PlotWindow::~PlotWindow()
{
    delete ui;
    delete mPlot;
}

void PlotWindow::timerSlot()
{
    if('a' == axisi)
    {
        posMut.lock();
        for(int i=0; i<6; i++)
        {
            mGraph[i]->setData(poscount, tarposition[i], true);
        }
        posMut.unlock();
        // make key axis range scroll with the data:
        mPlot->xAxis->rescale();
        mPlot->xAxis->setRange(mPlot->xAxis->range().upper, 3000, Qt::AlignRight);
//        for(int i=0; i<6; i++)
//        {
//            mGraph[i]->rescaleValueAxis(false, true);
//        }
    }

    else
    {
        posMut.lock();
        mGraph[axisi]->setData(poscount, tarposition[axisi], true);
        posMut.unlock();
        mPlot->xAxis->rescale();
        mPlot->xAxis->setRange(mPlot->xAxis->range().upper, 3000, Qt::AlignRight);
        mGraph[axisi]->rescaleValueAxis(false, true);
    }

    mPlot->replot();
}

