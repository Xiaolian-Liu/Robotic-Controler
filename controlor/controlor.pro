TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    base/MessageQueue.cpp \
    base/SharedMemory.cpp \
    base/Thread.cpp \
    base/Time.cpp \
    commu/PositionQueue.cpp \
    commu/ReceiveData.cpp \
    commu/TargetData.cpp \
    ecat/EthercatMaster.cpp \
    ecat/Slave.cpp \
    kinematics/circle.cpp \
    kinematics/er20.cpp \
    kinematics/Rotation.cpp \
    kinematics/trajectory.cpp \
    kinematics/transform.cpp \
    Controller.cpp \
    main.cpp \
    motion.cpp \
    ThreadTest.cpp \
    commu/StateData.cpp \
    drive.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    base/MessageQueue.hpp \
    base/SharedMemory.hpp \
    base/Thread.hpp \
    base/Time.hpp \
    commu/PositionQueue.hpp \
    commu/ReceiveData.hpp \
    commu/TargetData.hpp \
    ecat/EthercatMaster.hpp \
    ecat/Slave.hpp \
    kinematics/circle.h \
    kinematics/er20.h \
    kinematics/Rotation.h \
    kinematics/trajectory.h \
    kinematics/transform.h \
    Controller.hpp \
    motion.h \
    ThreadTest.hpp \
    commu/StateData.hpp \
    drive.h


INCLUDEPATH += /home/vito/Robotic-Controler/include

DESTDIR += /home/vito/Robotic-Controler/bin

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Wall

LIBS += -lrt
LIBS += -lpthread
LIBS += -lethercat
