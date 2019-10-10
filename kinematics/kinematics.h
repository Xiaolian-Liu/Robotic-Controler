#ifndef KINEMATICS_H
#define KINEMATICS_H

#ifndef PI
#define PI 3.1415926545
#endif

#include <Eigen/Dense>
using namespace Eigen;

Vector3d tr2eul(Matrix4d Trans, char mode = 'r');
Vector3f tr2eul(Matrix4f Trans, char mode = 'r');
Vector3d tr2eul(Matrix3d Rotation, char mode = 'r');
Vector3f tr2eul(Matrix3f Rotation, char mode = 'r');
Vector3d tr2rpy(Matrix3d Rotation, char mode = 'r');
Vector3f tr2rpy(Matrix3f Rotation, char mode = 'r');

Matrix3f eul2ro(Vector3f eul, char mode = 'r');
Matrix3d eul2ro(Vector3d eul, char mode = 'r');
Matrix3f rpy2ro(Vector3f rpy, char mode = 'r');
Matrix3d rpy2ro(Vector3d rpy, char mode = 'r');




#endif // !KINEMATICS_H
