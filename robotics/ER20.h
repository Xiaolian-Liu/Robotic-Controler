#pragma once
#ifndef PI
#define PI 3.14159265358979323846
#endif

/* can't define pi, because the Eigen use pi,
	if pi is defined here, compile err occurs */
	//#ifndef pi
	//#define pi 3.14159265358979323846
	//#endif

#ifndef PI_2
#define PI_2 1.57079632679489661923
#endif // !pi_2

#include <stdint.h>		/* where int32_t is included */
#include <Eigen/Dense>
//#include <Eigen/StdVector> //need include it, when using STL Containers,
// if use -std=C++17, or C++11 no need!
// http://eigen.tuxfamily.org/dox-devel/group__TopicStlContainers.html
#include <vector>
#include <map>
#include "Drive.h"
/*
#ifndef int32_t
#define int32_t int
#endif
*/

/* don't forget that using namespace */
using namespace Eigen;
using std::vector;
///The datatype

//The D-H linkage parameters, the length unit: mm, the angular unit: rad/degree
#define ER20_a1  0.16846
#define ER20_a2  0.78155
#define ER20_a3  0.14034
#define ER20_a4  0
#define ER20_a5  0
#define ER20_a6  0

#define alpha1	PI_2
#define alpha2	0
#define alpha3	PI_2
#define alpha4	-PI_2
#define alpha5	PI_2
#define alpha6	0

#define ER20_d1  0.504
#define ER20_d2  0
#define ER20_d3  (-0.0003)
#define ER20_d4  0.76039
#define ER20_d5  0
#define ER20_d6  0.125
#define ER20_pe	 0.206
#define ER20_pen 0.070
/* the offset between the D-H system and the Jointangle */
#define offset1 0
#define offset2 PI_2
#define offset3 0
#define offset4 0
#define offset5 0
#define offset6 0


// The reduction ratio of axies
#define ER20_I1 		(-147)
#define ER20_I2 		179.18
#define ER20_I3 		(-164.07)
#define ER20_I4 		(-76.95)
#define ER20_I5 		80
#define ER20_I6 		(-51)
#define ER20_I56		51

// The Spherical wrist torque, units: N.m
#define ER20_T4 49
#define ER20_T5 49
#define ER20_T6 23.5

// The Moment of inertia, units: kg.m^2
#define ER20_J4	1.6
#define ER20_J5	1.6
#define ER20_J6	0.8

// The Maximum and minimum values

//  degree/s^2 , the max Acceleration is three times of max velocity
#define MAXACC1		510
#define MAXACC2		495
#define MAXACC3		510
#define MAXACC4		1080
#define MAXACC5		1080
#define MAXACC6		1800

//  degree/s
#define MAXVEL1		170
#define MAXVEL2		165
#define MAXVEL3		170
#define MAXVEL4		360
#define MAXVEL5		360
#define MAXVEL6		600


// degree
#define MAXANG1		90
#define MAXANG2		64
#define MAXANG3		165
#define MAXANG4		178
#define MAXANG5		132
#define MAXANG6		180

#define MINANG1		-90
#define MINANG2		-142
#define MINANG3		-73
#define MINANG4		-178
#define MINANG5		-132
#define MINANG6		-180

//the max cart speed mm/s
#define LMAXVEL     1000
//the max cart acceleration mm/s^2
#define LMAXACC     2000
//the max rotation speed in cart space, uinits: degree/s
#define RMAXVEL		180
//the max rotation acceleration in cart space, uinits: degree/s^2
#define RMAXACC		540


#include "SerialLink.h"

namespace RKD {

class ER20 :
	public SerialLink
{
private:
	const double a1 = ER20_a1;
	const double d1 = ER20_d1;
	const double a2 = ER20_a2;
	const double a3 = ER20_a3;
	const double d3 = ER20_d3;
	const double d4 = ER20_d4;
	const double d6 = ER20_d6 + ER20_pe + ER20_pen;

	vector<double> a = { 0.16846, 0.78155, 0.14034, 0 , 0, 0 };

	vector<double> alpha = { PI_2, 0, PI_2, -PI_2, PI_2, 0 };

	vector<double> d = { 0.504, 0, -0.3, 0.76039, 0, 0.125 };
	
	JointVec offset = { 0, PI_2, 0, 0, 0, 0 };


public:
	ER20(bool modified = true);
	virtual Matrix4d fkinef(JointVec q);	//units rad
	virtual int ikine(JointVec &q, JointVec q0, Matrix4d T);  // units:rad
	~ER20();
};


}
