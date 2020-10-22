#include "ReceiveData.hpp"  
#include "base/SharedMemory.hpp"
#include <unistd.h>
#include <iostream>
#include <string.h>

using std::cerr;

ReceiveData::ReceiveData() : SharedMemory("ReceiveData", sizeof(receiveData_t))
{

}

ReceiveData::~ReceiveData()
{

}

receiveData_t ReceiveData::getData() const
{
    receiveData_t data;
    SharedMemory::read(&data);
    return data;
}

void ReceiveData::writeData(const receiveData_t &data) const
{
    SharedMemory::write(&data);
}
