#include "trajectory.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include "transform.h"
using std::cout;
using std::endl;

int lin(seqJointVec & seqangle, CartPose p0, CartPose pf, int a, int v, int f);
int arc(seqJointVec & seqangle, Matrix4d T0, Matrix4d Ti, Matrix4d Tf, int a, int v, int f);
int cir(seqJointVec & seqangle, Matrix4d T0, Matrix4d Ti, Matrix4d Tf, int a, int v, int f);
int main(int argc, char *argv[])
{
	seqJointVec angles1, angles2, angles3, angles4;
	CartPose ps,p0, pi, pf;
	ps.pe << 1000, 100, 630;
	ps.rpy << -180, 0, 0;

	p0.pe << 1000, 100, 597.5;
	p0.rpy << -180, 0, 0;

	pi.pe << 1100, 0, 597.5;
	pi.rpy << -180, 0, 0;

	pf.pe << 1000, -100, 597.5;
	pf.rpy << -180, 0, 0;
	lin(angles4, ps, p0, 20, 20, 100);
	lin(angles1, p0, pi, 20, 20, 100);
	lin(angles2, pi, pf, 20, 20, 100);
	lin(angles3, pf, p0, 20, 20, 100);
	std::ofstream linangles;

	linangles.open("E:\\My Projects\\Matlab simulation for robotics\\Learning Test\\linangles.txt");
	for (size_t i = 0; i < angles4.size(); i++)
	{
		for (int j = 0; j < 6; j++)
		{
			linangles << angles4[i][j] << " ";
		}
		linangles << std::endl;
	}

	for (size_t i = 0; i < angles1.size(); i++)
	{
		for (int j = 0; j < 6; j++)
		{
			linangles << angles1[i][j] << " ";
		}
		linangles << std::endl;
	}
	linangles.close();

	linangles.open("E:\\My Projects\\Matlab simulation for robotics\\Learning Test\\linangles.txt", std::ofstream::app);
	for (size_t i = 0; i < angles2.size(); i++)
	{
		for (int j = 0; j < 6; j++)
		{
			linangles << angles2[i][j] << " ";
		}
		linangles << std::endl;
	}
	for (size_t i = 0; i < angles3.size(); i++)
	{
		for (int j = 0; j < 6; j++)
		{
			linangles << angles3[i][j] << " ";
		}
		linangles << std::endl;
	}

	Matrix3d R0, Ri, Rf;
	Matrix4d T0, Ti, Tf;
	R0 = rpy2ro(p0.rpy, 'd');
	Ri = rpy2ro(pi.rpy, 'd');
	Rf = rpy2ro(pf.rpy, 'd');
	T0 = transl(p0.pe, R0);
	Ti = transl(pi.pe, Ri);
	Tf = transl(pf.pe, Rf);

	cout << T0 << endl << endl << Ti << endl << endl << Tf << endl;
	seqJointVec circleangles;
	cir(circleangles, T0, Ti, Tf, 20, 20, 100);
	for (size_t i = 0; i < circleangles.size(); i++)
	{
		for (int j = 0; j < 6; j++)
		{
			linangles << circleangles[i][j] << " ";
		}
		linangles << std::endl;
	}
	
	lin(angles4, p0, ps, 20, 20, 100);
	for (size_t i = 0; i < angles4.size(); i++)
	{
		for (int j = 0; j < 6; j++)
		{
			linangles << angles4[i][j] << " ";
		}
		linangles << std::endl;
	}

	linangles.close();

	p0.pe << 1000, 100, 597.5;
	pi.pe << 1100, 0, 597.5;
	pf.pe << 1000, -100, 597.5;
	p0.rpy << -140, 0, 0;
	pi.rpy << -180, 0, 0;
	pf.rpy << -220, 0, 0;
	

	lin(angles1, p0, pf, 20, 20, 100);
	std::ofstream arcangles;
	arcangles.open("E:\\My Projects\\Matlab simulation for robotics\\Learning Test\\arcangles.txt");
	lin(angles4, ps, p0, 20, 20, 100);
	for (size_t i = 0; i < angles4.size(); i++)
	{
		for (int j = 0; j < 6; j++)
		{
			linangles << angles4[i][j] << " ";
		}
		linangles << std::endl;
	}

	for (size_t i = 0; i < angles1.size(); i++)
	{
		for (int j = 0; j < 6; j++)
		{
			arcangles << angles1[i][j] << " ";
		}
		arcangles << std::endl;
	}
	
	Rf = rpy2ro(pf.rpy, 'd');
	Vector3d kb, k;
	kb << 0, 0, -1;
	k = Rf.inverse()*kb;
	Matrix3d Rf0 = angvec2r(PI, k);
	R0 = Rf * Rf0;
	Ri = Rf * angvec2r(PI_2, k);
	T0 = transl(p0.pe, R0);
	Ti = transl(pi.pe, Ri);
	Tf = transl(pf.pe, Rf);

	arc(angles1, Tf, Ti, T0, 20, 20, 100);
	for (size_t i = 0; i < angles1.size(); i++)
	{
		for (int j = 0; j < 6; j++)
		{
			arcangles << angles1[i][j] << " ";
		}
		arcangles << std::endl;
	}

	lin(angles4, p0, ps, 20, 20, 100);
	for (size_t i = 0; i < angles4.size(); i++)
	{
		for (int j = 0; j < 6; j++)
		{
			linangles << angles4[i][j] << " ";
		}
		linangles << std::endl;
	}

	arcangles.close();
	return 0;
}

int lin(seqJointVec & jangle, CartPose p0, CartPose pf, int a, int v, int f)
{
	jangle.clear();
	vecHTransform T;
	lintraj(T, p0, pf, a, v, f);
	size_t N = T.size();
	jangle.resize(N);
	JointVec lastangle;
	lastangle << 0, 0, 0, 0, 0, 0;
	invkine(jangle[0], lastangle, p0);

	for (size_t i = 1; i < N; i++)
	{
		invkine(jangle[i], jangle[i - 1], T[i]);
	}
	return 0;
}

int arc(seqJointVec & jangle, Matrix4d T0, Matrix4d Ti, Matrix4d Tf, int a, int v, int f)
{
	jangle.clear();
	vecHTransform T;
	arctraj(T, T0, Ti, Tf, a, v, f);
	size_t N = T.size();
	jangle.resize(N);
	JointVec lastangle;
	lastangle << 0, 0, 0, 0, 0, 0;
	invkine(jangle[0], lastangle, T[0]);

	for (size_t i = 1; i < N; i++)
	{
		invkine(jangle[i], jangle[i - 1], T[i]);
	}
	return 0;
}
int cir(seqJointVec & jangle, Matrix4d T0, Matrix4d Ti, Matrix4d Tf, int a, int v, int f)
{
	jangle.clear();
	vecHTransform T;
	circletraj(T, T0, Ti, Tf, a, v, f);
	size_t N = T.size();
	jangle.resize(N);
	JointVec lastangle;
	lastangle << 0, 0, 0, 0, 0, 0;
	invkine(jangle[0], lastangle, T[0]);

	for (size_t i = 1; i < N; i++)
	{
		if (invkine(jangle[i], jangle[i - 1], T[i]))
		{
			cout << i << endl;
			int abcd;
			std::cin >> abcd;
		}
	}
	return 0;
}