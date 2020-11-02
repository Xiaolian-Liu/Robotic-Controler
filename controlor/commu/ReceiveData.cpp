#include "ReceiveData.hpp"  
#include <unistd.h>
#include <iostream>
#include <string.h>

using std::cerr;

receiveData_t ReceiveData::data = {0};
pthread_mutex_t ReceiveData::mutex = PTHREAD_MUTEX_INITIALIZER;

ReceiveData::ReceiveData()
{
    
}

ReceiveData::~ReceiveData()
{

}

receiveData_t ReceiveData::getData()
{
    receiveData_t res;
    pthread_mutex_lock(&mutex);
    res = data;
    pthread_mutex_unlock(&mutex);
    return res;
}

void ReceiveData::writeData(const receiveData_t & wdata)
{
    pthread_mutex_lock(&mutex);
    data = wdata;
    pthread_mutex_unlock(&mutex);
}
