#ifndef __THREAD_H__
#define __THREAD_H__


#include <pthread.h>

class Thread
{
private:
	pthread_t threadId;
	char name[24];
	pthread_attr_t attr;
	struct sched_param param;
	int priority;
	static void *threadFunc(void *arg);
	void exit();

protected:
	// int m_iThreadStatus;
	// void * m_vpParameter;
	virtual void run() = 0;
	bool isRun;

public:
	Thread();
	Thread(char *threadName);
	Thread(char *threadName, int priority, bool detach);
	// Thread(void *arg);
	virtual ~Thread();
	bool start();
	// static void *ThreadCall(void *vpParameter);
	pthread_t getThreadId() const;
	bool isRunning() const;
	bool isFinished() const;
	void quit();
	void terminate();
	void wait();
	int getPriority();
	// int getThreadStatus();
};
#endif // __THREAD_H__