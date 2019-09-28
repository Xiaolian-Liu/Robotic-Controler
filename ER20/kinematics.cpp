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