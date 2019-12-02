#include "trajectory.h"
#include <iostream>
#include <fstream>
#include <cmath>

int main(int argc, char *argv[])
{
	vectorangle angle;
	joinpos_t angle0 = { {0,0,0,0,90,0} };
	cartpos_t pos0;
	forkine(angle0.joi, &pos0);
	cartpos_t posf;
	posf.pe[0] = pos0.pe[0];
	posf.pe[1] = -1000;
	posf.pe[2] = pos0.pe[2];
	posf.Rx0 = pos0.Rx0;
	posf.Ry0 = pos0.Ry0;
	posf.Rz0 = pos0.Rz0;
	lintraj(angle, angle0, posf, 100, 100);

	vecposition p;
	point p0, pi, pf;
	p0 << 20, 0, 0;
	pi << 0, 20, 0;
	pf << 0, 0, 20;
	cirtraj(p, p0, pi, pf, 10, 20);
	std::ofstream circlepath;
	circlepath.open("circle.txt");
	for (unsigned int i = 0; i < p.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			circlepath << p[i][j] << " ";
		}
		circlepath << std::endl;
	}

	int ret;
	std::cin >> ret;
	return ret;
}
