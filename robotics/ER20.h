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
#define ER20_a1  168.46
#define ER20_a2  781.55
#define ER20_a3  140.34
#define ER20_a4  0
#define ER20_a5  0
#define ER20_a6  0

#define alpha1	PI_2
#define alpha2	0
#define alpha3	PI_2
#define alpha4	-PI_2
#define alpha5	PI_2
#define alpha6	0

#define ER20_d1  504
#define ER20_d2  0
#define ER20_d3  (-0.3)
#define ER20_d4  760.39
#define ER20_d5  0
#define ER20_d6  125
#define ER20_pe	 206
#define ER20_pen 70
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


#include "SerialLink.h"
namespace RKD {

class ER20 :
	public SerialLink
{
public:
	ER20();
	~ER20();
};


}
