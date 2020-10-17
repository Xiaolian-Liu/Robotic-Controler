#include "CartPos.h"

namespace RKD {


CartPos::CartPos()
{
}

CartPos::CartPos(const Vector3d &xyz, const Vector3d &rpy)
{
	this->xyz = xyz;
	this->rpy = rpy;
}

CartPos::~CartPos()
{
}



}
