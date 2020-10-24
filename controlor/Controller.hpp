#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "base/Thread.hpp"
#include "ecat/EthercatMaster.hpp"
#include "base/Time.hpp"
#include "commu/ReceiveData.hpp"
#include "commu/TargetData.hpp"
#include "commu/PositionQueue.hpp"
#include "commu/StateData.hpp"

class Controller : public Thread
{
	private:
		int frequency;
		int cycleTime;
		EthercatMaster master;
		ReceiveData receiveData;
        TargetData targetData;
        PositionQueue posQueue;
		StateData statData;
		virtual void run();

	public:
		Controller(int freq);
		~Controller();
};
#endif // __CONTROLLER_H__
