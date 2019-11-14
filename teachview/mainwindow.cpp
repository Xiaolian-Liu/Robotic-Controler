#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

void MainWindow::createActions()
{
    connect(ui->actionAxisall,SIGNAL(triggered()), this, SLOT(plot()));
}