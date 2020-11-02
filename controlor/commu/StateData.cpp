#include "StateData.hpp"

int beEnable = 0;
stateData_t StateData::data = {0};
pthread_mutex_t StateData::mutex = PTHREAD_MUTEX_INITIALIZER;

StateData::StateData()
{

}


stateData_t StateData::getData()
{
    stateData_t res;
    pthread_mutex_lock(&mutex);
    res = data;
    pthread_mutex_unlock(&mutex);
    return res;
}

void StateData::writeData(const stateData_t &wdata)
{
    pthread_mutex_lock(&mutex);
    data = wdata;
    pthread_mutex_unlock(&mutex);
}

StateData::~StateData()
{

}
