#ifndef __TARGETDATA_H__
#define __TARGETDATA_H__
#include <stdint.h>


class TargetData  
{
	private:

	public:
		uint16_t controlWord[6];
		int32_t targetPosition[6];
		int32_t targetVelocity[6];
		int16_t targetTorque[6];
		uint8_t targetOperationMode[6];

		TargetData();
		~TargetData();

};
#endif // __TARGETDATA_H__