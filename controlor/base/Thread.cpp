#include "Thread.hpp"
#include <limits.h>
#include <string.h>
#include <iostream>
using std::cerr;
using std::endl;



Thread::Thread() 
{
    threadId = 0;
    strcpy(name, "normal");
    priority = -1;
    isRun = false;
    int ret = pthread_attr_init(&attr);
    if(ret)
        cerr << name << ":" << "init pthread attributes failed" << endl;
}

Thread::Thread(char *threadName)
{
    threadId = 0;
    strncpy(name, threadName, 24);
    name[23] = '\0';
    priority = -1;
    isRun = false;
    int ret = pthread_attr_init(&attr);
    if(ret)
        cerr << name << ":" << "init pthread attributes failed" << endl;
    // m_iThreadStatus = 0;
    // m_vpParameter = NULL;
}

Thread::Thread(char *threadName, int priority) 
{
    threadId = 0;
    strncpy(name, threadName, 24);
    name[23] = '\0';
    
    int maxPriority = sched_get_priority_max(SCHED_FIFO);
    int minPriority = sched_get_priority_min(SCHED_FIFO);
    if(priority > maxPriority)
        this->priority = maxPriority;
    if(priority > minPriority)
        this->priority = minPriority;

    isRun = false;

    int ret = pthread_attr_init(&attr);
    if(ret)
        cerr << name << ":" << "init pthread attributes failed" << endl;

    ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    if(ret)
        cerr << name << ":" << "pthread setstacksize failed" << endl;

    ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    if(ret)
        cerr << name << ":" << "pthread setschedpolicy failed" << endl;

    param.__sched_priority = this->priority;
    ret = pthread_attr_setschedparam(&attr, &param);
    if(ret)
        cerr << name << ":" << "pthread setschedparam failed\n";
    
    ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if(ret)
        cerr << name << ":" << "pthread setinheritsched failed\n";
    
}

Thread::~Thread()
{
    if(pthread_attr_destroy(&attr))
        cerr << name << ":" << "pthread attribute destroy failed\n";
}

bool Thread::start() 
{
    int ret = pthread_create(&(this->threadId), &attr, Thread::threadFunc, this);
    if(0 == ret)
    {
        isRun = true;
        return true;
    }
    return false;
}

void* Thread::threadFunc(void * arg) 
{
    Thread *t = (Thread *)arg;
    t->run();
    return nullptr;
}

void Thread::exit() 
{
    pthread_exit(nullptr);
}

pthread_t Thread::getThreadId() const
{
    return threadId;
}

bool Thread::isRunning() const
{
    return isRun;
}

bool Thread::isFinished() const
{
    return !isRun;
}

void Thread::quit() 
{
    isRun = false;
}

void Thread::terminate() 
{
    pthread_cancel(threadId);
}

void Thread::wait() 
{
    int ret = pthread_join(threadId, nullptr);
    if(ret)
        cerr << name << ":" << "can't join with thread" << endl;
}

void Thread::setPriority(int priority) 
{
    int maxPriority = sched_get_priority_max(SCHED_FIFO);
    int minPriority = sched_get_priority_min(SCHED_FIFO);
    if(priority > maxPriority)
        this->priority = maxPriority;
    if(priority > minPriority)
        this->priority = minPriority;
        
    param.__sched_priority = this->priority;
    int ret = pthread_attr_setschedparam(&attr, &param);
    if(ret)
        cerr << name << ":" << "pthread setschedparam failed\n";
}

int Thread::getPriority() const
{
    return priority;
}

void Thread::setDetached(bool detach) 
{
    if(detach){
        int ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        if(ret){
            cerr << name << ":" << "pthread setdetachedstate failed" << endl;
        }

    }else
    {
        int ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        if(ret){
            cerr << name << ":" << "pthread setdetachedstate failed" << endl;
        }
    }
}
