#ifndef __THREADTEST_H__
#define __THREADTEST_H__

#include "base/Thread.hpp"

class ThreadTest : public Thread
{
	private:
		virtual void run();

	public:

		ThreadTest();
		~ThreadTest();

};
#endif // __THREADTEST_H__
