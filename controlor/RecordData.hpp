#ifndef __RECORDDATA_H__
#define __RECORDDATA_H__
#include "Controller.hpp"
#include "base/Thread.hpp"

class RecordData : public Thread
{
	private:
		virtual void run();
		Controller *control;

	public:
		RecordData(Controller * contro);
		~RecordData();

};
#endif // __RECORDDATA_H__