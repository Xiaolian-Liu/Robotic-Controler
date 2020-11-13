#include <iostream>
#include <Eigen/Dense>
#include "CartVec.h"
#include "JointVec.h"
using namespace Eigen;
using std::cout;
using std::endl;
int main()
{
	/*Matrix4d m;
	m << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16;
	cout << m << endl;;
	m.setIdentity();*/
	Vector2d v(1,2);
	cout << v << endl;
	//cout << m;
	RKD::CartVec vec(1, 2, 3, 90, 90, -180);
	vec = { 1,4,5,20,40,90 };
	cout << vec << endl;
	
	RKD::JointVec jvec1(1, 2, 3, 4, 5, 6);
	jvec1= { 1,2,3,4,5,6 };
	cout << jvec1 << endl;

	RKD::JointVec jvec2(2, 3, 4, 5, 6, 7);
	RKD::JointVec jvec3 = Matrix<double, 6, 1>(jvec1 + jvec2);
	//jvec3 = Matrix<double, 6, 1>(jvec1 + jvec2);
	jvec3 = jvec1 + jvec2;

	cout << jvec3 << endl;

	Matrix<double, 6, 1> m1;
	m1 << 0, 1, 2, 3, 4, 5;
	Matrix<double, 6, 1> m2;
	m2 << 2, 3, 4, 5, 6, 7;
	Matrix<double, 6, 1> m3 = m1 + m2;
	m3 = m1 + m2;

	RKD::JointVec jvec4 = m1;
	cout << jvec4 << endl;


	int a;
	std::cin >> a;
	return 0;

}