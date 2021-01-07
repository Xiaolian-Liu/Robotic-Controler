#ifndef ER20_H
#define ER20_H
#define ER20CTEST	0

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

// The incremental offset
#define INCOFF1		79744
#define INCOFF2 	1182
#define INCOFF3		67872
#define INCOFF4		47616
#define INCOFF5		11973
#define INCOFF6		148827

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

//the max cart speed mm/s
#define LMAXVEL     1000
//the max cart acceleration mm/s^2
#define LMAXACC     2000
//the max rotation speed in cart space, uinits: degree/s
#define RMAXVEL		180
//the max rotation acceleration in cart space, uinits: degree/s^2
#define RMAXACC		540

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

typedef struct
{
	Vector3d pe;
	Vector3d rpy;
}CartPose;
typedef vector<CartPose> seqCartPose;

#ifndef joinpos_t
typedef struct 
{
	double joi[6];
}joinpos_t;
#endif

typedef Matrix<double, 6, 1> JointVec;
//EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(JointVec)
typedef vector<JointVec> seqJointVec; /* the sequence of JointVec*/


#ifndef incpos_t
typedef struct
{
    int32_t inc[6];
}incpos_t;
#endif

typedef Matrix<int32_t, 6, 1> DriveVec;
typedef Matrix<double, 6, 1> DHsysVec;

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

DHsysVec Joint2DH(JointVec);
JointVec DH2Joint(DHsysVec);

/************************************************************************
 * 	The transformation between incremental and jointangle
 * **********************************************************************/
joinpos_t increment2jointangle(const incpos_t & in);
JointVec increment2jointangle(const int32_t *in);
JointVec increVel2jointVel(const int32_t *in);
JointVec increTor2jointTor(const int16_t *in);
incpos_t jointangle2increment(const joinpos_t &jo);
void jointangle2increment(int32_t *inc, const JointVec &jo);

JointVec Drive2Joint(DriveVec);
DriveVec Joint2Drive(JointVec);

/*************************************************************************
 * the Homogeneous transformation matrix constructor,
 * arguments: angle is the rotation angle of the manipulator, units: degree
 *			  index is the No of A, range from 0 to 5
 * returns:  the 4X4 HT matrix
 *************************************************************************/
Matrix4d A(double angle, int index);

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
int forkine(CartPose & pose, JointVec angles);


/*************************************************************************
 * The inverse kinematics transfer function
 * return: 0 -- tranfer success, 1 -- error --the axies are off design
 //theta[] is the angle of six joints,
 * **********************************************************************/
int invkine(const cartpos_t &pos, const double lastjointangle[6], double jointangle[6]);
int invkine(JointVec & angles, JointVec lastangles, CartPose pose);
int invkine(JointVec & angles, JointVec lastangles, Matrix4d Transform);
//seqJointVec invkine(JointVec j0, CartPose pf);

void jointangle_show(const double jointangle[6]);

void cartpose_show(const cartpos_t &pos);

#endif

