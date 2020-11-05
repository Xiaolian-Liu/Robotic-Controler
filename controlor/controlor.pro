TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    base/MessageQueue.cpp \
    base/SharedMemory.cpp \
    base/Thread.cpp \
    base/ThreadTest.cpp \
    base/Time.cpp \
    commu/PositionQueue.cpp \
    commu/ReceiveData.cpp \
    commu/StateData.cpp \
    commu/TargetData.cpp \
    ecat/EthercatMaster.cpp \
    ecat/Slave.cpp \
    kinematics/circle.cpp \
    kinematics/er20.cpp \
    kinematics/Rotation.cpp \
    kinematics/trajectory.cpp \
    kinematics/transform.cpp \
    Controller.cpp \
    drive.cpp \
    main.cpp \
    motion.cpp \
    server.cpp


include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    base/MessageQueue.hpp \
    base/SharedMemory.hpp \
    base/Thread.hpp \
    base/ThreadTest.hpp \
    base/Time.hpp \
    commu/PositionQueue.hpp \
    commu/ReceiveData.hpp \
    commu/StateData.hpp \
    commu/TargetData.hpp \
    ecat/EthercatMaster.hpp \
    ecat/Slave.hpp \
    kinematics/circle.h \
    kinematics/er20.h \
    kinematics/Rotation.h \
    kinematics/trajectory.h \
    kinematics/transform.h \
    Controller.hpp \
    drive.h \
    motion.h \
    server.hpp

INCLUDEPATH += /home/vito/Robotic-Controler/include

DESTDIR += /home/vito/Robotic-Controler/bin

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Wall

LIBS += -lrt
LIBS += -lpthread
LIBS += -lethercat
