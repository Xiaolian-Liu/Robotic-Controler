#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "Thread.hpp"
#include "EthercatMaster.hpp"
#include "Time.hpp"

class Controller : public Thread
{
	private:
		int frequency;
		int cycleTime;
		EthercatMaster master;
		virtual void run();

	public:
		Controller(int freq);
		~Controller();
};
#endif // __CONTROLLER_H__