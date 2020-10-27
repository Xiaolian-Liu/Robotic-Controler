#-------------------------------------------------
#
# Project created by QtCreator 2020-10-14T09:52:50
#
#-------------------------------------------------

QT       -= core gui

TARGET = robotics
TEMPLATE = lib
CONFIG += staticlib


DEFINES += ROBOTICS_LIBRARY

SOURCES += Robotics.cpp

HEADERS += Robotics.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
