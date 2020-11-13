#pragma once
#include <ostream>
#include <Eigen/Dense>

using namespace Eigen;

namespace RKD {

#ifndef PI
#define PI 3.14159265358979323846
#endif

class JointVec :
	public Matrix<double, 6, 1>
{
public:
	JointVec();
	JointVec(double j1, double j2, double j3, double j4, double j5, double j6);
	JointVec(const Matrix<double, 6, 1> & m);
	JointVec & operator=(const Matrix<double, 6, 1> & m);

	/*template<typename OtherDerived>
	JointVec & operator=(const DenseBase<OtherDerived>& other)
	{
		return Matrix<double,6,1>(Base::_set(other)
	}*/

	friend std::ostream & operator<<(std::ostream & os, const JointVec j);
	JointVec rad();
	~JointVec();
};

}
