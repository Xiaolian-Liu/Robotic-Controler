#include "CartVec.h"

namespace RKD {


CartVec::CartVec()
{
}

CartVec::CartVec(const Vector3d &pe, const Vector3d &rpy)
{
	this->pe = pe;
	this->rpy = rpy;
}

CartVec::~CartVec()
{
}



std::ostream & operator<<(std::ostream & os, const CartVec & p)
{
	os << p.pe[0] << " " << p.pe[1] << " " << p.pe[2] << " " << p.rpy[0] << " " << p.rpy[1] << " " << p.rpy[2];
	return os;
	// TODO: insert return statement here
}

}


RKD::CartVec::CartVec(const CartVec & p)
{
	if (this != &p)
	{
		this->pe = p.pe;
		this->rpy = p.rpy;
	}
	// TODO: Add your implementation code here.
}

RKD::CartVec::CartVec(double x, double y, double z, double rx, double ry, double rz)
{
	this->pe << x, y, z;
	this->rpy << rx, ry, rz;
}

void RKD::CartVec::operator=(const CartVec & p)
{
	this->pe = p.pe;
	this->rpy = p.rpy;
}
