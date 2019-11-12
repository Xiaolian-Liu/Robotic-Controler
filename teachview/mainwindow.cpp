#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <ReadPipeThread.h>

/* Attentation: Ui::MainWindow is define in ui_mainwindow.h 
  while MainWindow is defined in mainwindow.h */

/* 构造函数初始化列表 */
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  mPlot(0)
{

  ui->setupUi(this); //ceat the window defined in mainwindow.ui
  
  // mPlot = new QCustomPlot(this->ui->widget);
  mPlot = new QCustomPlot(this);
  mPlot->setGeometry(ui->widget->frameGeometry());
  // ui->widget->se(mPlot);
  // setCentralWidget(mPlot);
  // configure plot to have two right axes:
  mPlot->yAxis->setTickLabels(false);
  connect(mPlot->yAxis2, SIGNAL(rangeChanged(QCPRange)), mPlot->yAxis, SLOT(setRange(QCPRange))); // left axis only mirrors inner right axis
  mPlot->yAxis2->setVisible(true);
/* 
  for(int i=0; i<5; i++){
    mPlot->axisRect()->addAxis(QCPAxis::atRight);
  }
   */
/* 
  for(int i=0; i<6; i++){
    mPlot->axisRect()->axis(QCPAxis::atRight, i)->setPadding(5); // add some padding to have space for tags
  }
 */  

  // create graphs:
  for(int i=0; i<6; i++)
  {
    // mGraph[i] = mPlot->addGraph(mPlot->xAxis, mPlot->axisRect()->axis(QCPAxis::atRight, i));
    mGraph[i] = mPlot->addGraph(mPlot->xAxis, mPlot->yAxis2);
  }
  
  mGraph[0]->setPen(QPen(QColor(255, 0, 0)));
  mGraph[1]->setPen(QPen(QColor(255, 140, 0)));
  mGraph[2]->setPen(QPen(QColor(255, 255, 0)));
  mGraph[3]->setPen(QPen(QColor(0, 255, 0)));
  mGraph[4]->setPen(QPen(QColor(0, 255, 255)));
  mGraph[5]->setPen(QPen(QColor(0, 0, 255)));
//  mGraph[6]->setPen(QPen(QColor(160, 32, 240)));
  
  // create tags with newly introduced AxisTag class (see axistag.h/.cpp):
//  for(int i=0; i<6; i++){
//    mTag[i] = new AxisTag(mGraph[i]->valueAxis());
//    mTag[i]->setPen(mGraph[i]->pen());
//  }
  
  connect(&mDataTimer, SIGNAL(timeout()), this, SLOT(timerSlot()));
  mDataTimer.start(100);  /* the cycle time is 100ms */
}

MainWindow::~MainWindow()
{
  delete ui;
//  WindowClose();
}

void MainWindow::timerSlot()
{
  // calculate and add a new data point to each graph:
  posMut.lock();
  for(int i=0; i<6; i++)
  {
    
    // mGraph[i]->addData(mGraph[i]->dataCount(), p.incpos(i));
    mGraph[i]->setData(poscount, tarposition[i], true);
  }
  posMut.unlock();
  // make key axis range scroll with the data:
  mPlot->xAxis->rescale();
  for(int i=0; i<6; i++)
  {
    mGraph[i]->rescaleValueAxis(false, true);
  }
  // mPlot->xAxis->setRange(mPlot->xAxis->range().upper, 100, Qt::AlignRight);
  
  // update the vertical axis tag positions and texts to match the rightmost data point of the graphs:
//  int32_t graphvalue[6];
//  for(int i=0; i<6; i++)
//  {
//    graphvalue[i] = mGraph[i]->dataMainValue(mGraph[i]->dataCount()-1);
//    mTag[i]->updatePosition(graphvalue[i]);
//    mTag[i]->setText(QString::number(graphvalue[i], 'd', 7));
//  }
  mPlot->replot();
}
