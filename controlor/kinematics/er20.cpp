#include "er20.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <Eigen/Dense>
#include "transform.h"

using namespace Eigen;
using namespace std;

const double a1 = ER20_a1;
const double d1 = ER20_d1;
const double a2 = ER20_a2;
const double a3 = ER20_a3;
const double d3 = ER20_d3;
const double d4 = ER20_d4;
const double d6 = ER20_d6 + ER20_pe + ER20_pen;

const double a[6] =
{
	ER20_a1,
	ER20_a2,
	ER20_a3,
	ER20_a4,
	ER20_a5,
	ER20_a6
};

const double alpha[6] =
{
	alpha1,
	alpha2,
	alpha3,
	alpha4,
	alpha5,
	alpha6
};

const double d[6] =
{
	ER20_d1,
	ER20_d2,
	ER20_d3,
	ER20_d4,
	ER20_d5,
	ER20_d6
};

const double offset[6] =
{
	offset1,
	offset2,
	offset3,
	offset4,
	offset5,
	offset6
};


const double ratio[6] = {ER20_I1, ER20_I2, ER20_I3, ER20_I4, ER20_I5, ER20_I6};
const double ratio56 = ER20_I56;

const int32_t incoff[6] = {INCOFF1, INCOFF2, INCOFF3, INCOFF4, INCOFF5, INCOFF6};


const double MAX_jointangle[6] =
{
	MAXANG1,
	MAXANG2,
	MAXANG3,
	MAXANG4,
	MAXANG5,
	MAXANG6
};
const double MIN_jointangle[6] =
{
	MINANG1,
	MINANG2,
	MINANG3,
	MINANG4,
	MINANG5,
	MINANG6
};
const double theta_offset[6] =
{
	0,
	PI / 2,
	0,
	0,
	0,
	0
};

const double Tk[6] = 
{
	0.007172397789735,
	0.009615071639379,
	0.003004258562934,
	0.001271802791642,
	0.001271802791642,
	6.409084934258972e-4
};

double degree2rad(double angle)
{
	return (angle * PI / 180);
}

double rad2degree(double theta)
{
	return (theta * 180 / PI);
}

void jointangle2theta(const double joinang[6], double theta[6])
{
	int i;
	for (i = 0; i < 6; i++)
	{
		theta[i] = degree2rad(joinang[i]) + theta_offset[i];
	}
}

void theta2jointangle(const double theta[6], double joinang[6])
{
	int i;
	for (i = 0; i < 6; i++)
	{
		joinang[i] = rad2degree(theta[i]- theta_offset[i]);
	}
}

DHsysVec Joint2DH(JointVec angle)
{
	DHsysVec theta;
	for (int i = 0; i < 6; i++)
	{
		theta[i] = degree2rad(angle(i) + theta_offset[i]);
	}
	return theta;
}

JointVec DH2Joint(DHsysVec theta)
{
	JointVec joinang;
	for (int i = 0; i < 6; i++)
	{
		joinang[i] = rad2degree(theta(i) - theta_offset[i]);
	}
	return joinang;
}

joinpos_t increment2jointangle(const incpos_t & in)
{
	joinpos_t p;
	for(int i=0; i<5; i++)
	{
		p.joi[i] = (in.inc[i] - incoff[i])*360.0/( (1<<BITS)*ratio[i] );
	}
	p.joi[5] = (in.inc[5]-incoff[5])*360.0/( (1<<BITS)*ratio[5] ) + p.joi[4]/ratio56;
	return p;
}

JointVec increment2jointangle(const int32_t *in) 
{
	JointVec p;
	for(int i=0; i<5; i++)
	{
		p[i] = (in[i] - incoff[i])*360.0/( (1<<BITS)*ratio[i] );
	}
	p[5] = (in[5]-incoff[5])*360.0/( (1<<BITS)*ratio[5] ) + p[4]/ratio56;
	// cout << "in[0]: " << in[0] << endl;
	// cout << "p[0]: " << p[0] << endl;
	return p;
}

JointVec increVel2jointVel(const int32_t *in) 
{
	JointVec p;
	for(int i=0; i<5; i++)
	{
		p[i] = in[i]*360/( (1<<BITS)*ratio[i] );
	}
	p[5] = in[5]*360/( (1<<BITS)*ratio[5] ) + p[4]/ratio56;
	return p;
}

JointVec increTor2jointTor(const int16_t *in) 
{
	JointVec T;
	double tau[6];
	for (int i = 0; i < 6; i++)
	{
		tau[i] = in[i] * Tk[i];
	}
	
	for (int i = 0; i < 5; i++)
	{
		T[i] = tau[i] * ratio[i];
	}
	T[5] = (tau[5] - tau[4] / ratio56) * ratio[5];
    return T;
}

incpos_t jointangle2increment(const joinpos_t & jo)
{
	incpos_t p;
	for(int i=0; i<5; i++)
	{
		p.inc[i] = jo.joi[i]*(1<<BITS)*ratio[i]/360 + incoff[i];
	}
	p.inc[5] = (jo.joi[5] - jo.joi[4]/ratio56)*(1<<BITS)*ratio[5]/360 + incoff[5];
	return p;
}

void jointangle2increment(int32_t *inc, const JointVec &joi) 
{
	for(int i=0; i<5; i++)
	{
		inc[i] = joi[i]*(1<<BITS)*ratio[i]/360 + incoff[i];
	}
	inc[5] = (joi[5] - joi[4]/ratio56)*(1<<BITS)*ratio[5]/360 + incoff[5];
}

JointVec Drive2Joint(DriveVec d)
{
	JointVec p;
	for (int i = 0; i < 5; i++)
	{
		p(i) = (d[i] - incoff[i]) * 360 / ((1 << BITS)*ratio[i]);
	}
	p(5) = (d[5] - incoff[5]) * 360 / ((1 << BITS)*ratio[5]) + d[4] / ratio56;
	return p;
}

DriveVec Joint2Drive(JointVec jo)
{
	DriveVec p;
	for (int i = 0; i < 5; i++)
	{
		p(i) = jo(i) * (1 << BITS)*ratio[i] / 360 + incoff[i];
	}
	p(5) = (jo(5) - jo(4) / ratio56)*(1 << BITS)*ratio[5] / 360 + incoff[5];
	return p;
}

const double MAX_theta[6] =
{
	degree2rad(MAX_jointangle[0]) + theta_offset[0],
	degree2rad(MAX_jointangle[1]) + theta_offset[1],
	degree2rad(MAX_jointangle[2]) + theta_offset[2],
	degree2rad(MAX_jointangle[3]) + theta_offset[3],
	degree2rad(MAX_jointangle[4]) + theta_offset[4],
	degree2rad(MAX_jointangle[5]) + theta_offset[5]
};
const double MIN_theta[6] =
{
	degree2rad(MIN_jointangle[0]) + theta_offset[0],
	degree2rad(MIN_jointangle[1]) + theta_offset[1],
	degree2rad(MIN_jointangle[2]) + theta_offset[2],
	degree2rad(MIN_jointangle[3]) + theta_offset[3],
	degree2rad(MIN_jointangle[4]) + theta_offset[4],
	degree2rad(MIN_jointangle[5]) + theta_offset[5]
};

const double ER20_Weig[6] = { 0.2, 0.3, 0.2, 0.15, 0.1, 0.05 };

Matrix4d A(double angle, int i)
{
	Matrix4d ret;
	double ta = degree2rad(angle)+offset[i];
	ret <<	cos(ta),	-sin(ta)*cos(alpha[i]),		sin(ta)*sin(alpha[i]),	a[i]*cos(ta),
			sin(ta),	cos(ta)*cos(alpha[i]),		-cos(ta)*sin(alpha[i]),	a[i]*sin(ta),
			0,			sin(alpha[i]),				cos(alpha[i]),			d[i],
			0,			0,							0,						1;
	return ret;
}

int forkine(const double jointangle[6], cartpos_t *pos)
{
	int i;
	for (i = 0; i < 6; i++)
	{
		if ((jointangle[i] < MIN_jointangle[i]) || (jointangle[i] > MAX_jointangle[i]))
        {
			cout << "forward kinematics err: the joint angle" << i+1 << "byound limits" << endl;
			return (i+1);
        }
	}
	double ta[7]; // the theta for D-H method, 7 numbers, 1th-6th  are useed, abandon the first
	double teta[6];
	jointangle2theta(jointangle, teta);
	for (i = 0; i < 6; i++)
		ta[i + 1] = teta[i];

	double ta23 = ta[2] + ta[3];
	Matrix3d R30, R63, R60, R70, R76;
	Vector3d P30, P63, P60;

	R30 << cos(ta[1])*cos(ta23), sin(ta[1]), cos(ta[1])*sin(ta23),
		   sin(ta[1])*cos(ta23), -cos(ta[1]), sin(ta[1])*sin(ta23),
		   sin(ta23),               0,              -cos(ta23);
	
	
	P30 <<
		cos(ta[1])*(a1+a2*cos(ta[2])+a3*cos(ta23)) + sin(ta[1])*d3,
		sin(ta[1])*(a1+a2*cos(ta[2])+a3*cos(ta23)) - cos(ta[1])*d3,
		a2*sin(ta[2]) + a3*sin(ta23) + d1;
	
	
	R63 <<
		cos(ta[4])*cos(ta[5])*cos(ta[6])-sin(ta[4])*sin(ta[6]),	-cos(ta[4])*cos(ta[5])*sin(ta[6])-sin(ta[4])*cos(ta[6]),	cos(ta[4])*sin(ta[5]),
		sin(ta[4])*cos(ta[5])*cos(ta[6])+cos(ta[4])*sin(ta[6]),	-sin(ta[4])*cos(ta[5])*sin(ta[6])+cos(ta[4])*cos(ta[6]),	sin(ta[4])*sin(ta[5]),
		-sin(ta[5])*cos(ta[6]),											sin(ta[5])*sin(ta[6]),											cos(ta[5]);
	

	P63 <<
		d6 * cos(ta[4])*sin(ta[5]),
		d6*sin(ta[4])*sin(ta[5]),
		d6*cos(ta[5]) + d4;

	R60 = R30 * R63;
	P60 = R30 * P63 + P30;

	pos->pe[0] = P60(0);
	pos->pe[1] = P60(1);
	pos->pe[2] = P60(2);
	
	R76 <<	1,  0, 0,
			0,  1, 0,
			0,  0, 1;
	R70 = R60 * R76;

	Vector3d rpy = ro2rpy(R70);
	pos->Rx0 = rpy(0);
	pos->Ry0 = rpy(1);
	pos->Rz0 = rpy(2);

#if ER20CTEST

	std::cout << R30 << std::endl;
	std::cout << P30 << std::endl;
	std::cout << R63 << std::endl;
	std::cout << P63 << std::endl;
	std::cout << R60 << std::endl;
	std::cout << P60 << std::endl;

#endif // ER20CTEST

	return 0;
}

int forkine(CartPose & pose, JointVec jointangle)
{
	int i;
	for (i = 0; i < 6; i++)
	{
		if ((jointangle[i] < MIN_jointangle[i]) || (jointangle[i] > MAX_jointangle[i]))
		{
			cout << "forward kinematics err: the joint angle" << i + 1 << "byound limits" << endl;
			return (i + 1);
		}
	}

	Matrix4d pos = Matrix4d::Identity();
	for (int i = 0; i < 6; i++)
	{
		pos = pos * A(jointangle(i), i);
	}
	Matrix3d rot = pos.block(0, 0, 3, 3);
	Vector3d pe = pos.block(0, 3, 3, 1);
	Vector3d rpy = ro2rpy(rot, 'd');
	pose.pe = pe;
	pose.rpy = rpy;

	return 0;
}

	
int invkine(const cartpos_t &pos, const double lastjointangle[6], double jointangle[6])
{
	int i, j;
	Matrix3d R70, R76, Re;
	Vector3d ae,Pw,Pe;
	double lasttheta[6];
	jointangle2theta(lastjointangle, lasttheta);
	
	Vector3d rpy;
	rpy << pos.Rx0, pos.Ry0, pos.Rz0;
	R70 = rpy2ro(rpy, 'd');

	R76 << 1, 0, 0,
		   0, 1, 0,
		   0, 0, 1;
	Re = R70 * R76.transpose();

	for (i = 0; i < 3; i++)
		Pe(i) = pos.pe[i];

	ae = Re.col(2);
	Pw = Pe - d6 * ae;
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

	static double l34 = sqrt(a3*a3 + d4*d4);
	static double a22 = a2 * a2;
	static double l342 = a3 * a3 + d4 * d4;
	static double da2l34 = 2 * a2*l34;
	static double deta34 = atan((double)d4 / a3);
	double lxy[2] = { sqrt(pwx*pwx + pwy*pwy - d3*d3), -sqrt(pwx*pwx + pwy*pwy - d3*d3) };
	double alpha[2] = { atan2(d3,lxy[0]), atan2(d3,lxy[1]) };
	double theta1[2] = { lasttheta[1], lasttheta[1] };
	if ((abs(pwx)>1e-6) || (abs(pwy)>1e-6))  // pwx, pwy != 0
	{
		for (i = 0; i < 2; i++)
			theta1[i] = atan2(pwy, pwx) + alpha[i];
	}

	double cosbeta3[2];
	for (i = 0; i < 2; i++)
		cosbeta3[i] = ( (lxy[0]-a1)*(lxy[0]-a1) + (pwz-d1)*(pwz-d1) - a22 - l342 ) / da2l34 ;

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

	
	
	double costheta2[4], sintheta2[4], theta2[4], theta23[4];
	for (i = 0; i < 4; i++)
	{
		costheta2[i] = (((lxy[i%2] - a1)*(a2 + l34*cosbeta3[i%2]) + (pwz-d1)*l34*sinbeta3[i]) / (a22 + da2l34*cosbeta3[i%2] + l342));
		sintheta2[i] = (((-lxy[i%2] + a1)*l34*sinbeta3[i] + (pwz-d1)*(a2 + l34*cosbeta3[i%2])) / (a22 + 2*a2*l34*cosbeta3[i%2] + l342));
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
		if (abs(axy[i]) < 1e-4)  //axy[i] == 0
		{
			theta4[i] = lasttheta[3];
			theta6[i] = atan2(R63[i](1, 0), R63[i](0, 0)) - theta4[i];
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
		if (abs(axy[i-4]) < 1e-4)
		{
			theta4[i] = lasttheta[3];
			theta6[i] = atan2(R63[i-4](1, 0), R63[i-4](0, 0)) - theta4[i];
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
			if ((restheta[i][j] < MIN_theta[j]) || (restheta[i][j] > MAX_theta[j]))	
				isaccept = 0;
			if(!isaccept)
				break;
		}
		if (!isaccept) continue;
		double en_i = 0;
		for (j = 0; j < 6; j++)
			en_i += ER20_Weig[j] * (restheta[i][j] - lasttheta[j])*(restheta[i][j] - lasttheta[j]);
		
		if (min_en > en_i)
		{
			min_i = i;
			min_en = en_i;
		}
	}
	if (8 == min_i)
	{
		cout << "inverse kinematics err: No such appropriate joint angle" << endl;
		return -1;
	} 
	else
	{
		theta2jointangle(restheta[min_i],jointangle);
		return 0;
	}
}		

int invkine(JointVec & jp, JointVec j0, CartPose p)
{
	int i, j;
	Matrix3d R70, R76, Re;
	Vector3d ae, Pw, Pe;
	double lasttheta[6];
	//jointangle2theta(lastjointangle, lasttheta);
	DHsysVec theta0 = Joint2DH(j0);
	for (int i = 0; i<6; i ++)
	{
		lasttheta[i] = theta0(i);
	}
	//Vector3d rpy;
	//rpy << pos.Rx0, pos.Ry0, pos.Rz0;
	//R70 = rpy2ro(rpy);
	R70 = rpy2ro(p.rpy, 'd');

	R76 << 1, 0, 0,
		0, 1, 0,
		0, 0, 1;

	Re = R70 * R76.transpose();

	/*for (i = 0; i < 3; i++)
		Pe(i) = pos.pe[i];*/
	Pe = p.pe;
	ae = Re.col(2);
	Pw = Pe - d6 * ae;
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

	static double l34 = sqrt(a3*a3 + d4 * d4);
	static double a22 = a2 * a2;
	static double l342 = a3 * a3 + d4 * d4;
	static double da2l34 = 2 * a2*l34;
	static double deta34 = atan((double)d4 / a3);
	double lxy[2] = { sqrt(pwx*pwx + pwy * pwy - d3 * d3), -sqrt(pwx*pwx + pwy * pwy - d3 * d3) };
	double alpha[2] = { atan2(d3,lxy[0]), atan2(d3,lxy[1]) };
	double theta1[2] = { lasttheta[1], lasttheta[1] };
	if ((abs(pwx) > 1e-6) || (abs(pwy) > 1e-6))  // pwx, pwy != 0
	{
		for (i = 0; i < 2; i++)
			theta1[i] = atan2(pwy, pwx) + alpha[i];
	}

	double cosbeta3[2];
	for (i = 0; i < 2; i++)
		cosbeta3[i] = ((lxy[0] - a1)*(lxy[0] - a1) + (pwz - d1)*(pwz - d1) - a22 - l342) / da2l34;

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
		beta3[i] = atan2(sinbeta3[i], cosbeta3[i % 2]);
		theta3[i] = beta3[i] + deta34;
	}



	double costheta2[4], sintheta2[4], theta2[4], theta23[4];
	for (i = 0; i < 4; i++)
	{
		costheta2[i] = (((lxy[i % 2] - a1)*(a2 + l34 * cosbeta3[i % 2]) + (pwz - d1)*l34*sinbeta3[i]) / (a22 + da2l34 * cosbeta3[i % 2] + l342));
		sintheta2[i] = (((-lxy[i % 2] + a1)*l34*sinbeta3[i] + (pwz - d1)*(a2 + l34 * cosbeta3[i % 2])) / (a22 + 2 * a2*l34*cosbeta3[i % 2] + l342));
		theta2[i] = atan2(sintheta2[i], costheta2[i]);
		theta23[i] = theta2[i] + theta3[i];
	}


	Matrix3d R30[4];
	Matrix3d R63[4];
	for (i = 0; i < 4; i++)
	{
		R30[i] << cos(theta1[i % 2])*cos(theta23[i]), sin(theta1[i % 2]), cos(theta1[i % 2])*sin(theta23[i]),
			sin(theta1[i % 2])*cos(theta23[i]), -cos(theta1[i % 2]), sin(theta1[i % 2])*sin(theta23[i]),
			sin(theta23[i]), 0, -cos(theta23[i]);

		R63[i] = (R30[i].transpose())*Re;
	}

	double theta4[8], theta5[8], theta6[8], axy[4];
	for (i = 0; i < 4; i++)
		axy[i] = sqrt((R63[i](0, 2))*(R63[i](0, 2)) + (R63[i](1, 2))*(R63[i](1, 2)));

	for (i = 0; i < 4; i++)
	{
		theta5[i] = atan2(axy[i], R63[i](2, 2));
		if (abs(axy[i]) < 1e-4)  //axy[i] == 0
		{
			theta4[i] = lasttheta[3];
			theta6[i] = atan2(R63[i](1, 0), R63[i](0, 0)) - theta4[i];
		}
		else
		{
			theta4[i] = atan2(R63[i](1, 2), R63[i](0, 2));
			theta6[i] = atan2(R63[i](2, 1), -R63[i](2, 0));
		}
	}

	for (i = 4; i < 8; i++)
	{
		theta5[i] = atan2(-axy[i - 4], R63[i - 4](2, 2));
		if (abs(axy[i - 4]) < 1e-4)
		{
			theta4[i] = lasttheta[3];
			theta6[i] = atan2(R63[i - 4](1, 0), R63[i - 4](0, 0)) - theta4[i];
		}
		else
		{
			theta4[i] = atan2(-R63[i - 4](1, 2), -R63[i - 4](0, 2));
			theta6[i] = atan2(-R63[i - 4](2, 1), R63[i - 4](2, 0));
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
	double min_en = 2 * PI * 2 * PI;
	int isaccept;
	for (i = 0; i < 8; i++)
	{
		isaccept = 1;
		for (j = 0; j < 6; j++)
		{
			if ((restheta[i][j] < MIN_theta[j]) || (restheta[i][j] > MAX_theta[j]))
				isaccept = 0;
			if (!isaccept)
				break;
		}
		if (!isaccept) continue;
		double en_i = 0;
		for (j = 0; j < 6; j++)
			en_i += ER20_Weig[j] * (restheta[i][j] - lasttheta[j])*(restheta[i][j] - lasttheta[j]);

		if (min_en > en_i)
		{
			min_i = i;
			min_en = en_i;
		}
	}
	if (8 == min_i)
	{
		cout << "inverse kinematics err: No such appropriate joint angle" << endl;
		return -1;
	}
	else
	{
		double jointangle[6];
		theta2jointangle(restheta[min_i], jointangle);
		jp << jointangle[0], jointangle[1], jointangle[2], jointangle[3], jointangle[4], jointangle[5];
		return 0;
	}
}

int invkine(JointVec & jp, JointVec j0, Matrix4d T)
{
	int i, j;
	Matrix3d R70, R76, Re;
	Vector3d ae, Pw, Pe;
	double lasttheta[6];
	//jointangle2theta(lastjointangle, lasttheta);
	DHsysVec theta0 = Joint2DH(j0);
	for (int i = 0; i < 6; i++)
	{
		lasttheta[i] = theta0(i);
	}
	
	R70 = T.block(0,0,3,3);

	R76 << 1, 0, 0,
		0, 1, 0,
		0, 0, 1;

	Re = R70 * R76.transpose();

	Pe = transl(T);
	ae = Re.col(2);
	Pw = Pe - d6 * ae;
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

	static double l34 = sqrt(a3*a3 + d4 * d4);
	static double a22 = a2 * a2;
	static double l342 = a3 * a3 + d4 * d4;
	static double da2l34 = 2 * a2*l34;
	static double deta34 = atan((double)d4 / a3);
	double lxy[2] = { sqrt(pwx*pwx + pwy * pwy - d3 * d3), -sqrt(pwx*pwx + pwy * pwy - d3 * d3) };
	double alpha[2] = { atan2(d3,lxy[0]), atan2(d3,lxy[1]) };
	double theta1[2] = { lasttheta[1], lasttheta[1] };
	if ((abs(pwx) > 1e-6) || (abs(pwy) > 1e-6))  // pwx, pwy != 0
	{
		for (i = 0; i < 2; i++)
			theta1[i] = atan2(pwy, pwx) + alpha[i];
	}

	double cosbeta3[2];
	for (i = 0; i < 2; i++)
		cosbeta3[i] = ((lxy[0] - a1)*(lxy[0] - a1) + (pwz - d1)*(pwz - d1) - a22 - l342) / da2l34;

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
		beta3[i] = atan2(sinbeta3[i], cosbeta3[i % 2]);
		theta3[i] = beta3[i] + deta34;
	}



	double costheta2[4], sintheta2[4], theta2[4], theta23[4];
	for (i = 0; i < 4; i++)
	{
		costheta2[i] = (((lxy[i % 2] - a1)*(a2 + l34 * cosbeta3[i % 2]) + (pwz - d1)*l34*sinbeta3[i]) / (a22 + da2l34 * cosbeta3[i % 2] + l342));
		sintheta2[i] = (((-lxy[i % 2] + a1)*l34*sinbeta3[i] + (pwz - d1)*(a2 + l34 * cosbeta3[i % 2])) / (a22 + 2 * a2*l34*cosbeta3[i % 2] + l342));
		theta2[i] = atan2(sintheta2[i], costheta2[i]);
		theta23[i] = theta2[i] + theta3[i];
	}


	Matrix3d R30[4];
	Matrix3d R63[4];
	for (i = 0; i < 4; i++)
	{
		R30[i] << cos(theta1[i % 2])*cos(theta23[i]), sin(theta1[i % 2]), cos(theta1[i % 2])*sin(theta23[i]),
			sin(theta1[i % 2])*cos(theta23[i]), -cos(theta1[i % 2]), sin(theta1[i % 2])*sin(theta23[i]),
			sin(theta23[i]), 0, -cos(theta23[i]);

		R63[i] = (R30[i].transpose())*Re;
	}

	double theta4[8], theta5[8], theta6[8], axy[4];
	for (i = 0; i < 4; i++)
		axy[i] = sqrt((R63[i](0, 2))*(R63[i](0, 2)) + (R63[i](1, 2))*(R63[i](1, 2)));

	for (i = 0; i < 4; i++)
	{
		theta5[i] = atan2(axy[i], R63[i](2, 2));
		if (abs(axy[i]) < 1e-4)  //axy[i] == 0
		{
			theta4[i] = lasttheta[3];
			theta6[i] = atan2(R63[i](1, 0), R63[i](0, 0)) - theta4[i];
		}
		else
		{
			theta4[i] = atan2(R63[i](1, 2), R63[i](0, 2));
			theta6[i] = atan2(R63[i](2, 1), -R63[i](2, 0));
		}
	}

	for (i = 4; i < 8; i++)
	{
		theta5[i] = atan2(-axy[i - 4], R63[i - 4](2, 2));
		if (abs(axy[i - 4]) < 1e-4)
		{
			theta4[i] = lasttheta[3];
			theta6[i] = atan2(R63[i - 4](1, 0), R63[i - 4](0, 0)) - theta4[i];
		}
		else
		{
			theta4[i] = atan2(-R63[i - 4](1, 2), -R63[i - 4](0, 2));
			theta6[i] = atan2(-R63[i - 4](2, 1), R63[i - 4](2, 0));
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
	double min_en = 2 * PI * 2 * PI;
	int isaccept;
	for (i = 0; i < 8; i++)
	{
		isaccept = 1;
		for (j = 0; j < 6; j++)
		{
			if ((restheta[i][j] < MIN_theta[j]) || (restheta[i][j] > MAX_theta[j]))
				isaccept = 0;
			if (!isaccept)
				break;
		}
		if (!isaccept) continue;
		double en_i = 0;
		for (j = 0; j < 6; j++)
			en_i += ER20_Weig[j] * (restheta[i][j] - lasttheta[j])*(restheta[i][j] - lasttheta[j]);

		if (min_en > en_i)
		{
			min_i = i;
			min_en = en_i;
		}
	}
	if (8 == min_i)
	{
		cout << "inverse kinematics err: No such appropriate joint angle" << endl;
		return -1;
	}
	else
	{
		double jointangle[6];
		theta2jointangle(restheta[min_i], jointangle);
		jp << jointangle[0], jointangle[1], jointangle[2], jointangle[3], jointangle[4], jointangle[5];
		return 0;
	}
}

void printJointVec(const JointVec &J) 
{
	for (int i = 0; i < 6; i++)
	{
        cout << right << setw(15) << J[i] << " ";
	}
	cout << endl;
}

