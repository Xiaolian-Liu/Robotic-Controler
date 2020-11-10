#pragma once
#include <Eigen/Dense>

using namespace Eigen;

namespace RKD {


class CartVec
{
private:
	Vector3d pe;
	Vector3d rpy;
public:

	CartVec();
	CartVec(const Vector3d &pe, const Vector3d &rpy);
	~CartVec();
	CartVec(const CartVec & p);
	CartVec(double x, double y, double z, double rx, double ry, double rz);
	void operator=(const CartVec & p);
	friend std::ostream &operator<<(std::ostream &os, const CartVec & p);
};


}
