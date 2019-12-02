#include "trajectory.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include "circle.h"

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

int cirtraj(vecposition & p, point p0, point pi, point pf, int a, int v, int f)
{
	double acc = a * LMAXACC / 100.0;
	double vel = v * LMAXVEL / 100.0;
	Circle cir(p0, pi, pf);
	double Sf = cir.arclen();
	vectord s, sd, sdd;
	ulspb(s, sd, sdd, 0, Sf, a, v, f);
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
