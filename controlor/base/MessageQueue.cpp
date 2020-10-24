#include "MessageQueue.hpp"  
#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <string.h>

using std::cerr;

MessageQueue::MessageQueue()
{
    this->queueName = "/MessageQueue";
    this->mode = 0666;
    this->oflag = O_CREAT | O_RDWR;
    this->attr.mq_maxmsg = 1024;
    this->attr.mq_msgsize = 24;
}

MessageQueue::MessageQueue(const string &name)
{
    this->queueName = "/MessageQueue" + name;
    this->mode = 0666;
    this->oflag = O_CREAT | O_RDWR;
    this->attr.mq_maxmsg = 1024;
    this->attr.mq_msgsize = 24;
}

MessageQueue::MessageQueue(const string &name, int oflag, mode_t mode, long msgsize, long maxmsg)
{
    this->queueName = "/MessageQueue"+name;
    this->oflag = oflag;
    this->mode = mode;
    this->attr.mq_flags = 0;
    this->attr.mq_maxmsg = maxmsg;
    this->attr.mq_msgsize = msgsize;
}

int MessageQueue::init() 
{
    mqd = mq_open(this->queueName.c_str(), oflag, mode, &attr);
    if((mode_t)-1 == mqd)
    {
        perror("MessageQueue open failed: ");
        return -1;
    }
    return 0;
}

void MessageQueue::setAttr(mq_attr attr) 
{
    this->attr = attr;
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
