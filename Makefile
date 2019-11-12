CC            = gcc
CXX           = g++
DEFINES       =
CFLAGS        = -g -Wall $(DEFINES)
CXXFLAGS      = -g -Wall -std=c++11 $(DEFINES)
INCPATH       = -I/usr/xenomai/include -I. -IEcatDrive -Ikinematics -I.
LINK          = g++
LFLAGS        =
LIBS          = -L/usr/xenomai/lib -lnative -lrtdm -lxenomai -L/opt/etherlab/lib -lethercat_rtdm
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/local/Trolltech/Qt-4.8.6/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./Objects/

####### Files

SOURCES       = robot.cpp \
		EcatDrive/drive.cpp \
		EcatDrive/EcatDrive.cpp \
		kinematics/er20.cpp \
		kinematics/exampleview.cpp \
		kinematics/kinematics.cpp \
		kinematics/motion.cpp \
		kinematics/trajectory.cpp 
OBJECTS = 	Objects/robot.o \
			Objects/drive.o \
			Objects/EcatDrive.o \
			Objects/er20.o \
			Objects/kinematics.o \
			Objects/motion.o \
			Objects/trajectory.o

TARGET        = robot.run

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)


.PHONY:clean
clean:
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) $(TARGET) 


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 

check: first

####### Compile

Objects/robot.o: robot.cpp EcatDrive/EcatDrive.h \
		EcatDrive/drive.h \
		kinematics/motion.h \
		kinematics/trajectory.h \
		kinematics/er20.h \
		commu.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

Objects/drive.o: EcatDrive/drive.cpp EcatDrive/drive.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

Objects/EcatDrive.o: EcatDrive/EcatDrive.cpp EcatDrive/EcatDrive.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<
Objects/er20.o: kinematics/er20.cpp kinematics/er20.h \
		kinematics/kinematics.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

Objects/kinematics.o: kinematics/kinematics.cpp kinematics/kinematics.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<
Objects/motion.o: kinematics/motion.cpp kinematics/motion.h \
		kinematics/trajectory.h \
		kinematics/er20.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

Objects/trajectory.o: kinematics/trajectory.cpp kinematics/trajectory.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

####### Install

install: 

uninstall: 

save:
	git commit -a

push:
	git push origin

updateui:
	uic -o ./teachview/ui_mainwindow.h ./teachview/mainwindow.ui


##all: robot.run
#
## Which compiler
#CC = gcc
#CXX = g++
#
## wthat is the objects
#OBJ = robot.o EcatDrive.o motion.o
#
## The source file
#ROBOT_SRC = robot.cpp ecrt.h
#DRIVES_SRC = drives2.c ecrt.h
## Where to install
##INSTDIR = /usr/sbin
#
## Where are include files kept
#INCLUDE = /usr/xenomai/include ./
#
## Options for C development
#CFLAGS = -g -Wall
#
## Options for C++ development
#CXXFLAGS = -g -Wall -std=c++11
#
## Where to link liberary
#ECAT_LIBPATH =/opt/etherlab/lib 
#XENO_LIBPATH =/usr/xenomai/lib
#
## Which to link
#ECAT_LIBVAR = -lethercat_rtdm 
#XENO_LIBVAR = -lnative -lrtdm -lxenomai
## Options for release
## CFLAGS = -O -Wall -ansi
#
#robot.run: $(OBJ)
#	$(CXX) -o $@ $(OBJ) -L$(XENO_LIBPATH) $(XENO_LIBVAR) -L$(ECAT_LIBPATH) $(ECAT_LIBVAR)
#
#robot.o: robot.cpp EcatDrive.h ecrt.h
#	$(CXX) -I$(INCLUDE) $(CXXFLAGS) -c $<
#
#EcatDrive.o: EcatDrive.cpp ecrt.h EcatDrive.h
#	$(CXX) -I$(INCLUDE) $(CXXFLAGS) -c $<
#motion.o: motion.cpp motion.h EcatDrive.h ecrt.h
#	$(CXX) -I$(INCLUDE) $(CXXFLAGS) -c $<
#
##ecat_dc.o: ecat_dc.c ecat_dc.h ecrt.h
##	$(CC) -I$(INCLUDE) $(CFLAGS) -c $<
##2.o: 2.c a.h b.h
##	$(CC) -I$(INCLUDE) $(CFLAGS) -c 2.c
##3.o: 3.c b.h c.h
##	$(CC) -I$(INCLUDE) $(CFLAGS) -c 3.c
#
#.PHONY:clean
#clean:
#	-rm -f *.o
#	-rm -f *.run

#install: myapp
#	@if [ -d $(INSTDIR) ]; \
		then \
		cp myapp $(INSTDIR);\
		chmod a+x $(INSTDIR)/myapp;\
		chmod og-w $(INSTDIR)/myapp;\
		echo "Installed in $(INSTDIR)";\
	else \
		echo "Sorry, $(INSTDIR) does not exist";\
	fi


#.PHONY: all clean
#CC = gcc
#RM = rm
#MKDIR = mkdir
#CFLAGS = -Wall -std=gnu99
#
#
#INC = -I ../include/hh_include
#
#
#DIR_OBJS = ../obj
#DIR_EXEC = ../exec
#
#
#DIRS := $(DIR_OBJS) $(DIR_EXEC)
#EXEC = prin
#SRCS = $(wildcard *.c)
#OBJS = $(SRCS:.c=.o)
#
#
#OBJS := $(addprefix $(DIR_OBJS)/,$(OBJS))
#EXEC := $(addprefix $(DIR_EXEC)/,$(EXEC))
#
## $@ ---target files
## $^ -----dependecy files
#all: $(DIRS) $(EXEC)
#$(DIRS):
#$(MKDIR) $@
#$(EXEC):$(OBJS)
#$(CC) -o $@ $^
#$(DIR_OBJS)/%.o:%.c
#$(CC) $(CFLAGS) $(INC) -o $@ -c $^
#
#
#clean:
#
#$(RM) -rf  $(DIRS) $(EXEC)
#