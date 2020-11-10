#include "JointVec.h"



RKD::JointVec::JointVec()
{
}

RKD::JointVec::JointVec(double j1, double j2, double j3, double j4, double j5, double j6)
{
	*this << j1, j2, j3, j4, j5, j6;
}

RKD::JointVec::~JointVec()
{
}

std::ostream & RKD::operator<<(std::ostream & os, const JointVec j)
{
	os << j[0] << " " << j[1] << " " << j[2] << " " << j[3] << " " << j[4] << " " << j[5];
	// TODO: insert return statement here
	return os;
}
