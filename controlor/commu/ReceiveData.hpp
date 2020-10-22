#ifndef __RECEIVEDATA_H__
#define __RECEIVEDATA_H__
#include <stdint.h>
#include "base/SharedMemory.hpp"

struct receiveData_t
{
	uint16_t statusWrod[6];
	int32_t actualPosition[6];
	int32_t actualVelocity[6];
	int16_t actualTorque[6];
	uint8_t actualOperationMode[6];
};


class ReceiveData : public SharedMemory
{
	private:

	public:

		ReceiveData();
		~ReceiveData();
		receiveData_t getData() const;
		void writeData(const receiveData_t &data) const;
};
#endif // __RECEIVEDATA_H__
