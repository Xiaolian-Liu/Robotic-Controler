#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QcustomPlot/qcustomplot.h"
// #include "axistag.h"
// #include <queue>
// #include <Position.h>

/* fist creat a Ui::Main */
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
/*   
signals:
  void WindowClose();
 */

private slots:
  void timerSlot();
  
private:
  Ui::MainWindow *ui; /* in this place, ui->mainwindow.ui */
  QCustomPlot *mPlot;
  QPointer<QCPGraph> mGraph[6];
  QTimer mDataTimer;
};


#endif // MAINWINDOW_H
