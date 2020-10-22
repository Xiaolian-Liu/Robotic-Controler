#ifndef TRANSFORM_H
#define TRANSFORM_H

#ifndef PI
#define PI 3.14159265358979323846
#endif
#ifndef PI_2
#define PI_2 1.57
#endif // !PI_2


#include <Eigen/Dense>
using namespace Eigen;
#include <string>
using std::string;

typedef Vector3d point;
//typedef Vector3d direction;

/* the transformation between Homogeneous transformation matrix and Rotation Matrix */
Matrix3d tr2ro(Matrix4d Trans);

/* the transformtion between Homogeneous transformstion matrix and eul angle(ZYZ) or rpy angle,
	the default units is rad - mode = 'r'; degree - mode = 'd' */
Vector3d tr2eul(Matrix4d Trans, char mode = 'r');

/* EUL = TR2EUL(T, OPTIONS) are the ZYZ Euler angles(1x3) corresponding to
   the rotational part of a homogeneous transform T(4x4).The 3 angles
   EUL = [PHI, THETA, PSI] correspond to sequential rotations about the Z, Y and
   Z axes respectively.
  
   EUL = TR2EUL(R, OPTIONS) as above but the input is an orthonormal
   rotation matrix R(3x3). 

   Options::
    'deg'      Compute angles in degrees (radians default)
    'flip'     Choose first Euler angle to be in quadrant 2 or 3.
  
   Notes::
   - There is a singularity for the case where THETA=0 in which case PHI is arbitrarily
     set to zero and PSI is the sum (PHI+PSI). 
	tested by Xiaolian Liu */
Vector3d tr2eul(Matrix3d Rotation, string option = "rad");

Vector3d ro2eul(Matrix3d Rotation, char mode = 'r');

Vector3d tr2rpy(Matrix4d Trans, char mode = 'r');
Vector3d ro2rpy(Matrix3d Rotation, char mode = 'r');

Matrix3d eul2ro(Vector3d eul, char mode = 'r');
Matrix3d rpy2ro(Vector3d rpy, char mode = 'r');

/*  R = ANGVEC2R(THETA, K) is an orthonormal rotation matrix (3x3)
  equivalent to a rotation of THETA about the vector K 
% Notes::
% - If THETA == 0 then return identity matrix.
% - If THETA ~= 0 then V must have a finite length. 
   tested by Xiaolian Liu*/
Matrix3d angvec2r(double theta, Vector3d k, char mode = 'r');

int tr2angvec(double & theta, Vector3d & k, Matrix3d R, char mode = 'r');

/* Rotation around the X,Y,Z axies; tested by Xiaolian Liu*/
Matrix3d rotx(double theta, char mode = 'r');
Matrix3d roty(double theta, char mode = 'r');
Matrix3d rotz(double theta, char mode = 'r');


/* %SKEW Create skew-symmetric matrix
%
% S = SKEW(V) is a skew-symmetric matrix formed from V.
% 
% If V (1x1) then S =
%
%           | 0  -v |
%           | v   0 |
%
% and if V (1x3) then S =
%
%           |  0  -vz   vy |
%           | vz    0  -vx |
%           |-vy   vx    0 |
%
%
% Notes::
% - This is the inverse of the function VEX().
% - These are the generator matrices for the Lie algebras so(2) and so(3).
% this function is tested by Xiaolian Liu */
Matrix3d skew(Vector3d k);
Matrix2d skew(double k);

/* [theta,w] = trlog(R) as above but returns directly theta the rotation
% angle and w (3x1) the unit-vector indicating the rotation axis. */
int trlog(double & theta, Vector3d & w, Matrix3d R, char mode = 'r');

Vector3d vex(Matrix3d S);

/* R = OA2R(O, A) is an SO(3) rotation matrix (3x3) for the specified
% orientation and approach vectors (3x1) formed from 3 vectors such that R
% = [N O A] and N = O x A. 
  tested by Xiaolian Liu
*/
Matrix3d oa2r(Vector3d o, Vector3d a);

/* judge the 3X3 Rotation Matrix is a orthonormal matrix; 
	tested by Xiaolian-Liu*/
bool isrot(Matrix3d Rotation);

inline bool ishomog(Matrix4d T);

Vector3d transl(Matrix4d Transformation);
Matrix4d transl(Vector3d p, Matrix3d R);

#endif // !TRANSFORM_H
