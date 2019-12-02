#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H
#include <Eigen/Dense>
using namespace Eigen;


typedef Vector3d point;
typedef Vector3d direction;
/* the transformation between Homogeneous transformation matrix and Rotation Matrix */
Matrix3d tr2ro(Matrix4d);

/* the transformtion between Homogeneous transformstion matrix and eul angle(ZYZ),
    the default units is rad - mode = 'r'; degree - mode = 'd' */
Vector3d tr2eul(Matrix4d T, char mode = 'r');

/* the transformation between Rotation transformation matrix and eul angle(ZYZ),
    the default units is rad - mode = 'r'; degree - mode = 'd' */
Vector3d ro2eul(Matrix3d R, char mode = 'r');

/* the transformtion between Homogeneous transformstion matrix and rpy angle,
    the default units is rad - mode = 'r'; degree - mode = 'd' */
Vector3d tr2rpy(Matrix4d T, char mode = 'r');

/* the transformtion between Rotation transformstion matrix and rpy angle,
    the default units is rad - mode = 'r'; degree - mode = 'd' */
Vector3d ro2rpy(Matrix3d R, char mode = 'r');

/* the inverse transformation */
Matrix4d eul2tr(Vector3d eul, char mode = 'r');
Matrix3d eul2ro(Vector3d eul, char mode = 'r');
Matrix4d rpy2tr(Vector3d rpy, char mode = 'r');
Matrix3d rpy2ro(Vector3d rpy, char mode = 'r');
#endif // TRANSFORMATION_H

