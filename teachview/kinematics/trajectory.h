#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <vector>
#include <queue>
#include <Eigen/Dense>
#include "er20.h"
#include "circle.h"


#ifndef PI
#define PI 3.1415926545
#endif

using namespace Eigen;
using std::queue;
using std::vector;

typedef std::vector<double> vectord;    //The STL of C++

typedef vector<cartpos_t> vectorpos;
typedef vector<joinpos_t> vectorangle;
typedef vector<Vector3d> vecposition;


/*****************************************************************
	Unit Linear interpolation with parabolic transitions in joint space
	a--the acceleration
	v--the velocity
	x0--the start position
	xf--the final position
	f--the frequency
	return vector -- the uint sequency
*****************************************************************/
vectord julipt(double x0, double xf, double a, double v, int f = 500);

/*****************************************************************
	Linear interpolation with parabolic transitions in joint space
	a--the acceleration
	v--the velocity
	x0--the start position
	xf--the final position
	f--the frequency
	return vector -- the position sequency
*****************************************************************/
vectord jlipt(double x0, double xf, double a, double v, int f = 500);

// unit Linear segment with parabolic blend
int ulspb(vectord & s, vectord & sd, vectord & sdd, double s0, double sf, double a, double v , int f = 500);


int lintraj(vectorpos & p, cartpos_t p0, cartpos_t pf , int a, int v, int f = 500);

int lintraj(vectorangle & j, joinpos_t j0, cartpos_t pf , int a, int v, int f = 500);

/* the circle trajectory function 
 arguments: p0, pi, pf, 3 points determine a circle; a, v, f, the motion parameters 
 returns: vecposition p --- the position sequence of the manipulator end */

int cirtraj(vecposition & p, point p0, point pi, point pf, int a, int v, int f = 500);

#endif
