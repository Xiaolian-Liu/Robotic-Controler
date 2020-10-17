#include <iostream>
#include <Eigen/Dense>
using namespace Eigen;
using std::cout;
using std::endl;
int main()
{
	Matrix4d m;
	m << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16;
	cout << m << endl;;
	m.setIdentity();
	cout << m;
	int a;
	std::cin >> a;
	return 0;

}