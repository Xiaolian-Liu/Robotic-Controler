#ifndef ER20_H
#define ER20_H
#define ER20CTEST	0
#ifndef PI
#define PI 3.141592653589793
#endif

///The datatype

//The D-H linkage parameters, the length unit: mm, the angular unit: rad/degree
#define ER20_a1  168.46
#define ER20_d1  504
#define ER20_a2  781.55
#define ER20_a3  140.34
#define ER20_d3  (-0.3)
#define ER20_d4  760.39
#define ER20_d6  125

//The Var
typedef struct
{
	double pe[3];
	double Rx0;	//Rx0 That means rotation around the x0, uint: degree;
	double Ry0;	//Ry0 That means rotation around the y0; uint: degree;
	double Rz0;	//Rz0 That means rotation around the z0; uint: degree;
}cartpos_t;		//The cartesian coordinate system position type

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

