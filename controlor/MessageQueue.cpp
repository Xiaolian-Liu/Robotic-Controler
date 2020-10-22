#include "MessageQueue.hpp"  
#include <iostream>
using std::cerr;

MessageQueue::MessageQueue()
{
    this->queueName = "MessageQueue";
    this->attrDefault = true;
    this->mode = 0666;
    this->oflag = O_CREAT | O_RDWR;
}

MessageQueue::MessageQueue(const string &name, int oflag, mode_t mode, mq_attr attr) 
{
    this->queueName = "MessageQueue"+name;
    this->oflag = oflag;
    this->mode = mode;
    this->attr = attr;
    this->attrDefault = false;
}

int MessageQueue::init() 
{
    if(attrDefault){
        mqd = mq_open(queueName.c_str(), oflag, mode, NULL);
    }
    else{
        mqd = mq_open(queueName.c_str(), oflag, mode, &attr);
    }
    if((mode_t)-1 == mqd)
    {
        cerr << "MessageQueue open failed\n";
        return -1;
    }
    return 0;
}

void MessageQueue::setAttr(mq_attr attr) 
{
    this->attr = attr;
    this->attrDefault = false;
}

mq_attr MessageQueue::getAttr() 
{
    mq_attr attrRes;
    mq_getattr(mqd, &attrRes);
    return attrRes;
}

MessageQueue::~MessageQueue()
{
    mq_close(mqd);
    mq_unlink(this->queueName.c_str());
}