#include "JointVec.h"

namespace RKD {



JointVec::JointVec()
{

}

JointVec::JointVec(double j1, double j2, double j3, double j4, double j5, double j6)
{
	*this << j1, j2, j3, j4, j5, j6;
}

JointVec::JointVec(const Matrix<double, 6, 1>& m) : Matrix<double, 6, 1>(m)
{

}

JointVec & JointVec::operator=(const Matrix<double, 6, 1>& m)
{
	if (this != &m)
	{
		*this << m[0], m[1], m[2], m[3], m[4], m[5];
	}
	// TODO: 在此处插入 return 语句
	return *this;
}

JointVec JointVec::rad()
{
	return JointVec(*this * PI / 180);
}

JointVec::~JointVec()
{

}

std::ostream & operator<<(std::ostream & os, const JointVec j)
{
	os << j[0] << " " << j[1] << " " << j[2] << " " << j[3] << " " << j[4] << " " << j[5];
	// TODO: insert return statement here
	return os;
}



}
