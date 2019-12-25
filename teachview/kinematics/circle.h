#ifndef CIRCLE_H
#define CIRCLE_H
#include <Eigen/Dense>
using namespace Eigen;

#ifndef point
typedef Vector3d point;

#endif // !point

class direction : public Vector3d
{
public:
	direction()
	{

	}
	direction(const Vector3d & d) : Vector3d(d/d.norm())
	{

	}
	void operator=(const Vector3d & d)
	{
		Vector3d temp = d;
		temp.normalize();
		this->block(0, 0, 3, 1) << temp[0], temp[1], temp[2];
		this->normalize();
	}
};


class Circle
{
public:
	Circle(point start, point center, double radius, direction axis);
	Circle(point start, point inter, point end);
	Circle(point start, point center, direction axis);
    ~Circle();
	Matrix4d SE3();
	Matrix3d SO3();
	Vector3d Pc();
	double arclen();
	double circumference();

	/* return the coordinate position of the point in the arc, 
	the input is the arc length between the point we caculate and the begin point */
	Vector3d coordposi(double len); 
	


private:
	point s;		//the arc starts in point s;
	point e;		//the arc ends in point e;
	point i;		//the arc intermediate point i;
	point o;		//the center
	double r;		//the radius
	direction Z;	//the axis direction
	direction X;
	direction Y;
	double arc;
};

#endif // CIRCLE_H
