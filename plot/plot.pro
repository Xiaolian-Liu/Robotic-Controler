#-------------------------------------------------
#
# Project created by QtCreator 2020-10-29T15:10:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = plot
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h

FORMS    += mainwindow.ui

DESTDIR += /home/vito/Robotic-Controler/bin
