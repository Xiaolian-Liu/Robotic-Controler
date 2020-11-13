#pragma once
#include <Eigen/Dense>
#include "transform.h"

using namespace Eigen;

namespace RKD {


class CartVec
{
private:
	Vector3d pe;
	Vector3d rpy; //rad
public:

	CartVec();
	CartVec(const Vector3d &pe, const Vector3d &rpy);
	~CartVec();
	CartVec(const CartVec & p);
	CartVec(double x, double y, double z, double rx, double ry, double rz);
	CartVec(Matrix4d T);
	void operator=(const CartVec & p);
	friend std::ostream &operator<<(std::ostream &os, const CartVec & p);
	Matrix4d Transl() const;
	Vector3d PE() const;
	Vector3d RPY() const;

};


}
