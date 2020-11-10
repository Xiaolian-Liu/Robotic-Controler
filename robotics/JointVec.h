#pragma once
#include <ostream>
#include <Eigen/Dense>
using namespace Eigen;

namespace RKD {

class JointVec :
	public Matrix<double, 6, 1>
{
public:
	JointVec();
	JointVec(double j1, double j2, double j3, double j4, double j5, double j6);
	friend std::ostream & operator<<(std::ostream & os, const JointVec j);
	~JointVec();
};

}
