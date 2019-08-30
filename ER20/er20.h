#ifndef ER20_H
#define ER20_H
#define ER20CTEST	1
#include "Eigen/Dense"


//The D-H linkage parameters, the length unit: mm, the angular unit: rad
#define a1  170
#define a2  780
#define a3  140
#define d4  760
#define d6  300

//The Var
float theta[7];
float R30[4][4], R63[4][4];
typedef struct
{
	float pe[4];
	float Re[4][4];
	float roll;
	float pich;
	float yall;
}actuator;

//The func
/*************************************************************************
 * The forward kinematics transfer function,
 * return: 0 -- tranfer success, 1 -- error
 *************************************************************************/
int trans(const float theta[], actuator *pos);

/*************************************************************************
 * The inverse kinematics transfer function
 * return: 0 -- tranfer success, 1 -- error
 * **********************************************************************/
int invtrans(const actuator &pos, float theta[]);

/************************************************************************
 * The rotation matrix multiply function
 * return: the result
 ************************************************************************/

#endif

