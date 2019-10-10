#include "kinematics.h"
#include <cmath>


Vector3d tr2eul(Matrix4d Trans, char mode)
{
	double alpha, beta, gama;
	double sinbeta = sqrt(Trans(2, 0)*Trans(2, 0) + Trans(2, 1)*Trans(2, 1));
	if (abs(sinbeta) < 1e-4)
	{
		beta = atan2(0, Trans(2, 2));
		alpha = 0;
		gama = atan2(-Trans(0, 1), Trans(0, 0));
	}
	else
	{
		beta = atan2(sinbeta, Trans(2, 2));
		alpha = atan2(Trans(1, 2), Trans(0, 2));
		gama = atan2(Trans(2, 1), -Trans(2, 0));
	}
	Vector3d eul;
	if ('d' == mode)
	{
		alpha = alpha * 180 / PI;
		beta = beta * 180 / PI;
		gama = gama * 180 / PI;
	}
	eul << alpha, beta, gama;
	return eul;
}

Vector3f tr2eul(Matrix4f Trans, char mode)
{
	float alpha, beta, gama;
	float sinbeta = sqrt(Trans(2, 0)*Trans(2, 0) + Trans(2, 1)*Trans(2, 1));
	if (abs(sinbeta) < 1e-4)
	{
		beta = atan2(0, Trans(2, 2));
		alpha = 0;
		gama = atan2(-Trans(0, 1), Trans(0, 0));
	}
	else
	{
		beta = atan2(sinbeta, Trans(2, 2));
		alpha = atan2(Trans(1, 2), Trans(0, 2));
		gama = atan2(Trans(2, 1), -Trans(2, 0));
	}
	Vector3f eul;
	if ('d' == mode)
	{
		alpha = alpha * 180 / PI;
		beta = beta * 180 / PI;
		gama = gama * 180 / PI;
	}
	eul << alpha, beta, gama;
	return eul;
}

Vector3d tr2eul(Matrix3d Rotation, char mode)
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
		alpha = alpha * 180 / PI;
		beta = beta * 180 / PI;
		gama = gama * 180 / PI;
	}
	eul << alpha, beta, gama;
	return eul;
}

Vector3f tr2eul(Matrix3f Rotation, char mode)
{

	float alpha, beta, gama;
	float sinbeta = sqrt(Rotation(2, 0)*Rotation(2, 0) + Rotation(2, 1)*Rotation(2, 1));
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
	Vector3f eul;
	if ('d' == mode)
	{
		alpha = alpha * 180 / PI;
		beta = beta * 180 / PI;
		gama = gama * 180 / PI;
	}
	eul << alpha, beta, gama;
	return eul;
}

Vector3d tr2rpy(Matrix3d Rotation, char mode)
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
		Ry0 = Ry0 * 180 / PI;
		Rz0 = Rz0 * 180 / PI;
		Rx0 = Rx0 * 180 / PI;
	}
	rpy << Rx0, Ry0, Rz0;
	return rpy;
}
Vector3f tr2rpy(Matrix3f Rotation, char mode)
{
	float r11 = Rotation(0, 0);
	float r21 = Rotation(1, 0);
	float r23 = Rotation(1, 2);
	float r13 = Rotation(0, 2);
	float r31 = Rotation(2, 0);
	float r32 = Rotation(2, 1);
	float r33 = Rotation(2, 2);

    Vector3f rpy;
	float Rx0 = 0, Ry0 = 0, Rz0 = 0;
	float cosRy0 = sqrt(r11*r11 + r21*r21);
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
		Ry0 = Ry0 * 180 / PI;
		Rz0 = Rz0 * 180 / PI;
		Rx0 = Rx0 * 180 / PI;
	}
	rpy << Rx0, Ry0, Rz0;
	return rpy;
}

Matrix3f eul2ro(Vector3f eul, char mode)
{
	float alpha;
	float beta;
	float gama;
	if ('d' == mode)
	{
		alpha = eul(0)*PI/180;
		beta = eul(1)*PI/180;
		gama = eul(2)*PI/180;
	}
	else
	{
		alpha = eul(0);
		beta = eul(1);
		gama = eul(2);
	}
	Matrix3f rotation;
	rotation <<	cos(alpha)*cos(beta)*cos(gama)-sin(alpha)*sin(gama),	-cos(alpha)*cos(beta)*sin(gama)-sin(alpha)*cos(gama),	cos(alpha)*sin(beta),
				sin(alpha)*cos(beta)*cos(gama)+cos(alpha)*sin(gama),	-sin(alpha)*cos(beta)*sin(gama)+cos(alpha)*cos(gama),	sin(alpha)*sin(beta),
				-sin(beta)*cos(gama),									sin(beta)*sin(gama),									cos(beta);
	return rotation;
}

Matrix3d eul2ro(Vector3d eul, char mode)
{
	double alpha;
	double beta;
	double gama;
	if ('d' == mode)
	{
		alpha = eul(0)*PI / 180;
		beta = eul(1)*PI / 180;
		gama = eul(2)*PI / 180;
	}
	else
	{
		alpha = eul(0);
		beta = eul(1);
		gama = eul(2);
	}
	Matrix3d rotation;
	rotation << cos(alpha)*cos(beta)*cos(gama) - sin(alpha)*sin(gama), -cos(alpha)*cos(beta)*sin(gama) - sin(alpha)*cos(gama), cos(alpha)*sin(beta),
				sin(alpha)*cos(beta)*cos(gama) + cos(alpha)*sin(gama), -sin(alpha)*cos(beta)*sin(gama) + cos(alpha)*cos(gama), sin(alpha)*sin(beta),
				-sin(beta)*cos(gama),									sin(beta)*sin(gama),									cos(beta);
	return rotation;
}

Matrix3f rpy2ro(Vector3f rpy, char mode)
{
	float a = rpy(2);
	float B = rpy(1);
	float y = rpy(0);

	if ('d' == mode)
	{
		a = rpy(2)*PI / 180;
		B = rpy(1)*PI / 180;
		y = rpy(0)*PI / 180;
	}
	Matrix3f R70;
	R70 << cos(a)*cos(B), cos(a)*sin(B)*sin(y) - sin(a)*cos(y), cos(a)*sin(B)*cos(y) + sin(a)*sin(y),
		  sin(a)*cos(B), sin(a)*sin(B)*sin(y) + cos(a)*cos(y), sin(a)*sin(B)*cos(y) - cos(a)*sin(y),
		  -sin(B),       cos(B)*sin(y),                        cos(B)*cos(y);
	
	return R70;
}

Matrix3d rpy2ro(Vector3d rpy, char mode)
{
	double a = rpy(2);
	double B = rpy(1);
	double y = rpy(0);

	if ('d' == mode)
	{
		a = rpy(2)*PI / 180;
		B = rpy(1)*PI / 180;
		y = rpy(0)*PI / 180;
	}
    Matrix3d R70;
	R70 << cos(a)*cos(B), cos(a)*sin(B)*sin(y) - sin(a)*cos(y), cos(a)*sin(B)*cos(y) + sin(a)*sin(y),
		  sin(a)*cos(B), sin(a)*sin(B)*sin(y) + cos(a)*cos(y), sin(a)*sin(B)*cos(y) - cos(a)*sin(y),
		  -sin(B),       cos(B)*sin(y),                        cos(B)*cos(y);
	
	return R70;
}
