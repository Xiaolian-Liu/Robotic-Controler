/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionVelocity;
    QAction *actionAccerlation;
    QAction *actionAxis1;
    QAction *actionAxis2;
    QAction *actionAxis3;
    QAction *actionAxis4;
    QAction *actionAxis5;
    QAction *actionAxis6;
    QAction *actionAxisall;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuPlot;
    QMenu *menuPosition;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QDockWidget *dockWidget_3;
    QWidget *dockWidgetContents_3;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1366, 768);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/panda.ico"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/open.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen->setIcon(icon1);
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon2);
        actionVelocity = new QAction(MainWindow);
        actionVelocity->setObjectName(QString::fromUtf8("actionVelocity"));
        actionAccerlation = new QAction(MainWindow);
        actionAccerlation->setObjectName(QString::fromUtf8("actionAccerlation"));
        actionAxis1 = new QAction(MainWindow);
        actionAxis1->setObjectName(QString::fromUtf8("actionAxis1"));
        actionAxis2 = new QAction(MainWindow);
        actionAxis2->setObjectName(QString::fromUtf8("actionAxis2"));
        actionAxis3 = new QAction(MainWindow);
        actionAxis3->setObjectName(QString::fromUtf8("actionAxis3"));
        actionAxis4 = new QAction(MainWindow);
        actionAxis4->setObjectName(QString::fromUtf8("actionAxis4"));
        actionAxis5 = new QAction(MainWindow);
        actionAxis5->setObjectName(QString::fromUtf8("actionAxis5"));
        actionAxis6 = new QAction(MainWindow);
        actionAxis6->setObjectName(QString::fromUtf8("actionAxis6"));
        actionAxisall = new QAction(MainWindow);
        actionAxisall->setObjectName(QString::fromUtf8("actionAxisall"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icons/plot.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAxisall->setIcon(icon3);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1366, 25));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuPlot = new QMenu(menuBar);
        menuPlot->setObjectName(QString::fromUtf8("menuPlot"));
        menuPosition = new QMenu(menuPlot);
        menuPosition->setObjectName(QString::fromUtf8("menuPosition"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);
        dockWidget_3 = new QDockWidget(MainWindow);
        dockWidget_3->setObjectName(QString::fromUtf8("dockWidget_3"));
        dockWidget_3->setMinimumSize(QSize(20, 41));
        dockWidgetContents_3 = new QWidget();
        dockWidgetContents_3->setObjectName(QString::fromUtf8("dockWidgetContents_3"));
        dockWidget_3->setWidget(dockWidgetContents_3);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidget_3);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuPlot->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave);
        menuPlot->addAction(menuPosition->menuAction());
        menuPlot->addAction(actionVelocity);
        menuPlot->addAction(actionAccerlation);
        menuPosition->addAction(actionAxis1);
        menuPosition->addAction(actionAxis2);
        menuPosition->addAction(actionAxis3);
        menuPosition->addAction(actionAxis4);
        menuPosition->addAction(actionAxis5);
        menuPosition->addAction(actionAxis6);
        menuPosition->addAction(actionAxisall);
        mainToolBar->addAction(actionOpen);
        mainToolBar->addAction(actionSave);
        mainToolBar->addAction(actionAxisall);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionOpen->setText(QApplication::translate("MainWindow", "Open", 0, QApplication::UnicodeUTF8));
        actionSave->setText(QApplication::translate("MainWindow", "Save", 0, QApplication::UnicodeUTF8));
        actionVelocity->setText(QApplication::translate("MainWindow", "Velocity", 0, QApplication::UnicodeUTF8));
        actionAccerlation->setText(QApplication::translate("MainWindow", "Accerlation", 0, QApplication::UnicodeUTF8));
        actionAxis1->setText(QApplication::translate("MainWindow", "Axis1", 0, QApplication::UnicodeUTF8));
        actionAxis2->setText(QApplication::translate("MainWindow", "Axis2", 0, QApplication::UnicodeUTF8));
        actionAxis3->setText(QApplication::translate("MainWindow", "Axis3", 0, QApplication::UnicodeUTF8));
        actionAxis4->setText(QApplication::translate("MainWindow", "Axis4", 0, QApplication::UnicodeUTF8));
        actionAxis5->setText(QApplication::translate("MainWindow", "Axis5", 0, QApplication::UnicodeUTF8));
        actionAxis6->setText(QApplication::translate("MainWindow", "Axis6", 0, QApplication::UnicodeUTF8));
        actionAxisall->setText(QApplication::translate("MainWindow", "Axisall", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        menuPlot->setTitle(QApplication::translate("MainWindow", "Plot", 0, QApplication::UnicodeUTF8));
        menuPosition->setTitle(QApplication::translate("MainWindow", "Position", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
