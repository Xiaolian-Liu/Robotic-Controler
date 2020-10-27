TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    circle.cpp \
    er20.cpp \
    Rotation.cpp \
    trajectory.cpp \
    transform.cpp \
    motion.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    circle.h \
    er20.h \
    motion.h \
    Rotation.h \
    trajectory.h \
    transform.h \
    kinematics.h


INCLUDEPATH += ../include


DESTDIR += /home/vito/Robotic-Controler/bin

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Wall
