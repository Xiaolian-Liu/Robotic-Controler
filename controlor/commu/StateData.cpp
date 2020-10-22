#include "StateData.hpp"  

StateData::StateData() : SharedMemory("StateData", sizeof(stateData_t))
{

}

stateData_t StateData::getData() const
{
    stateData_t res;
    SharedMemory::read(&res);
    return res;
}

void StateData::writeData(const stateData_t &data) const
{
    SharedMemory::write(&data);
}

StateData::~StateData()
{

}