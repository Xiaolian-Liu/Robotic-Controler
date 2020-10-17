#pragma once
#include <Eigen/Dense>

using namespace Eigen;

namespace RKD {


class CartPos
{
private:
	Vector3d xyz;
	Vector3d rpy;
public:

	CartPos();
	CartPos(const Vector3d &xyz, const Vector3d &rpy);
	~CartPos();
};


}
