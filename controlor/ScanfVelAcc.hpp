#ifndef SCANFVELACC_H
#define SCANFVELACC_H
#pragma once

#include "base/Thread.hpp"
#include "Controller.hpp"
	
class ScanfVelAcc : public Thread
{
	private:
		virtual void run();
		Controller *control;

	public:

		ScanfVelAcc(Controller * control);

		~ScanfVelAcc();

};
#endif