#include "trajectory.h"
#include "er20.h"

#ifndef PI
#define PI 3.1415926545
#endif

using namespace Eigen;
using std::queue;
using std::vector;

typedef std::vector<double> vectord;    //The STL of C++
typedef vector<joinpos_t>vectorj;
typedef vector<cartpos_t>vectorc;
typedef vector<incpos_t>vectori;

typedef queue<joinpos_t> queuej;
typedef queue<incpos_t> queuei;

/*******************************************************************
 * 	the point to point function, using the LIPT method
 * 	p0 is the begin
 * 	pf is the end
 * 	vjoin is the vector for jointangle
 *  vcart is the vector for cartpos
 * 	a and v is the percentage of Acceleration and velocity, range 1-100
 * 	return 0 is noerr, -1 is err
 * *****************************************************************/
int ptp(const joinpos_t & p0, const joinpos_t & pf, const char * queue_name, int a, int v, int f = 500);
// int ptp(const incpos_t & inc0, const incpos_t & incf, int a, int v, int f = 500 );
// int ptp(const joinpos_t & p0, const cartpos_t & cpf, int a, int v, int f = 500 );
void PTP(void *cookie);
