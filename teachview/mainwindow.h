#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "plot/plotwindow.h"
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QTimer>

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
    QPushButton * plusButton[6];
    QPushButton * minusButton[6];
    QLabel * coordlabel[6];
    QLabel * coordvalue[6];
    QTimer keyscanTimer;
    void createActions();

private slots:
    void plot();
    void updatetext();
    void disable();
    void enable();
    void keyScan();

signals:
    void windowClosed();
};

#endif // MAINWINDOW_H
