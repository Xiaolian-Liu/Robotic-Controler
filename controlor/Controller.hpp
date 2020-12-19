#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "base/Thread.hpp"
#include "ecat/EthercatMaster.hpp"
#include "base/Time.hpp"
#include "commu/ReceiveData.hpp"
#include "commu/TargetData.hpp"
#include "commu/PositionQueue.hpp"
#include "kinematics/er20.h"

struct fifoData
{
	int32_t tarpos[6];
	double joiangles[6];
};



class Controller : public Thread
{
	private:
		enum State
		{
			Inactive,
			Active,
			Moving,
			Error
		};
		enum MoveMode
		{
			ManualJoint = 0,
			ManualCart = 1,
			Auto = 2
		};
		bool enableFlag;
		int frequency;
		int cycleTime;
		EthercatMaster master;
		PositionQueue posQueue;
		State state;
		MoveMode moveMode;
		virtual void run();
		void error();
		void inactive();
		void active();
		void jogJoint();
		void autoMoving();

		int pipe_fd;

	public:
		double Vmax;
		double acc;
		double dec;
		double deltaT;
		JointVec q;
		JointVec qNext;
		JointVec v;
		JointVec vNext;
		JointVec vLast;
		JointVec q0;
		JointVec v0;
		uint64_t n;
		uint64_t n1;

		int init();
		Controller(int freq);
		EthercatMaster * getMaster();
		~Controller();
};
#endif // __CONTROLLER_H__
