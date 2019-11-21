/********************************************************************************
** Form generated from reading UI file 'plotwindow.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLOTWINDOW_H
#define UI_PLOTWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QScrollBar>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlotWindow
{
public:
    QAction *actionSave;
    QAction *actionZoom_in;
    QAction *actionZoom_out;
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QWidget *widget;
    QScrollBar *verticalScrollBar;
    QScrollBar *horizontalScrollBar;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuView;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *PlotWindow)
    {
        if (PlotWindow->objectName().isEmpty())
            PlotWindow->setObjectName(QString::fromUtf8("PlotWindow"));
        PlotWindow->resize(800, 600);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/curve.ico"), QSize(), QIcon::Normal, QIcon::Off);
        PlotWindow->setWindowIcon(icon);
        actionSave = new QAction(PlotWindow);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        actionZoom_in = new QAction(PlotWindow);
        actionZoom_in->setObjectName(QString::fromUtf8("actionZoom_in"));
        actionZoom_out = new QAction(PlotWindow);
        actionZoom_out->setObjectName(QString::fromUtf8("actionZoom_out"));
        centralwidget = new QWidget(PlotWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        widget = new QWidget(centralwidget);
        widget->setObjectName(QString::fromUtf8("widget"));

        gridLayout->addWidget(widget, 0, 1, 1, 1);

        verticalScrollBar = new QScrollBar(centralwidget);
        verticalScrollBar->setObjectName(QString::fromUtf8("verticalScrollBar"));
        verticalScrollBar->setOrientation(Qt::Vertical);

        gridLayout->addWidget(verticalScrollBar, 0, 0, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);

        horizontalScrollBar = new QScrollBar(centralwidget);
        horizontalScrollBar->setObjectName(QString::fromUtf8("horizontalScrollBar"));
        horizontalScrollBar->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(horizontalScrollBar, 1, 0, 1, 1);

        PlotWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(PlotWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 25));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuView = new QMenu(menubar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        PlotWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(PlotWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        PlotWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuView->menuAction());
        menuFile->addAction(actionSave);
        menuView->addAction(actionZoom_in);
        menuView->addAction(actionZoom_out);

        retranslateUi(PlotWindow);

        QMetaObject::connectSlotsByName(PlotWindow);
    } // setupUi

    void retranslateUi(QMainWindow *PlotWindow)
    {
        PlotWindow->setWindowTitle(QApplication::translate("PlotWindow", "Plot", 0, QApplication::UnicodeUTF8));
        actionSave->setText(QApplication::translate("PlotWindow", "Save", 0, QApplication::UnicodeUTF8));
        actionZoom_in->setText(QApplication::translate("PlotWindow", "Zoom in", 0, QApplication::UnicodeUTF8));
        actionZoom_out->setText(QApplication::translate("PlotWindow", "Zoom out", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("PlotWindow", "File", 0, QApplication::UnicodeUTF8));
        menuView->setTitle(QApplication::translate("PlotWindow", "View", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PlotWindow: public Ui_PlotWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLOTWINDOW_H
