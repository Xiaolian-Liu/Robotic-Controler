#include "trajectory.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include "circle.h"
#include "transform.h"

vectord julipt(double x0, double xf, double a, double v, int f)
{
	double xl = abs(xf - x0) - v * v / a;  //the linear segment
	if (xl > 0)
	{
		double t1 = v / a;
		double t2 = t1 + xl / v;
		double tf = t2 + v / a;
		double T = 1.0 / f;
		unsigned long N = tf * f;
		vectord x(N);
        for (unsigned long i = 0; i < N; i++)
		{
			double t = i * T;
			if (t < t1)
			{
				x[i] = (a*t*t / 2)/abs(xf-x0);
			}
            else if ((t >= t1) && (t < t2))
			{
				x[i] = (v*v / a / 2 + v * (t - t1))/abs(xf-x0);
			}
			else
			{
				x[i] = (abs(xf-x0) - a*(tf - t)*(tf - t)/2)/abs(xf -x0);
			}
		}
		return x;
	}
	else
	{
		double t1 = sqrt(abs(xf - x0) / a);
		double tf = 2 * t1;
		double T = 1.0 / f;
		unsigned long N = tf * f;
		vectord x(N);
        for (unsigned long i = 0; i < N; i++)
		{
			double t = i * T;
			if (t < t1)
			{
				x[i] = (a*t*t / 2)/abs(xf - x0);
			}
			else
			{
				x[i] = (abs(xf-x0) - a*(tf - t)*(tf - t)/2)/abs(xf-x0);
			}
		}
		return x;
	}
}

vectord jlipt(double x0, double xf, double a, double v, int f)
{
	int sgn = 0;
	if (xf > x0)
		sgn = 1;
	if (xf < x0)
		sgn = -1;

	double xl = abs(xf - x0) - v * v / a;  //the linear segment
	if (xl > 0)
	{
		double t1 = v / a;
		double t2 = t1 + xl / v;
		double tf = t2 + v / a;
		double T = 1.0 / f;
		unsigned long N = tf * f;
		vectord x(N);
        x[0] = 150;
        for (unsigned long i = 0; i < N; i++)
		{
			double t = i * T;
			if (t < t1)
			{
                x[i] = x0 + sgn*(a*t*t / 2);
			}
            else if ((t >= t1) && (t < t2))
			{
				x[i] = x0 + sgn * (v*v / a / 2 + v * (t - t1));
			}
			else
			{
				x[i] = xf - sgn * (a*(tf - t)*(tf - t) / 2);
			}
		}
		return x;
	}
	else
	{
		double t1 = sqrt(abs(xf - x0) / a);
		double tf = 2 * t1;
		double T = 1.0 / f;
		unsigned long N = tf * f;
		vectord x(N);
        for (unsigned long i = 0; i < N; i++)
		{
			double t = i * T;
			if (t < t1)
			{
				x[i] = x0 + sgn * (a*t*t / 2);
			}
			else
			{
				x[i] = xf - sgn * (a*(tf - t)*(tf - t) / 2);
			}
		}
		return x;
	}
}


// unit Linear segment with parabolic blend
int ulspb(vectord & s, vectord & sd, vectord & sdd, double s0, double sf, double a, double v , int f)
{
    double s1 = abs(sf - s0) - v * v / a;  //the linear segment
    if (s1 > 0)
    {
        double t1 = v / a;
        double t2 = t1 + s1 / v;
        double tf = t2 + v / a;
        double T = 1.0 / f;
        unsigned long N = tf * f;
        s.resize(N);
        sd.resize(N);
		sdd.resize(N);
        for (unsigned long i = 0; i < N; i++)
        {
            double t = i * T;
            if (t < t1)
            {
                s[i]  	= (a*t*t / 2)/abs(sf-s0);
				sd[i] 	= a*t/abs(sf-s0);
				sdd[i] 	= a/abs(sf-s0);
            }
            else if ((t >= t1) && (t < t2))
            {
                s[i] 	= (v*v / a / 2 + v * (t - t1))/abs(sf-s0);
				sd[i] 	= v/abs(sf-s0);
				sdd[i]	= 0;
            }
            else
            {
                s[i] 	= (abs(sf-s0) - a*(tf - t)*(tf - t)/2)/abs(sf -s0);
				sd[i] 	= a*(tf-t)/abs(sf-s0);
				sdd[i] 	= -a/abs(sf-s0);
            }
        }
        return 0;
    }
    else
    {
        double t1 = sqrt(abs(sf - s0) / a);
        double tf = 2 * t1;
        double T = 1.0 / f;
        unsigned long N = tf * f;
        s.resize(N);
        sd.resize(N);
		sdd.resize(N);
        for (unsigned long i = 0; i < N; i++)
        {
            double t = i * T;
            if (t < t1)
            {
                s[i] 	= (a*t*t / 2)/abs(sf - s0);
				sd[i]	= a*t/abs(sf-s0);
				sdd[i]	= a/abs(sf-s0);
            }
            else
            {
                s[i] 	= (abs(sf-s0) - a*(tf - t)*(tf - t)/2)/abs(sf-s0);
				sd[i]	= a*(tf-t)/abs(sf-s0);
            }
        }
        return 0;
    }
}

int lintraj(vectorpos & p, cartpos_t p0, cartpos_t pf , int a, int v, int f)
{
    double acc = a*LMAXACC/100.0;
    double vel = v*LMAXVEL/100.0;
    double Sf = 0;
    for(int i=0; i<3; i++)
    {
        Sf += (pf.pe[i]-p0.pe[i])*(pf.pe[i]-p0.pe[i]);
    }
    Sf = sqrt(Sf);
    vectord s,sd,sdd;
    ulspb(s,sd,sdd, 0, Sf, acc, vel, f);
    int N = s.size();
    p.resize(N);
    for(int i=0; i<N; i++)
    {
        cartpos_t temp;
        for(int j =0; j<3; j++){
            temp.pe[j] = p0.pe[j]+(pf.pe[j]-p0.pe[j])*s[i];
        }
        temp.Rx0 = p0.Rx0+(pf.Rx0-p0.Rx0)*s[i];
        temp.Ry0 = p0.Ry0+(pf.Ry0-p0.Ry0)*s[i];
        temp.Rz0 = p0.Rz0+(pf.Rz0-p0.Rz0)*s[i];
		p[i] = temp;
    }
	return 0;
}

int lintraj(vectorangle & j, joinpos_t j0, cartpos_t pf , int a, int v, int f)
{
    std::ofstream angleout;
    angleout.open("angles.txt");
    vectorpos pos;
    cartpos_t p0;
    forkine(j0.joi, &p0);
    lintraj(pos, p0, pf, a, v, f);
    int N = pos.size();
    j.resize(N);
    j[0] = j0;
	for(int i=0; i<6; i++){
        angleout << std::setprecision(5) << j[0].joi[i] << " ";
	}
	angleout << std::endl;
    for(int i=1; i<N; i++)
    {
        if(i == 531)
        {
            int Q = 1;
            Q = Q;
        }
        invkine(pos[i], j[i-1].joi, j[i].joi);
        for(int h=0; h<6; h++){
            angleout << std::setprecision(5) << (j[i]).joi[h] << " ";
		}
		angleout << std::endl;
    }
    return 0;
}

int lintraj(vecHTransform & T, CartPose pose0, CartPose posef, int a, int v, int f)
{

	Vector3d p0 = pose0.pe;
	Vector3d pf = posef.pe;
	Vector3d p = pf-p0;
    double l = p.norm();

	Vector3d rpy0 = pose0.rpy;
	Vector3d rpyf = posef.rpy;
	Vector3d rpy = rpyf - rpy0;
	double r = abs(rpy.maxCoeff());

    vectord s,sd,sdd;
	if (l / LMAXVEL > r / RMAXVEL)
	{
		double acc = a*LMAXACC/100.0;
		double vel = v*LMAXVEL/100.0;
		ulspb(s,sd,sdd, 0, l, acc, vel, f);
	}
	else
	{
		double acc = a*RMAXACC/100.0;
		double vel = v*RMAXVEL/100.0;
		ulspb(s,sd,sdd, 0, r, acc, vel, f);
	}
	
    int N = s.size();
    T.resize(N);
	for (int i = 0; i < N; i++)
	{
		Vector3d rpyi = rpy0 + s[i] * (rpyf - rpy0);
		Matrix3d R = rpy2ro(rpyi,'d');

		Vector3d pi = p0 + s[i] * (pf - p0);

		T[i] = transl(pi, R);
	}
	return 0;
}

int cirtraj(vectord & s, vecposition & p, point p0, point pi, point pf, int a, int v, int f)
{
	double acc = a * LMAXACC / 100.0;
	double vel = v * LMAXVEL / 100.0;
	Circle cir(p0, pi, pf);
	double Sf = cir.arclen();
	vectord sd, sdd;
	ulspb(s, sd, sdd, 0, Sf, acc, vel, f);
	int N = s.size();
	p.resize(N);
	Matrix3d R;
	R = cir.SO3();
	Vector3d c;
	c = cir.Pc();

	for (int i = 0; i < N; i++)
	{
		p[i] = c + R * cir.coordposi(s[i]*Sf);
	}

	return 0;
}

int cirtraj(seqJointVec & seqangle, Vector3d rpy0, Vector3d rpyf,point p0, point pi, point pf, int a, int v, int f)
{
	vecposition p;
	vectord s;
	cirtraj(s, p, p0, pi, pf, a, v, f);
	size_t N = p.size();
	seqCartPose pose;
	pose.resize(N);
	seqangle.resize(N);
	pose[0].pe = p0;
	pose[0].rpy = rpy0;
	JointVec join0;
	join0 << 0, 0, 0, 0, 0, 0;
	if (invkine(seqangle[0], join0, pose[0]) < 0) {
		std::cout << "inverse kinematics failed in cirtraj \n";
		return -1;
	}
	//cartpos_t pos;
	//pos.pe[0] = p0[0];
	//pos.pe[1] = p0[1];
	//pos.pe[2] = p0[2];
	//pos.Rx0 = rpy[0];
	//pos.Ry0 = rpy[1];
	//pos.Rz0 = rpy[2];
	//double lastjointangle[6] = { 0,0,0,0,0,0 };
	//double jointangle[6];
	//if (invkine(pos, lastjointangle, jointangle) < 0) {
	//	std::cout << "inverse kinematics failed twice\n";
	//		return -1;
	//}
	for (size_t i = 1; i < N; i++)
	{
		pose[i].pe = p[i];
		pose[i].rpy = rpy0 + s[i]*(rpyf-rpy0);
		if (invkine(seqangle[i], seqangle[i - 1], pose[i]) != 0) {
			std::cout << "inverse kinematics failed in cirtraj \n";
			return -1;
		}
	}
	std::ofstream angleout;
	angleout.open("angles.txt");
	for (size_t i = 0; i < seqangle.size(); i++)
	{
		for (int j = 0; j < 6; j++) {
			angleout << seqangle[i][j] << " ";
		}
		angleout << std::endl;
	}
	angleout.close();
	return 0;
}

int arctraj(vecHTransform & T, Matrix4d pose0, Matrix4d posei, Matrix4d posef, int a, int v, int f)
{
	Vector3d p0 = transl(pose0);
	Vector3d pi = transl(posei);
	Vector3d pf = transl(posef);
	Matrix3d R0 = pose0.block(0,0,3,3);
	Matrix3d Rf = posef.block(0,0,3,3);
	Matrix3d Rf0 = R0.inverse()*Rf;
	
	Vector3d k;
	double theta;
	tr2angvec(theta, k, Rf0);

	Vector3d kb = R0 * k;
	std::cout << kb << std::endl;
	// double ang = theta * 180 / PI;
	Circle cir(p0, pi, pf);
	double Sf = cir.arclen();

	vectord s, sd, sdd;

	if(Sf/LMAXVEL > theta/RMAXVEL)
	{
		double acc = a * LMAXACC / 100.0;
		double vel = v * LMAXVEL / 100.0;
		ulspb(s, sd, sdd, 0, Sf, acc, vel, f);
	}
	else
	{
		double acc = a * RMAXACC / 100.0;
		double vel = v * RMAXVEL / 100.0;
		ulspb(s,sd,sdd,0,theta,acc,vel,f);
	}

	int N = s.size();
	vecposition p(N);
	Matrix3d Rc;
	Rc = cir.SO3();
	Vector3d c;
	c = cir.Pc();
	for (int i = 0; i < N; i++)
	{
		p[i] = c + Rc * cir.coordposi(s[i]*Sf);
	}

	vecOrientation R(N);
	for (int i = 0; i < N; i++)
	{
		R[i] = R0 * angvec2r(s[i] * theta, k);
	}

	T.resize(N);
	for (int i = 0; i < N; i++)
	{
		T[i] = transl(p[i], R[i]);
	}
	return 0;
}

int circletraj(vecHTransform & T, Matrix4d pose0, Matrix4d posei, Matrix4d posef, int a, int v, int f)
{
	Vector3d p0 = transl(pose0);
	Vector3d pi = transl(posei);
	Vector3d pf = transl(posef);
	Matrix3d R0 = pose0.block(0,0,3,3);
	Matrix3d Rf = posef.block(0,0,3,3);
	Matrix3d Rf0 = R0.inverse()*Rf;
	
	Vector3d k;
	double theta;
	tr2angvec(theta, k, Rf0);

	Circle cir(p0, pi, pf);
	double Sf = cir.circumference();

	vectord s, sd, sdd;

	if(Sf/LMAXVEL > theta/RMAXVEL)
	{
		double acc = a * LMAXACC / 100.0;
		double vel = v * LMAXVEL / 100.0;
		ulspb(s, sd, sdd, 0, Sf, acc, vel, f);
	}
	else
	{
		double acc = a * RMAXACC / 100.0;
		double vel = v * RMAXVEL / 100.0;
		ulspb(s,sd,sdd,0,theta,acc,vel,f);
	}

	int N = s.size();
	vecposition p(N);
	Matrix3d Rc;
	Rc = cir.SO3();
	Vector3d c;
	c = cir.Pc();
	for (int i = 0; i < N; i++)
	{
		p[i] = c + Rc * cir.coordposi(s[i]*Sf);
	}

	vecOrientation R(N);
	for (int i = 0; i < N; i++)
	{
		R[i] = R0 * angvec2r(s[i] * theta, k);
	}

	T.resize(N);
	for (int i = 0; i < N; i++)
	{
		T[i] = transl(p[i], R[i]);
		//std::cout << T[i] << std::endl << std::endl;
	}
	return 0;
}