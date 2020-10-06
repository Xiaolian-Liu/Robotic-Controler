#include "transform.h"
#include <cmath>
#include <cfloat>
#include <iostream>
#include <cstdlib>
using std::cout;
using std::endl;

#ifndef pi
#define pi	3.14159265358979323846
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

Vector3d tr2eul(Matrix3d R, string opt)
{
	if (!isrot(R))
	{
		cout << "error----tr2eul:isrot:badarg, expecting an 3X3 orthogonal matrix\n";
	}
	R.col(0).normalize();
	R.col(1).normalize();
	R.col(2).normalize();
	Vector3d eul;  // eul = [phi theta psi]
	if (abs(R(0, 2)) < DBL_EPSILON && abs(R(1, 2)) < DBL_EPSILON)
	{
		//singularity
		eul(0) = 0;
		double sp = 0;
		double cp = 1;
		eul(1) = atan2(cp*R(0, 2)+sp*R(1, 2),  R(2, 2));
		eul(2) = atan2(-sp*R(0, 0)+cp* R(1, 0),  -sp*R(0, 1)+cp*R(1, 1));
	}
	else
	{
		//non singularity
		if (string::npos != opt.find("flip"))
		{
			eul(0) = atan2(-R(1, 2), -R(0, 2));
		}
		else
		{
			eul(0) = atan2(R(1, 2), R(0, 2));
		}
		double sp = sin(eul(0));
		double cp = cos(eul(0));
		eul(1) = atan2(cp*R(0, 2)+sp*R(1, 2),   R(2, 2));
		eul(2) = atan2(-sp*R(0, 0)+cp*R(1, 0),  -sp*R(0, 1)+cp* R(1, 1));
	}
	if (string::npos != opt.find("deg"))
	{
		eul = eul * 180 / pi;
	}
	return eul;
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

Matrix3d skew(Vector3d k)
{
	Matrix3d S;
	S << 0,		-k(2),	k(1),
		k(2),	0,		-k(0),
		-k(1),	k(0),	0;
	return S;
}
Matrix2d skew(double k)
{
	Matrix2d S;
	S << 0, -k,
		 k,  0;
	return S;
}

Matrix3d angvec2r(double theta, Vector3d k, char mode)
{
	Matrix3d R;
	if (k.norm() < 10 * DBL_EPSILON)
	{
		if (abs(theta) > 0)
		{
			cout << "angvec2r:badarg, norm of direction is zero\n";
			exit(EXIT_FAILURE);
		}
		else
		{
			R = Matrix3d::Identity();
			return R;
		}
	}
	if ('d' == mode)
	{
		theta = theta * pi / 180;
	}
	k.normalize();
	Matrix3d sk = skew(k);
	R = Matrix3d::Identity() + sin(theta)*sk + (1 - cos(theta))*sk*sk;
	return R;
}

int tr2angvec(double & theta, Vector3d & k, Matrix3d R, char mode)
{
	for (int i = 0; i < 3; i++)
	{
		R.row(i).normalize();
	}

	if (abs(R.determinant() - 1) > 10 * DBL_EPSILON)
	{
		cout << "tr2angvec:badarg, matrix is not orthonormal" << endl;
		return -1;
	}
	Vector3d v;
	double th;
	trlog(th, v, R);
	theta = th;
	k = v;

	if ('d' == mode)
	{
		theta = theta * 180 / pi;
	}
	return 0;
}

int trlog(double & theta, Vector3d & w, Matrix3d R, char mode)
{
	if (abs(R.trace() - 3) < 100 * DBL_EPSILON)
	{
		w << 0, 0, 0;
		theta = 0;
	}
	else if (abs(R.trace()+1) < 100*DBL_EPSILON)
	{
		Vector3d dia = R.diagonal();
		int k = 0;
		double mx = dia.maxCoeff(&k);
		Matrix3d I = Matrix3d::Identity();
		Vector3d col = R.col(k) + I.col(k);
		w = col / sqrt(2 * (1 + mx));
		theta = pi;
	}
	else
	{
		theta = acos((R.trace() - 1) / 2);
		Matrix3d skw = (R - R.inverse()) / 2 / sin(theta);
		w = vex(skw);
	}
	if ('d' == mode)
	{
		theta = theta * 180 / pi;
	}
	return 0;
}

Vector3d vex(Matrix3d S)
{
	Vector3d v;
	v <<	0.5*(S(2, 1) - S(1, 2)),	
			0.5*(S(0, 2) - S(2, 0)),	
			0.5*(S(1, 0) - S(0, 1));
	return v;
}

Matrix3d rotx(double theta, char mode)
{
	if ('d' == mode)
	{
		theta = theta * pi / 180;
	}
	double ct = cos(theta);
	double st = sin(theta);

	if (abs(ct) < DBL_EPSILON)
		ct = 0;
	if (abs(st) < DBL_EPSILON)
		st = 0;

	Matrix3d R;
	R <<	1,	0,	0,
			0,	ct,	-st,
			0,	st,	ct;
	return R;
}

Matrix3d roty(double theta, char mode)
{
	if ('d' == mode)
	{
		theta = theta * pi / 180;
	}
	double ct = cos(theta);
	double st = sin(theta);

	if (abs(ct) < DBL_EPSILON)
		ct = 0;
	if (abs(st) < DBL_EPSILON)
		st = 0;

	Matrix3d R;
	R <<	ct,		0,		st,
			0,		1,		0,
			-st,	0,		ct;

	return R;
}

Matrix3d rotz(double theta, char mode)
{
	if ('d' == mode)
	{
		theta = theta * pi / 180;
	}
	double ct = cos(theta);
	double st = sin(theta);

	if (abs(ct) < DBL_EPSILON)
		ct = 0;
	if (abs(st) < DBL_EPSILON)
		st = 0;

	Matrix3d R;
	R <<	ct,	-st,	0,
			st,	ct,		0,
			0,	0,		1;
	return R;
}

Matrix3d oa2r(Vector3d o, Vector3d a)
{
	Vector3d n = o.cross(a);
	o = a.cross(n);
	n.normalize();
	o.normalize();
	a.normalize();
	Matrix3d R;
	R.col(0) = n;
	R.col(1) = o;
	R.col(2) = a;
	return R;
}

bool isrot(Matrix3d R)
{
	/* 100*DBL_EPSILON to lower the precision of caculation */
	if (abs(R.determinant() - 1) < 100*DBL_EPSILON)
	{
		return true;
	}
	else
	{
		return false;
	}
}

inline bool ishomog(Matrix4d T)
{
	int ret = false;
	Matrix3d R;
	R = T.block(0, 0, 3, 3);
	if (abs(R.determinant() - 1) < 10*DBL_EPSILON && abs(T.block(3,0,1,3).norm()) < 10*DBL_EPSILON &&
		abs(T(3,3)-1) < 10*DBL_EPSILON)
	{
		ret = true;
	}
	return ret;
}

Vector3d transl(Matrix4d T)
{
	if (ishomog(T))
	{
		return T.block(0, 3, 3, 1);
	}
	else
	{
		cout << "transl:badarg, the matrix argument is not homogo\n";
		exit(EXIT_FAILURE);
	}
}

Matrix4d transl(Vector3d p, Matrix3d R)
{
	if (!isrot(R))
	{
		cout << "transl:badarg, the 3X3 matrix is not a Rotation\n";
		//exit(EXIT_FAILURE);
	}
	Matrix4d T = Matrix4d::Identity();
	T.block(0, 0, 3, 3) = R;
	T.block(0, 3, 3, 1) = p;
	return T;
}