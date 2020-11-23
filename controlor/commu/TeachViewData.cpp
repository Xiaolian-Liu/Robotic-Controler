#include "TeachViewData.hpp"  

TeachViewData::TeachViewData()
{

}

teachViewData_t TeachViewData::getData() 
{
    teachViewData_t res;
    pthread_mutex_lock(&mutex);
    res = data;
    pthread_mutex_unlock(&mutex);
    return res;
}

void TeachViewData::writeData(const teachViewData_t &wdata) 
{
    pthread_mutex_lock(&mutex);
    data = wdata;
    pthread_mutex_unlock(&mutex);
}

TeachViewData::~TeachViewData()
{

}