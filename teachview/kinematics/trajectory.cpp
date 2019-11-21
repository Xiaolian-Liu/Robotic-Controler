#include "trajectory.h"
#include <iostream>

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

