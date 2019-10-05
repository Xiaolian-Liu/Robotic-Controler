#ifndef KINEMATICS_H
#define KINEMATICS_H
#ifndef PI
#define PI 3.1415926545
#endif
#include <vector>
#include <Eigen/Dense>
using namespace Eigen;
typedef std::vector<double> vectord;    //The STL of C++

Vector3d tr2eul(Matrix4d Trans, char mode = 'r');
Vector3f tr2eul(Matrix4f Trans, char mode = 'r');
Vector3d tr2eul(Matrix3d Rotation, char mode = 'r');
Vector3f tr2eul(Matrix3f Rotation, char mode = 'r');

Matrix3f eul2ro(Vector3f eul, char mode = 'r');
Matrix3d eul2ro(Vector3d eul, char mode = 'r');


/*****************************************************************
	Unit Linear interpolation with parabolic transitions in joint space
	a--the acceleration
	v--the velocity
	x0--the start position
	xf--the final position
	f--the frequency
	return vector -- the uint sequency
*****************************************************************/
vectord julipt(double x0, double xf, double a, double v, int f = 500);

/*****************************************************************
	Linear interpolation with parabolic transitions in joint space
	a--the acceleration
	v--the velocity
	x0--the start position
	xf--the final position
	f--the frequency
	return vector -- the position sequency
*****************************************************************/
vectord jlipt(double x0, double xf, double a, double v, int f = 500);





#endif // !KINEMATICS_H
