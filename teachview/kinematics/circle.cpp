#include "circle.h"
#include <iostream>
#include <cmath>
using namespace std;
using namespace Eigen;

#ifndef pi
#define pi 3.1415926545
#endif

Circle::Circle(point start, point center, double radius, direction axis)
{
	o = center;
	r = radius;
	Z = axis;
	s = start;
	e = start;
}

Circle::Circle(point start, point inter, point end)
{
	s = start;
	i = inter;
	e = end;

	/* solve the center point o of circle */
	Vector3d A1 = inter - start;
	Vector3d A2 = end - inter;
	Matrix2d A;
	A(0, 0) = A1.dot(A1);
	A(0, 1) = A(1, 0) = A1.dot(A2);
	A(1, 1) = A2.dot(A2);
	Vector2d b;
	b << 0.5*A1.dot(A1), 0.5*A2.dot(A2) + A1.dot(A2);
	Vector2d xy = A.colPivHouseholderQr().solve(b);
	Vector3d so = xy(0)*A1 + xy(1)*A2;
	o = s + so;
	r = so.norm();

	X = -so;

	/* solve the Y and Z coordiante axis of circle*/
	if (abs(b[1]) < 1e-6)
	{
		Y = A2;
	}
	else
	{
		Y = A1 + (-b[0]/b[1])*A2;
	}
	Z = X.cross(Y);
	cout << "X: " << X << endl;
	cout << "Y: " << Y << endl;
	cout << "Y: " << Z << endl;
	Matrix3d B;
	B.col(0) = X;
	B.col(1) = Y;
	B.col(2) = Z;
	cout << "B: " << B << endl;
	Vector3d oi = -so + i - s;
	Vector3d oe = oi + e - i;
	Vector3d xyzi = B.colPivHouseholderQr().solve(oi);
	Vector3d xyze = B.colPivHouseholderQr().solve(oe);
	double theta_i = atan2(xyzi[1], xyzi[0]);
	if (theta_i < 0) {
		theta_i += 2 * pi;
	}
	double theta_e = atan2(xyze[1], xyze[0]);
	if (theta_e < 0){
		theta_e += 2 * pi;
	}

	if (theta_e < theta_i)
	{
		Y = -Y;
		Z = X.cross(Y);

		/* recaculate them */
		B.col(0) = X;
		B.col(1) = Y;
		B.col(2) = Z;
		xyzi = B.colPivHouseholderQr().solve(oi);
		xyze = B.colPivHouseholderQr().solve(oe);
		theta_i = atan2(xyzi[1], xyzi[0]);
		if (theta_i < 0) {
			theta_i += 2 * pi;
		}
		theta_e = atan2(xyze[1], xyze[0]);
		if (theta_e < 0) {
			theta_e += 2 * pi;
		}
	}

	/* caculate the arc length */
	arc = r * theta_e;

}

Circle::Circle(point start, point center, direction axis)
{

}

Matrix4d Circle::SE3()
{
	Matrix4d se3;
	se3.block(0, 0, 3, 1) = X;
	se3.block(0, 1, 3, 1) = Y;
	se3.block(0, 2, 3, 1) = Z;
	se3.block(0, 3, 3, 1) = o;
	se3.block(3, 0, 1, 4) << 0, 0, 0, 1;
	return se3;
}

Matrix3d Circle::SO3()
{
	Matrix3d so3;
	so3.col(0) = X;
	so3.col(1) = Y;
	so3.col(2) = Z;
	return so3;
}

Vector3d Circle::Pc()
{
	return o;
}

double Circle::arclen()
{
	return arc;
}

Vector3d Circle::coordposi(double len)
{
	Vector3d p;
	p << r*cos(len / r), r*sin(len / r), 0;
	return p;
}

Circle::~Circle()
{

}

