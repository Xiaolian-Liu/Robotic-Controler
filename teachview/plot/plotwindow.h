#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H
#include <QMainWindow>
#include "qcustomplot.h"

namespace Ui {
class PlotWindow;
}
class PlotWindow : public QMainWindow
{
    Q_OBJECT

public:
    PlotWindow(QWidget * parent, int index = 'a');
    ~PlotWindow();

private:
    Ui::PlotWindow *ui;
    QCustomPlot *mPlot;
    QPointer<QCPGraph> mGraph[6];
    QTimer mDataTimer;
    int axisi;

private slots:
    void timerSlot();
};

#endif // PLOTWINDOW_H
