#ifndef KINEMATICS_H
#define KINEMATICS_H

#ifndef PI
#define PI 3.14159265358979323846
#endif

#include <Eigen/Dense>
using namespace Eigen;

typedef Vector3d point;
typedef Vector3d direction;

/* the transformation between Homogeneous transformation matrix and Rotation Matrix */
Matrix3d tr2ro(Matrix4d Trans);

/* the transformtion between Homogeneous transformstion matrix and eul angle(ZYZ) or rpy angle,
	the default units is rad - mode = 'r'; degree - mode = 'd' */
Vector3d tr2eul(Matrix4d Trans, char mode = 'r');
Vector3d ro2eul(Matrix3d Rotation, char mode = 'r');

Vector3d tr2rpy(Matrix4d Trans, char mode = 'r');
Vector3d ro2rpy(Matrix3d Rotation, char mode = 'r');

Matrix3d eul2ro(Vector3d eul, char mode = 'r');
Matrix3d rpy2ro(Vector3d rpy, char mode = 'r');

/*  R = ANGVEC2R(THETA, K) is an orthonormal rotation matrix (3x3)
  equivalent to a rotation of THETA about the vector K */
Matrix3d angvec2r(double theta, Vector3d k, char mode = 'r');

int tr2angvec(double & theta, Vector3d & k, Matrix3d R, char mode = 'r');

Matrix3d rotx(double theta, char mode = 'r');
Matrix3d roty(double theta, char mode = 'r');
Matrix3d rotz(double theta, char mode = 'r');

Matrix3d skew(Vector3d k);

/* [theta,w] = trlog(R) as above but returns directly theta the rotation
% angle and w (3x1) the unit-vector indicating the rotation axis. */
int trlog(double & theta, Vector3d & w, Matrix3d R, char mode = 'r');

Vector3d vex(Matrix3d S);

/* R = OA2R(O, A) is an SO(3) rotation matrix (3x3) for the specified
% orientation and approach vectors (3x1) formed from 3 vectors such that R
% = [N O A] and N = O x A. */
Matrix3d oa2r(Vector3d o, Vector3d a);
#endif // !KINEMATICS_H
