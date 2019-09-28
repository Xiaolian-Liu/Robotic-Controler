#include "er20.h"
#include <iostream>
using namespace std;

int main(void)
{
	const double jointangle[5][6] =
	{
		{0,			0,			0,			0,			0,			0},
		{-21.179,	41.862,		-39.407,	26.328,		20.561,		41.544},
		{9.11,		23.169,		-39.407,	26.328,		20.561,		41.544},
		{15.318,	29.978,		-36.999,	45.869,		23.620,		41.543},
		{12.405,	25.226,		-55.917,	75.108,		1.179,		273.720}
	};
	const cartpos_t cartposes[5] = 
	{
		{1053.855,		0.301,		1425.886,		0,			90,			-180},
		{474.063,		-204.273,	1303.171,		-30.769,	69.231,		-110.169},
		{747.159,		100.396,	1149.687,		0.142,		87.684,		-113.232},
		{653.419,		142.021,	1247.954,		-1.634,		80.508,		-92.216},
		{653.441,		142.033,	880.380,		11.130,		120.381,	168.157}
	};  //attention, this place can't be cartpos_t * respos[5]

	cartpos_t resposes[5];
	double resjoinangl[5][6];


	for (int i = 0; i < 5; i++)
	{
		jointangle_show(jointangle[i]);
		int axies;
		if (axies = forkine(jointangle[i], resposes + i))
			cout << " forward kenimatics error, axis " << axies << "beyound limits" << endl;
		else
			cartpose_show(resposes[i]);
	}
	double defalastjoinangl[6] = { 0, 0, 0, 0, 0, 0};

	for (int i = 0; i < 5; i++)
	{
		cartpose_show(cartposes[i]);
		if (invkine(cartposes[i], defalastjoinangl, resjoinangl[i]))
			cout << " inverse kenimatics error, the pose beyound limits" << endl;
		else
			jointangle_show(resjoinangl[i]);
	}
	std::cin.get();
}