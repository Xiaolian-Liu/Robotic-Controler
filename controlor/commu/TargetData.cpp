#include "TargetData.hpp"  

TargetData::TargetData() : SharedMemory("TargetData", sizeof(targetData_t))
{

}

TargetData::~TargetData()
{

}


targetData_t TargetData::getData() const
{
    targetData_t data;
    SharedMemory::read(&data);
    return data;
}

void TargetData::writeData(const targetData_t &data) const
{
    SharedMemory::write(&data);
}