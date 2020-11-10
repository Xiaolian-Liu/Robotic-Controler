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
	
	RKD::JointVec jvec(1, 2, 3, 4, 5, 6);
	jvec = { 1,2,3,4,5,6 };
	cout << jvec << endl;
	int a;
	std::cin >> a;
	return 0;

}