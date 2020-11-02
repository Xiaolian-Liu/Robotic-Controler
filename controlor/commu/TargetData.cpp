#include "TargetData.hpp"  

targetData_t TargetData::data = {0};
pthread_mutex_t TargetData::mutex = PTHREAD_MUTEX_INITIALIZER;

TargetData::TargetData()
{

}

TargetData::~TargetData()
{

}


targetData_t TargetData::getData()
{
    targetData_t res;
    pthread_mutex_lock(&mutex);
    res = data;
    pthread_mutex_unlock(&mutex);
    return res;
}

void TargetData::writeData(const targetData_t &wdata)
{
    pthread_mutex_lock(&mutex);
    data = wdata;
    pthread_mutex_unlock(&mutex);
}
