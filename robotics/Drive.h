#pragma once
#include <stdint.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

/* The value of ctl_wrd */
#define SHUDOWN                     0x0006
#define SWITCH_ON                   0x0007
#define DISABLE_VOLTAGE             0x0000
#define QUICK_STOP                  0x0002
#define DISABLE_OPERATION           0x0007
#define ENABLE_OPERATION            0x000F
#define FAULT_RESET                 0x0080

/* The value of sta_wrd */
#define NOT_READY_TO_SWITCH_ON      0x0000
#define SWITCH_ON_DISABLED          0x0040
#define READY_TO_SWITCH_ON          0x0021
#define SWITCH_ON_ENABLED           0x0023
#define OPERATION_ENABLED           0x0027
#define QUICK_STOP_ACTIVE           0x0007
#define FAULT_REACTION_ACTIVE       0x000F
#define FAULT                       0x0008

//We have to set the mode of the drive according to our requirment
/* The vaule and meanning of op_mode */
#define PP          0x01
#define PV          0x03
#define PT          0x04
#define HM          0x06
#define IP          0x07
#define CSP         0x08
#define CSV         0x09
#define CST         0x0A

namespace RKD {

class Drive
{
private:
	unsigned int nBit;
	int32_t offset;			//increment offset


public:
	Drive();
	Drive(unsigned int bBit, int32_t offset);
	static double degree2rad(double angle);
	static double rad2degree(double theta);
	double increment2jointangle(int32_t inc);
	int32_t jointangle2increment(double jointangle);
	void setOffset(int32_t offSet);
	virtual ~Drive();
};

}
