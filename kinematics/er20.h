#ifndef ER20_H
#define ER20_H
#define ER20CTEST	0

#ifndef PI
#define PI 3.141592653589793
#endif
#include <stdint.h>		/* where int32_t is included */
/*
#ifndef int32_t
#define int32_t int
#endif
*/

///The datatype

//The D-H linkage parameters, the length unit: mm, the angular unit: rad/degree
#define ER20_a1  168.46
#define ER20_d1  504
#define ER20_a2  781.55
#define ER20_a3  140.34
#define ER20_d3  (-0.3)
#define ER20_d4  760.39
#define ER20_d6  125

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

// The incremental offset
#define INCOFF1		-59471
#define INCOFF2 	-157781
#define INCOFF3		9281
#define INCOFF4		3402
#define INCOFF5		2700548
#define INCOFF6		144115

// The bits of encoders in the momotor
#define BITS 	17


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


//The Var
#ifndef cartpos_t
typedef struct
{
	double pe[3];
	double Rx0;	//Rx0 That means rotation around the x0, uint: degree;
	double Ry0;	//Ry0 That means rotation around the y0; uint: degree;
	double Rz0;	//Rz0 That means rotation around the z0; uint: degree;
}cartpos_t;		//The cartesian coordinate system position type
#endif

#ifndef joinpos_t
typedef struct 
{
	double joi[6];
}joinpos_t;
#endif

#ifndef incpos_t
typedef struct
{
    int32_t inc[6];
}incpos_t;
#endif


//The func
/*************************************************************************
	The transferfunction between jointangle and theta
**************************************************************************/
double degree2rad(double angle);
double rad2degree(double theta);



/*************************************************************************
	The jointangle described in keba system --joinang
	The theta(rad) described in D-H  system --theta
	the difference between them are not only uinit difference(rad/degree) but
	also the offset,
*************************************************************************/
void jointangle2theta(const double joinang[6], double theta[6]);
void theta2jointangle(const double theta[6], double joinang[6]);



/************************************************************************
 * 	The transformation between incremental and jointangle
 * **********************************************************************/
joinpos_t increment2jointangle(const incpos_t & in);
incpos_t jointangle2increment(const joinpos_t & jo);




/*************************************************************************
 * The forward kinematics transfer function,
   The unit of jointangle is degree, the same as keba control,
   Thr uint of carttesian pos is mm and degree, the same as keba control,
 * return: 0 -- tranfer success, !0 -- error
   1--the 1th axis is off design
   2--the 2th axis is off design
   .....
 *************************************************************************/
int forkine(const double jointangle[6], cartpos_t *pos);



/*************************************************************************
 * The inverse kinematics transfer function
 * return: 0 -- tranfer success, 1 -- error --the axies are off design
 //theta[] is the angle of six joints,
 * **********************************************************************/
int invkine(const cartpos_t &pos, const double lastjointangle[6], double jointangle[6]);

void jointangle_show(const double jointangle[6]);

void cartpose_show(const cartpos_t &pos);

#endif

