#include "er20.h"
#include <cmath>
#include <iostream>
#include "Eigen/Dense"

using namespace Eigen;
using namespace std;

int trans(const float theta[], actuator *pos)
{
	float theta23 = theta[2] + theta[3];
	Matrix3f R30, R63;
	Vector3f P30, P63;

	R30 << cos(theta[1])*cos(theta23), sin(theta[1]), cos(theta[1])*sin(theta23),
		sin(theta[1])*cos(theta23), -cos(theta[1]), sin(theta[1])*sin(theta23),
		sin(theta23), 0, -cos(theta23);
	
	
	P30 <<
		cos(theta[1])*(a1 + a2 * cos(theta[2]) + a3 * cos(theta23)),
		sin(theta[1])*(a1 + a2 * cos(theta[2]) + a3 * cos(theta23)),
		a2*sin(theta[2]) + a3 * sin(theta23);
	
	
	R63 <<
		cos(theta[4])*cos(theta[5])*cos(theta[6])-sin(theta[4])*sin(theta[6]),	-cos(theta[4])*cos(theta[5])*sin(theta[6])-sin(theta[4])*cos(theta[6]),	cos(theta[4])*sin(theta[5]),
		sin(theta[4])*cos(theta[5])*cos(theta[6])+cos(theta[4])*sin(theta[6]),	-sin(theta[4])*cos(theta[5])*sin(theta[6])+cos(theta[4])*cos(theta[6]),	sin(theta[4])*sin(theta[5]),
		-sin(theta[5])*cos(theta[6]),											sin(theta[5])*sin(theta[6]),											cos(theta[5]);
	

	P63 <<
		d6 * cos(theta[4])*sin(theta[5]),
		d6*sin(theta[4])*sin(theta[5]),
		d6*cos(theta[5]) + d4;

#if ER20CTEST

	cout << R30 << endl;
	cout << P30 << endl;
	cout << R63 << endl;
	cout << P63 << endl;

#endif // ER20CTEST

};
	return 0;



}