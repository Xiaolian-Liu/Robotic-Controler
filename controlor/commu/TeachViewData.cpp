#include "TeachViewData.hpp"

teachViewData_t TeachViewData::data = {0};
pthread_mutex_t TeachViewData::mutex = PTHREAD_MUTEX_INITIALIZER;

TeachViewData::TeachViewData()
{

}

TeachViewData::~TeachViewData()
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
