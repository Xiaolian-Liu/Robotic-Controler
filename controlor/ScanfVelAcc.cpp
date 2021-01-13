#include "ScanfVelAcc.hpp"  
#include "iostream"
#include <unistd.h>
using std::cin;
using std::cout;

ScanfVelAcc::ScanfVelAcc(Controller * con)
{
    this->control = con;
}

ScanfVelAcc::~ScanfVelAcc()
{
	
}
void ScanfVelAcc::run() 
{
    while (isRun)
    {
        usleep(500000);
        cout << "Please input the Velocity Percent: ";
        cin >> control->velPercent;
    }
}
