#include "PositionQueue.hpp"  

PositionQueue::PositionQueue() : 
MessageQueue("PositionQueue", O_CREAT|O_RDWR, 0666, sizeof(incPos_t),2048)

{

}

PositionQueue::PositionQueue(int flag) :
MessageQueue("PositionQueue", flag, 0666, sizeof(incPos_t),2048)

{

}

PositionQueue::~PositionQueue()
{

}

int PositionQueue::getPosition(incPos_t * pos)
{
    ssize_t res = mq_receive(mqd, (char *)pos, sizeof(incPos_t), NULL);
    int b = sizeof(incPos_t);
    mq_attr attra;
    mq_getattr(mqd, &attra);
    int c = attra.mq_msgsize;
    int a = errno;
    return res;
}

int PositionQueue::getPosition(incPos_t * pos, Time timeOut)
{
    return mq_timedreceive(mqd, (char *)pos, sizeof(incPos_t), NULL, &timeOut);
}

int PositionQueue::sendPosition(incPos_t pos) 
{
    return mq_send(mqd, (char *)&pos, sizeof(incPos_t), NULL);
}

int PositionQueue::sendPosition(incPos_t pos, Time timeOut) 
{
    return mq_timedsend(mqd, (char *)&pos, sizeof(incPos_t), 0, &timeOut);
}

int PositionQueue::size() 
{
    mq_attr attra;
    mq_getattr(mqd, &attra);
    return attra.mq_curmsgs;
}
