#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "base/Thread.hpp"
#include "ecat/EthercatMaster.hpp"
#include "base/Time.hpp"
#include "commu/ReceiveData.hpp"
#include "commu/TargetData.hpp"


class Controller : public Thread
{
	private:
		int frequency;
		int cycleTime;
		EthercatMaster master;
		ReceiveData receiveData;
        TargetData targetData;
		virtual void run();

	public:
		Controller(int freq);
		~Controller();
};
#endif // __CONTROLLER_H__
