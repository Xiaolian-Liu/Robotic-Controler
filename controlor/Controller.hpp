#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "base/Thread.hpp"
#include "ecat/EthercatMaster.hpp"
#include "base/Time.hpp"
#include "commu/ReceiveData.hpp"
#include "commu/TargetData.hpp"
#include "commu/PositionQueue.hpp"
#include "kinematics/er20.h"

#include "queue"
using std::queue;

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
			Auto = 2,
			FromText = 3
		};

		void printMoveMode(MoveMode mode)
		{
			if(ManualJoint == mode){
				printf("ManualJoint\n");
			}
			else if(ManualCart == mode){
				printf("ManualCart\n");
			}
			else if(Auto == mode){
				printf("Auto\n");
			}
		}
		
		bool enableFlag;
		int frequency;
		int cycleTime;
		EthercatMaster master;
		PositionQueue posQueue;
		State state;
		State lastState;
		MoveMode moveMode;
		virtual void run();
		void error();
		void inactive();
		void active();
		void jogJoint();
		void autoMoving();

		int pipe_fd;

		bool simulate;

	public:
		const double VThreshold = 50;
		double Vmax;
		double acc;
		double dec;
		double deltaT;
		JointVec q;
		JointVec qNext;
		JointVec v;
		JointVec vNext;
		JointVec vLast;
		JointVec torque;
		JointVec torNext;
		
		JointVec q0;
		JointVec v0;
		uint64_t n;
		uint64_t n1;

		queue<JointVec> qBuff;
		queue<JointVec> qdBuff;
		queue<JointVec> TorqueBuff;

		double velPercent;
		double accPercent;

		int init();
		Controller(int freq);
		EthercatMaster * getMaster();
		~Controller();
};
#endif // __CONTROLLER_H__
