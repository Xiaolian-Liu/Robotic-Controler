#include "Link.h"

namespace RKD {



Link::Link()
{
	alpha = 0;
	a = 0;
	theta = 0;
	d = 0;
	revolute = true;
	mdh = false;
	offset = 0;
	flip = false;

	m = 0;
	r << 0, 0, 0;
	I.setZero();

	Jm = 0;
	G = 1;
	B = 0;
	Tc.setZero();
}

Link::Link(double theta, double d, double a, double alpha, double offset, bool modified, bool isRevolute)
{
	this->theta = theta;
	this->d = d;
	this->a = a;
	this->alpha = alpha;
	this->offset = offset;
	mdh = modified;
	this->revolute = isRevolute;
	flip = false;

	m = 0;
	r << 0, 0, 0;
	I.setZero();

	Jm = 0;
	G = 1;
	B = 0;
	Tc.setZero();
}

Link:: ~Link()
{

}

// set the DH parameters of the link
void Link::setDh(const Vector4d & dh, double offset)
{
	this->theta = dh(1);
	this->d = dh(2);
	this->a = dh(3);
	this->alpha = dh(4);
	this->offset = offset;
}

// Link.A Link transform matrix
Matrix4d Link::A(double q)
{
	// TODO: Add your implementation code here.
	Matrix4d res;

	double sa = sin(this->alpha); 
	double ca = cos(this->alpha);
	if (this->flip)
	{
		q = -q + this->offset;
	}
	else
	{
		q = q + this->offset;
	}
	double st, ct, d;
	if (revolute)
	{
		st = sin(q); 
		ct = cos(q);
		d = this->d;
	}
	else
	{
		st = sin(this->theta); 
		ct = cos(this->theta);
		d = q;
	}

	if (this->mdh)
	{
		res <<	ct,		-st,		0,		this->a,
				st*ca,	ct*ca,		-sa,	-sa*d,
				st*sa,  ct*sa,		ca,		ca*d,
				0,		0,			0,		1;
	}
	else
	{
		res <<	ct,		-st*ca,		st*sa,   this->a*ct,
				st,		ct*ca,		-ct*sa,  this->a*st,
				0,		sa,			ca,      d,
				0,		0,			0,       1;
	}
	return res;
}


}




