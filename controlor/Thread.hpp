#ifndef __THREAD_H__
#define __THREAD_H__


#include <pthread.h>

class Thread
{
private:
protected:
	pthread_t m_ThreadId;
	int m_iThreadStatus;
	void *m_vpParameter;

public:
	Thread();
	Thread(void *vpParameter);
	virtual ~Thread();
	int Start(void *Parameter);
	static void *ThreadCall(void *vpParameter);
	pthread_t GetThreadId();
	int GetThreadStatus();
	virtual void *Run(void *Parameter) = 0;

};
#endif // __THREAD_H__