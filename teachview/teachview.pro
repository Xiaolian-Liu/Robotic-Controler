#-------------------------------------------------
#
# Project created by QtCreator 2019-11-13T11:33:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = teachview
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    plot/plotwindow.cpp \
    plot/qcustomplot.cpp \
    Threads/ReadPipeThread.cpp \
    kinematics/er20.cpp \
    kinematics/kinematics.cpp \
    kinematics/trajectory.cpp

HEADERS  += mainwindow.h \
    plot/plotwindow.h \
    plot/qcustomplot.h \
    Threads/ReadPipeThread.h \
    ui_mainwindow.h \
    ui_plotwindow.h \
    kinematics/er20.h \
    kinematics/kinematics.h \
    kinematics/trajectory.h \

FORMS    += mainwindow.ui \
    plot/plotwindow.ui

RESOURCES += \
    icons.qrc

DISTFILES +=

INCLUDEPATH += . QcustomPlot Threads Eigen

QMAKE_CXXFLAGS   +=    -std=c++11 -Wall
