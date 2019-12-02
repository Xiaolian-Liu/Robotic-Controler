#include "transform.h"
#include <cmath>

#ifndef pi
#define pi	3.1415926545
#endif

static inline double degree2rad(double deg){ return deg*pi/180; }
static inline double rad2degree(double rad){ return rad*180/pi; }

Matrix3d tr2ro(Matrix4d T)
{
	return T.block<3, 3>(0, 0);
}

Vector3d tr2eul(Matrix4d Trans, char mode)
{
	Matrix3d R;
	R = tr2ro(Trans);
	return ro2eul(R, mode);
}

Vector3d ro2eul(Matrix3d Rotation, char mode)
{
	double alpha, beta, gama;
	double sinbeta = sqrt(Rotation(2, 0)*Rotation(2, 0) + Rotation(2, 1)*Rotation(2, 1));
	if (abs(sinbeta) < 1e-4)
	{
		beta = atan2(0, Rotation(2, 2));
		alpha = 0;
		gama = atan2(-Rotation(0, 1), Rotation(0, 0));
	}
	else
	{
		beta = atan2(sinbeta, Rotation(2, 2));
		alpha = atan2(Rotation(1, 2), Rotation(0, 2));
		gama = atan2(Rotation(2, 1), -Rotation(2, 0));
	}
	Vector3d eul;
	if ('d' == mode)
	{
		alpha	= rad2degree(alpha);
		beta	= rad2degree(beta);
		gama	= rad2degree(gama);
	}
	eul << alpha, beta, gama;
	return eul;
}

Vector3d tr2rpy(Matrix4d Trans, char mode)
{
	Matrix3d R;
	R = tr2ro(Trans);
	return ro2rpy(R, mode);
}

Vector3d ro2rpy(Matrix3d Rotation, char mode)
{
	double r11 = Rotation(0, 0);
	double r21 = Rotation(1, 0);
	double r23 = Rotation(1, 2);
	double r13 = Rotation(0, 2);
	double r31 = Rotation(2, 0);
	double r32 = Rotation(2, 1);
	double r33 = Rotation(2, 2);

	Vector3d rpy;
	double Rx0 = 0, Ry0 = 0, Rz0 = 0;
	double cosRy0 = sqrt(r11*r11 + r21*r21);
	if (cosRy0 < 1e-4) //estimate if cpsRy0 = 0
	{
		Ry0 = atan2(-r31, 0);
		Rz0 = atan2(-r31*r23, -r31*r13);
		Rx0 = 0;
	}
	else
	{
		Ry0 = atan2(-r31, cosRy0);
		Rz0 = atan2(r21, r11);
		Rx0 = atan2(r32, r33);
	}
	if ('d' == mode)
	{
		Ry0 = rad2degree(Ry0);
		Rz0 = rad2degree(Rz0);
		Rx0 = rad2degree(Rx0);
	}
	rpy << Rx0, Ry0, Rz0;
	return rpy;
}


Matrix3d eul2ro(Vector3d eul, char mode)
{
	double alpha = eul(0);
	double beta = eul(1);
	double gama = eul(2);
	if ('d' == mode)
	{
		alpha = degree2rad(alpha);
		beta = degree2rad(beta);
		gama = degree2rad(gama);
	}
	Matrix3d rotation;
	rotation << cos(alpha)*cos(beta)*cos(gama) - sin(alpha)*sin(gama), -cos(alpha)*cos(beta)*sin(gama) - sin(alpha)*cos(gama), cos(alpha)*sin(beta),
				sin(alpha)*cos(beta)*cos(gama) + cos(alpha)*sin(gama), -sin(alpha)*cos(beta)*sin(gama) + cos(alpha)*cos(gama), sin(alpha)*sin(beta),
				-sin(beta)*cos(gama),									sin(beta)*sin(gama),									cos(beta);
	return rotation;
}


Matrix3d rpy2ro(Vector3d rpy, char mode)
{
	double a = rpy(2);
	double B = rpy(1);
	double y = rpy(0);

	if ('d' == mode)
	{
		a = degree2rad(a);
		B = degree2rad(B);
		y = degree2rad(y);
	}
    Matrix3d R70;
	R70 << cos(a)*cos(B), cos(a)*sin(B)*sin(y) - sin(a)*cos(y), cos(a)*sin(B)*cos(y) + sin(a)*sin(y),
		   sin(a)*cos(B), sin(a)*sin(B)*sin(y) + cos(a)*cos(y), sin(a)*sin(B)*cos(y) - cos(a)*sin(y),
		   -sin(B),       cos(B)*sin(y),                        cos(B)*cos(y);
	
	return R70;
}
