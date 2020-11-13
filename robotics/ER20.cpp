#include "ER20.h"
#include <iostream>

namespace RKD {


	ER20::ER20(bool modified) :SerialLink("ER20C-C10-Modified","EFFORT-MARILAB", modified)
	{
		if (modified)
		{
			vector<Link> links(6);
			for (int i = 0; i < links.size(); i++)
			{
				links[i] = { 0, d[i], a[i], alpha[i], offset[i], modified, true };
			}
			SerialLink::setLinks(links);
			Matrix4d tool;
			tool.setIdentity();
			tool.col(3) << 0, 0, ER20_pe + ER20_pen, 1;
		}
		else
		{

		}
	}

	Matrix4d ER20::fkinef(JointVec q)
	{
		double ta[7]; // the theta for D-H method, 7 numbers, 1th-6th  are useed, abandon the first
		//double teta[6];
		JointVec teta;
		teta = q + offset;
		
		for (int i = 0; i < 6; i++)
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
	
		R76 <<	1,  0, 0,
				0,  1, 0,
				0,  0, 1;
		R70 = R60 * R76;

		return transl(P60, R70);
	
	}

	int ER20::ikine(JointVec & q, JointVec q0, Matrix4d T)
	{
		Matrix3d R70, R76, Re;
		Vector3d ae, Pw, Pe;
		//double lasttheta[6];
		//jointangle2theta(lastjointangle, lasttheta);

		JointVec  lastTheta;
		lastTheta = q0 + offset;
	
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


		static double l34 = sqrt(a3*a3 + d4 * d4);
		static double a22 = a2 * a2;
		static double l342 = a3 * a3 + d4 * d4;
		static double da2l34 = 2 * a2*l34;
		static double deta34 = atan((double)d4 / a3);
		
		double candidateLxy[2] = 
		{ 
			sqrt(pwx*pwx + pwy * pwy - d3 * d3), 
			-sqrt(pwx*pwx + pwy * pwy - d3 * d3) 
		};
		double candidateAlpha[2] = 
		{ 
			atan2(d3,candidateLxy[0]), 
			atan2(d3,candidateLxy[1]) 
		};
		double candidateTheta1[2] = 
		{ 
			atan2(pwy, pwx) + candidateAlpha[0],
			atan2(pwy, pwx) + candidateAlpha[1]
		};

		double theta1 = candidateTheta1[0];
		double lxy = candidateLxy[0];
		double alpha = candidateAlpha[0];
		if (abs(candidateTheta1[1] - lastTheta[0]) < abs(theta1- lastTheta[0]))
		{
			theta1 = candidateTheta1[1];
			lxy = candidateLxy[1];
			alpha = candidateAlpha[1];
		}

		double cosbeta3 = ((lxy - a1)*(lxy - a1) + (pwz - d1)*(pwz - d1) - a22 - l342) / da2l34;
		double candidatesSinbeta3[2] =
		{
			sqrt(1 - cosbeta3 * cosbeta3),
			-sqrt(1 - cosbeta3 * cosbeta3)
		};
		double candidateBeta3[2] =
		{
			atan2(candidatesSinbeta3[0], cosbeta3),
			atan2(candidatesSinbeta3[1], cosbeta3)
		};
		double candidateTheta3[2] =
		{
			candidateBeta3[0] + deta34,
			candidateBeta3[1] + deta34
		};
		double theta3 = candidateTheta3[0];
		double sinBeta3 = candidatesSinbeta3[0];
		if (abs(candidateTheta3[1] - lastTheta[2]) < abs(theta3 - lastTheta[2]))
		{
			theta3 = candidateTheta3[1];
			sinBeta3 = candidatesSinbeta3[1];
		}

		double costheta2, sintheta2, theta2, theta23;
		costheta2 = (((lxy - a1)*(a2 + l34 * cosbeta3) + (pwz - d1)*l34*sinBeta3) / (a22 + da2l34 * cosbeta3 + l342));
		sintheta2 = (((-lxy + a1)*l34*sinBeta3 + (pwz - d1)*(a2 + l34 * cosbeta3)) / (a22 + 2 * a2*l34*cosbeta3 + l342));
		theta2 = atan2(sintheta2, costheta2);
		theta23 = theta2 + theta3;

		Matrix3d R30;
		Matrix3d R63;
		R30 <<	cos(theta1)*cos(theta23),			sin(theta1),			cos(theta1)*sin(theta23),
				sin(theta1)*cos(theta23),			-cos(theta1),			sin(theta1)*sin(theta23),
				sin(theta23),						0,						-cos(theta23);
		R63 = (R30.transpose())*Re;

		double axy = sqrt((R63(0, 2))*(R63(0, 2)) + (R63(1, 2))*(R63(1, 2)));
		double candidateTheta5[2] =
		{
			atan2(axy, R63(2, 2)),
			atan2(-axy, R63(2, 2))
		};
		double theta4 = lastTheta[3];
		double theta6 = atan2(R63(1, 0), R63(0, 0)) - theta4;
		double theta5 = candidateTheta5[0];
		if (abs(candidateTheta5[1] - lastTheta[4]) < abs(theta5 - lastTheta[4]))
		{
			theta5 = candidateTheta5[1];
		}

		double candidateTheta4[2] =
		{
			atan2(R63(1, 2), R63(0, 2)),
			atan2(-R63(1, 2), -R63(0, 2))
		};
		double candidateTheta6[2] =
		{
			atan2(R63(2, 1), -R63(2, 0)),
			atan2(-R63(2, 1), R63(2, 0))
		};

		if (abs(axy > 1e-4))  // axy != 0;
		{
			theta4 = candidateTheta4[0];
			theta6 = candidateTheta6[0];
			if (abs(candidateTheta4[1] - lastTheta[3]) < abs(theta4 - lastTheta[3]))
			{
				theta4 = candidateTheta4[1];
				theta6 = candidateTheta6[1];
			}
		}

		JointVec theta;
		theta << theta1, theta2, theta3, theta4, theta5, theta6;
		q = theta - offset;

		for (int i = 0; i < 6; i++)
		{
			if ((q[i] > links[i].qlim[1]) || (q[i] < links[i].qlim[0]))
			{
				std::cout << "inverse kinematics err: No such appropriate joint angle" << std::endl;
				return -1;
			}
		}

		return 0;

	}


ER20::~ER20()
{
}


}
