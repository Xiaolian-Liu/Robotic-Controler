#include "er20.h"
#include "iostream"

int main(void)
{
	double theta[2][7] =
	{
		{0,0,PI/2,0,0,0,0,},
		{0,PI/2,PI/2,0,PI/2,PI/2,0}
	};
	cartpos_t respos[2] = 
	{
		{0,0,0,0,0,0},
		{0,0,0,0,0,0}
	};  //attention, this place can't be cartpos_t * respos[2]
	
	for (int i = 0; i < 2; i++)
	{
		if (-1 == trans(theta[i], &respos[i]))
		{
			std::cout << " forward kenimatics error" << std::endl;
			continue;
		}
		jointspace_show(theta[i]);
		cartspace_show(respos[i]);
	}

	cartpos_t cartpos[2] = 
	{
		{1230,0,920,0,0,0},
		{300,930,920,PI/2,0,0}
	};
	double restheta[2][7] = { {0},{0} };
	double defalasttheta[2][7] = { { 0, 0, PI / 2, 0, 0, 0, 0 },	{ 0, PI / 2, PI / 2, 0, PI / 2, PI / 2, 0 } };

	for (int i = 0; i < 2; i++)
	{
		if (-1 == invtrans(cartpos[i], defalasttheta[i], restheta[i]))
		{
			std::cout << " inverse kenimatics error" << std::endl;
			continue;
		}
		cartspace_show(cartpos[i]);
		jointspace_show(restheta[i]);
	}
	std::cin.get();
}