#ifndef __TARGETDATA_H__
#define __TARGETDATA_H__
#include <stdint.h>
#include "base/SharedMemory.hpp"

struct targetData_t
{
	uint16_t controlWord[6];
	int32_t targetPosition[6];
	int32_t targetVelocity[6];
	int16_t targetTorque[6];
	uint8_t targetOperationMode[6];

};


class TargetData : public SharedMemory
{
	private:

	public:
		TargetData();
		~TargetData();

		targetData_t getData() const;
		void writeData(const targetData_t &data) const;
};
#endif // __TARGETDATA_H__
