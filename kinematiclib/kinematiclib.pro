#-------------------------------------------------
#
# Project created by QtCreator 2020-01-09T15:43:12
#
#-------------------------------------------------

QT       -= core gui

TARGET = kinematics
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    ../kinematics/circle.cpp \
    ../kinematics/er20.cpp \
    ../kinematics/main.cpp \
    ../kinematics/Rotation.cpp \
    ../kinematics/trajectory.cpp \
    ../kinematics/transform.cpp

HEADERS += \
    ../kinematics/circle.h \
    ../kinematics/er20.h \
    ../kinematics/kinematics.h \
    ../kinematics/Rotation.h \
    ../kinematics/trajectory.h \
    ../kinematics/transform.h

DESTDIR += /home/vito/Robotic-Controler/lib

unix {
    target.path = /home/vito/Robotic-Controler/lib
    INSTALLS += target
}
