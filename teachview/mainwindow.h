#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "plot/plotwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    PlotWindow * plotAxis[6];
    PlotWindow * plotAll;
    void createActions();

private slots:
    void plot();

signals:
    void windowClosed();
};

#endif // MAINWINDOW_H
