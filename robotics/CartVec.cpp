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

CartVec::CartVec(const CartVec & p)
{
	if (this != &p)
	{
		this->pe = p.pe;
		this->rpy = p.rpy;
	}
	// TODO: Add your implementation code here.
}
CartVec::CartVec(double x, double y, double z, double rx, double ry, double rz)
{
	this->pe << x, y, z;
	this->rpy << rx, ry, rz;
}

CartVec::CartVec(Matrix4d T)
{
	this->pe = T.block(0, 3, 2, 3);
	this->rpy = tr2rpy(T);
}

void CartVec::operator=(const CartVec & p)
{
	this->pe = p.pe;
	this->rpy = p.rpy;
}

Matrix4d CartVec::Transl() const
{
	return transl(pe, rpy2ro(rpy));
}

Vector3d CartVec::PE() const
{
	return pe;
}

Vector3d CartVec::RPY() const
{
	return rpy;
}


}



