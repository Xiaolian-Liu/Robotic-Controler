#ifndef ER20_H
#define ER20_H
#define ER20CTEST	0
#define PI 3.141592653589793

///The datatype

//The D-H linkage parameters, the length unit: mm, the angular unit: rad/degree
#define ER20_a1  170
#define ER20_a2  780
#define ER20_a3  140
#define ER20_d4  760
#define ER20_d6  300

//The Var
typedef struct
{
	double pe[3];
	double Rx0;	//Rx0 That means rotation around the x0;
	double Ry0;	//Ry0 That means rotation around the y0;
	double Rz0;	//Rz0 That means rotation around the z0;
}cartpos_t;		//The cartesian coordinate system position type

//The func
/*************************************************************************
 * The forward kinematics transfer function,
 * return: 0 -- tranfer success, 1 -- error
 *************************************************************************/
int trans(const double theta[], cartpos_t *pos);

/*************************************************************************
 * The inverse kinematics transfer function
 * return: 0 -- tranfer success, 1 -- error
 //theta[] is the angle of six joints,
 * **********************************************************************/
int invtrans(const cartpos_t &pos, const double lasttheta[7], double theta[7]);

void jointspace_show(const double theta[7]);

void cartspace_show(const cartpos_t &pos);

#endif

