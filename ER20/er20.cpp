#include "er20.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include "Eigen/Dense"

using namespace Eigen;

const double MAX_Theta[6] =
{
	90 * PI / 180,
	154 * PI / 180,
	165 * PI / 180,
	178 * PI / 180,
	132 * PI / 180,
	180 * PI / 180
};
const double MIN_Theta[6] =
{
	-90 * PI / 180,
	-52 * PI / 180,
	-73 * PI / 180,
	-178 * PI / 180,
	-132 * PI / 180,
	-180 * PI / 180
};
const double ER20_Weig[6] = { 0.2, 0.3, 0.2, 0.15, 0.1, 0.5 };
int trans(const double theta[], cartpos_t *pos)
{
	for (int i = 1; i < 6; i++)
		if ((theta[i+1] < MIN_Theta[i]) || (theta[i+1] > MAX_Theta[i]))
			return -1;
	
	double theta23 = theta[2] + theta[3];
	Matrix3d R30, R63, R60, R70, R76;
	Vector3d P30, P63, P60;

	R30 << cos(theta[1])*cos(theta23), sin(theta[1]), cos(theta[1])*sin(theta23),
		   sin(theta[1])*cos(theta23), -cos(theta[1]), sin(theta[1])*sin(theta23),
		   sin(theta23),               0,              -cos(theta23);
	
	
	P30 <<
		cos(theta[1])*(ER20_a1 +ER20_a2 * cos(theta[2]) + ER20_a3 * cos(theta23)),
		sin(theta[1])*(ER20_a1 + ER20_a2 * cos(theta[2]) + ER20_a3 * cos(theta23)),
		ER20_a2*sin(theta[2]) + ER20_a3 * sin(theta23);
	
	
	R63 <<
		cos(theta[4])*cos(theta[5])*cos(theta[6])-sin(theta[4])*sin(theta[6]),	-cos(theta[4])*cos(theta[5])*sin(theta[6])-sin(theta[4])*cos(theta[6]),	cos(theta[4])*sin(theta[5]),
		sin(theta[4])*cos(theta[5])*cos(theta[6])+cos(theta[4])*sin(theta[6]),	-sin(theta[4])*cos(theta[5])*sin(theta[6])+cos(theta[4])*cos(theta[6]),	sin(theta[4])*sin(theta[5]),
		-sin(theta[5])*cos(theta[6]),											sin(theta[5])*sin(theta[6]),											cos(theta[5]);
	

	P63 <<
		ER20_d6 * cos(theta[4])*sin(theta[5]),
		ER20_d6*sin(theta[4])*sin(theta[5]),
		ER20_d6*cos(theta[5]) + ER20_d4;

	R60 = R30 * R63;
	P60 = R30 * P63 + P30;

	pos->pe[0] = P60(0);
	pos->pe[1] = P60(1);
	pos->pe[2] = P60(2);
	
	R76 <<	0,  0, 1,
			0, -1, 0,
			1,  0, 0;
	R70 = R60 * R76;

	double r31 = R70(2, 0);
	double r11 = R70(0, 0);
	double r21 = R70(1, 0);
	double r32 = R70(2, 1);
	double r33 = R70(2, 2);
	double r12 = R70(0, 1);
	double r22 = R70(1, 1);

	double cosRy0 = sqrt(r11*r11 + r21*r21);
	if (0 == cosRy0)
	{
		pos->Ry0 = PI / 2;
		pos->Rz0 = 0;
		pos->Rx0 = atan2(r12, r22);
	}
	else
	{
		pos->Ry0 = atan2(-r31, cosRy0);
		pos->Rz0 = atan2(r21, r11);
		pos->Rx0 = atan2(r32, r33);
	}

#if ER20CTEST

	std::cout << R30 << std::endl;
	std::cout << P30 << std::endl;
	std::cout << R63 << std::endl;
	std::cout << P63 << std::endl;
	std::cout << R60 << std::endl;
	std::cout << P60 << std::endl;

#endif // ER20CTEST

	return 0;
};
	
int invtrans(const cartpos_t &pos, const double lasttheta[7], double theta[7])
{
	int i, j;
	Matrix3d R70, R76, Re;
	Vector3d ae,Pw,Pe;
	double a = pos.Rz0, B = pos.Ry0, y = pos.Rx0;
	R70 << cos(a)*cos(B), cos(a)*sin(B)*sin(y) - sin(a)*cos(y), cos(a)*sin(B)*cos(y) + sin(a)*sin(y),
		  sin(a)*cos(B), sin(a)*sin(B)*sin(y) + cos(a)*cos(y), sin(a)*sin(B)*cos(y) - cos(a)*sin(y),
		  -sin(B),       cos(B)*sin(y),                        cos(B)*cos(y);
	
	R76 << 0, 0, 1,
		   0, -1, 0,
		   1, 0, 0;
	Re = R70 * R76.transpose();

	for (i = 0; i < 3; i++)
		Pe(i) = pos.pe[i];

	ae = Re.col(2);
	Pw = Pe - ER20_d6 * ae;
	double pwx = Pw(0);
	double pwy = Pw(1);
	double pwz = Pw(2);

#if ER20CTEST
	std::cout << R70 << std::endl;
	std::cout << Re << std::endl;
	std::cout << Pe << std::endl;
	std::cout << ae << std::endl;
	std::cout << Pw << std::endl;
#endif

	static double l34 = sqrtf(ER20_a3*ER20_a3 + ER20_d4*ER20_d4);
	static double l234 = ER20_a2* ER20_a2 + ER20_a3* ER20_a3 + ER20_d4* ER20_d4;
	static double deta34 = atan((float)ER20_d4 / ER20_a3);
	static double a2l34 = 2 * ER20_a2 * l34;
	double lxy[2] = { sqrt(pwx*pwx + pwy * pwy), -sqrt(pwx*pwx + pwy * pwy) };
	double lxy0[2] = { lxy[0] - ER20_a1, lxy[1] - ER20_a1 };

	double theta1[2] = { lasttheta[1], lasttheta[1] };
	if ((pwx != 0) || (pwy != 0))
	{
		theta1[0] = atan2(pwy, pwx);
		theta1[1] = atan2(-pwy, -pwx);
	}

	double cosbeta3[2] =
	{
		((lxy0[0] * lxy0[0] + pwz * pwz - l234) / a2l34) ,
		((lxy0[1] * lxy0[1] + pwz * pwz - l234) / a2l34)
	};
	double sinbeta3[4] =
	{ 
		sqrt(1 - cosbeta3[0] * cosbeta3[0]), 
		sqrt(1 - cosbeta3[1] * cosbeta3[1]),
		-sqrt(1 - cosbeta3[0] * cosbeta3[0]),
		-sqrt(1 - cosbeta3[1] * cosbeta3[1])
	};
	double beta3[4], theta3[4];
/*	beta3[0] = atan2(sinbeta3[0], cosbeta3[0]);
	beta3[1] = atan2(sinbeta3[1], cosbeta3[1]);
	beta3[2] = atan2(sinbeta3[2], cosbeta3[0]);
	beta3[3] = atan2(sinbeta3[3], cosbeta3[1]);
*/	for (i = 0; i < 4; i++)
	{
		beta3[i] = atan2(sinbeta3[i], cosbeta3[i%2]);
		theta3[i] = beta3[i] + deta34;
	}

	static double a22 = ER20_a2 * ER20_a2;
	static double l342 = ER20_a3 * ER20_a3 + ER20_d4 * ER20_d4;
	double costheta2[4], sintheta2[4], theta2[4], theta23[4];
	for (i = 0; i < 4; i++)
	{
		costheta2[i] = (((lxy[i%2] - ER20_a1)*(ER20_a2 + l34*cosbeta3[i%2]) + pwz*l34*sinbeta3[i]) / (a22 + a2l34*cosbeta3[i%2] + l342));
		sintheta2[i] = (((-lxy[i%2] + ER20_a1)*l34*sinbeta3[i] + pwz*(ER20_a2 + l34*cosbeta3[i%2])) / (a22 + a2l34*cosbeta3[i%2] + l342));
		theta2[i] = atan2(sintheta2[i], costheta2[i]);
		theta23[i] = theta2[i] + theta3[i];
	}


	Matrix3d R30[4];
	Matrix3d R63[4];
	for (i = 0; i < 4; i++)
	{
		R30[i] << cos(theta1[i%2])*cos(theta23[i]), sin(theta1[i%2]), cos(theta1[i%2])*sin(theta23[i]),
				  sin(theta1[i%2])*cos(theta23[i]), -cos(theta1[i%2]), sin(theta1[i%2])*sin(theta23[i]),
			      sin(theta23[i]),					0,					-cos(theta23[i]);

		R63[i] = (R30[i].transpose())*Re;
	}

	double theta4[8], theta5[8], theta6[8], axy[4];
	for (i = 0; i < 4; i++)
		axy[i] = sqrt((R63[i](0, 2))*(R63[i](0, 2)) + (R63[i](1, 2))*(R63[i](1, 2)));
	
	for (i = 0; i < 4; i++)
	{
		theta5[i] = atan2(axy[i], R63[i](2, 2));
		if (0 == axy[i])
		{
			theta4[i] = 0;
			theta6[i] = atan2(-R63[i](0, 1), R63[i](0, 0));
		}
		else
		{
			theta4[i] = atan2(R63[i](1, 2), R63[i](0, 2));
			theta6[i] = atan2(R63[i](2, 1), -R63[i](2, 0));
		}
	}
		
	for (i = 4; i < 8; i++)
	{
		theta5[i] = atan2(-axy[i-4], R63[i-4](2, 2));
		if (0 == axy[i-4])
		{
			theta4[i] = 0;
			theta6[i] = atan2(-R63[i-4](0, 1), R63[i-4](0, 0));
		}
		else
		{
			theta4[i] = atan2(-R63[i-4](1, 2), -R63[i-4](0, 2));
			theta6[i] = atan2(-R63[i-4](2, 1), R63[i-4](2, 0));
		}
	}

	double *res[8];
	double restheta[8][6];
	for (i = 0; i < 8; i++)
	{
		res[i] = new double[6]{ theta1[i % 2],theta2[i % 4],theta3[i % 4],theta4[i],theta5[i],theta6[i] };
		for (j = 0; j < 6; j++)
			restheta[i][j] = res[i][j];
		delete[] res[i];
	}
		
	int min_i = 8;
	double min_en = 2*PI*2*PI;
	int isaccept;
	for (i = 0; i < 8; i++)
	{
		isaccept = 1;
		for (j = 0; j < 6; j++)
		{
			if ((restheta[i][j] < MIN_Theta[j]) || (restheta[i][j] > MAX_Theta[j]))	
				isaccept = 0;
			if(!isaccept)
				break;
		}
		if (!isaccept) continue;
		double en_i = 0;
		for (j = 0; j < 6; j++)
			en_i += ER20_Weig[j] * (restheta[i][j] - lasttheta[j+1])*(restheta[i][j] - lasttheta[j+1]);
		
		if (min_en > en_i)
		{
			min_i = i;
			min_en = en_i;
		}
	}
	if (8 == min_i) return -1;
	else
	{
		for (int j = 0; j < 6; j++)
		{
			theta[j + 1] = restheta[min_i][j];
		}
		return 0;
	}
}				

void jointspace_show(const double theta[7])
{
	std::cout << " The joints values are: ";
	for (int i = 1; i < 7; i++)
	{
		std::cout.width(15);
		std::cout << theta[i] << '\t';
	}
		
	std::cout << "\n";
}

void cartspace_show(const cartpos_t &pos)
{
	std::cout << " The Cartesia vals are: ";
	for (int i = 0; i < 3; i++)
	{
		std::cout.width(15);
		std::cout << pos.pe[i] << '\t';
	}
	
	std::cout.width(15);
	std::cout << pos.Rx0 << '\t';
	std::cout.width(15);
	std::cout << pos.Ry0 << '\t';
	std::cout.width(15);
	std::cout << pos.Rz0 << '\t';
	std::cout << std::endl;
}


