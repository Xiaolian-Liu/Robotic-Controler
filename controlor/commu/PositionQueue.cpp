#include "PositionQueue.hpp"  

PositionQueue::PositionQueue() : 
MessageQueue("PositionQueue", O_CREAT|O_RDWR,
 0666, {0,1024,1024,0})

{

}

PositionQueue::~PositionQueue()
{

}

incPos_t PositionQueue::getPosition() 
{
    incPos_t res;
    mq_receive(mqd, (char *)&res, sizeof(incPos_t), 0);
    return res;
}

int PositionQueue::sendPosition(incPos_t pos) 
{
    return mq_send(mqd, (char *)&pos, sizeof(incPos_t), 0);
}

int PositionQueue::sendPosition(incPos_t pos, Time timeOut) 
{
    return mq_timedsend(mqd, (char *)&pos, sizeof(incPos_t), 0, &timeOut);
}
