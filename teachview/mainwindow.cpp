#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    plusButton[0] = ui->plusButton1;
    plusButton[1] = ui->plusButton2;
    plusButton[2] = ui->plusButton3;
    plusButton[3] = ui->plusButton4;
    plusButton[4] = ui->plusButton5;
    plusButton[5] = ui->plusButton6;

    minusButton[0] = ui->minusButton1;
    minusButton[1] = ui->minusButton2;
    minusButton[2] = ui->minusButton3;
    minusButton[3] = ui->minusButton4;
    minusButton[4] = ui->minusButton5;
    minusButton[5] = ui->minusButton6;

    plotAll = 0;
    for(int i=0; i<6; i++)
    {
        plotAxis[i] = 0;
    }
    createActions();
}

MainWindow::~MainWindow()
{
    delete ui;
    windowClosed();
}

void MainWindow::plot()
{
    if(!plotAll)
    {
        plotAll = new PlotWindow(this);
    }
    plotAll->show();
}

void MainWindow::updatetext()
{
    ui->coordlabel1->setText("a");
}

void MainWindow::enable()
{
    for(int i=0; i<6; i++)
    {
        plusButton[i]->setEnabled(true);
        minusButton[i]->setEnabled(true);
        ui->actionRun->setEnabled(true);
        ui->actionStop->setEnabled(true);
        ui->actionPause->setEnabled(true);
    }
}

void MainWindow::disable()
{
    for(int i=0; i<6; i++)
    {
        plusButton[i]->setDisabled(true);
        minusButton[i]->setDisabled((true));
        ui->actionRun->setDisabled(true);
        ui->actionStop->setDisabled(true);
        ui->actionPause->setDisabled(true);
    }
}

void MainWindow::keyScan()
{
    if(plusButton[0]->isDown())
    {
        ui->coordvalue1->setText("1");
    }
    else
    {
        ui->coordvalue1->setText("0");
    }
}

void MainWindow::createActions()
{
    connect(ui->actionAxisall,SIGNAL(triggered()), this, SLOT(plot()));
    connect(ui->actionEnable, SIGNAL(triggered()), this, SLOT(enable()));
    connect(ui->actionDisable, SIGNAL(triggered()), this, SLOT(disable()));
    connect(&keyscanTimer, SIGNAL(timeout()), this, SLOT(keyScan()));
    keyscanTimer.start(200);
}
