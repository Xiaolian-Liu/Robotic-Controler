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
    Threads/ReadPipeThread.cpp

HEADERS  += mainwindow.h \
    plot/plotwindow.h \
    ../build-teachview-Qt_4_8_6_Qt_4_8_6-Debug/ui_mainwindow.h \
    ../build-teachview-Qt_4_8_6_Qt_4_8_6-Debug/ui_plotwindow.h \
    plot/qcustomplot.h \
    Threads/ReadPipeThread.h

FORMS    += mainwindow.ui \
    plot/plotwindow.ui

RESOURCES += \
    icons.qrc

DISTFILES +=

INCLUDEPATH += . QcustomPlot Threads
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Wall

INCLUDEPATH += /home/vito/Robotic-Controler/include

UI_DIR = /home/vito/Robotic-Controler/teachview
DESTDIR = /home/vito/Robotic-Controler/bin

LIBS += -L/home/vito/Robotic-Controler/lib -lkinematics
