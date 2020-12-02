#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "base/Thread.hpp"
#include "ecat/EthercatMaster.hpp"
#include "base/Time.hpp"
#include "commu/ReceiveData.hpp"
#include "commu/TargetData.hpp"
#include "commu/PositionQueue.hpp"


class Controller : public Thread
{
	private:
		int frequency;
		int cycleTime;
		EthercatMaster master;
		PositionQueue posQueue;
		virtual void run();

	public:
		Controller(int freq);
        EthercatMaster * getMaster();
		~Controller();
};
#endif // __CONTROLLER_H__
