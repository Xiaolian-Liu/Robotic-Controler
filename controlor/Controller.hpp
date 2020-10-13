#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "Thread.hpp"
#include "EthercatMaster.hpp"
#include "Time.hpp"
#include "ReceiveData.hpp"
#include "TargetData.hpp"


class Controller : public Thread
{
	private:
		int frequency;
		int cycleTime;
		EthercatMaster master;
		ReceiveData receiveData;
		virtual void run();

	public:
		Controller(int freq);
		~Controller();
};
#endif // __CONTROLLER_H__