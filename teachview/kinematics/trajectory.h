#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <vector>
#include <queue>
#include <Eigen/Dense>


#ifndef PI
#define PI 3.1415926545
#endif

using namespace Eigen;
using std::queue;
using std::vector;

typedef std::vector<double> vectord;    //The STL of C++



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



#endif