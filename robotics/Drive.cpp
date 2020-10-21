#include "Drive.h"


namespace RKD {


Drive::Drive()
{
	this->nBit = 17;
	this->offset = 0;
}

Drive::Drive(unsigned int nBit, int32_t offset)
{
	this->nBit = nBit;
	this->offset = offset;
}

double Drive::degree2rad(double angle)
{

	return (angle * PI / 180);
}

double Drive::rad2degree(double theta)
{
	return (theta * 180 / PI);
}

double Drive::increment2jointangle(int32_t inc)
{
	return (inc - offset) * 360 / (1 << nBit);
}

int32_t Drive::jointangle2increment(double joi)
{
	return int32_t(joi * (1 << nBit) / 360 + offset);
}

void Drive::setOffset(int32_t offSet)
{
	this->offset = offSet;
}


Drive::~Drive()
{

}


}
