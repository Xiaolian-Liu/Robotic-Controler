#pragma once
#include <Eigen/Dense>
#include <string>



using std::string;
using namespace Eigen;

namespace RKD {
/*
* the link class, for constructed the serialLink class
*/
class Link
{
public:

	double theta;
	double d;
	double a;
	double alpha;

	bool revolute;
	bool mdh;
	double offset;
	string name;
	bool flip;

	double m;
	Vector3d r;
	Matrix3d I;
	double Jm;
	double B;

	Vector2d Tc;
	double G;
	Vector2d qlim;

	Link();
	Link(double theta, double d, double a, double alpha, double offset, bool modified, bool isRevolute = true);
	
	virtual ~Link();
	// set the DH parameters of the link, 
	// dh:the four coloums vector stores the DH parameter
	void setDh(const Vector4d & dh, double offset);
	Matrix4d A(double q);
};



}
