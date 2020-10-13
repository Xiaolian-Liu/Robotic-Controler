#ifndef __RECEIVEDATA_H__
#define __RECEIVEDATA_H__
#include <stdint.h>

class ReceiveData  
{
	private:

	public:
		
		uint16_t statusWrod[6];
		int32_t actualPosition[6];
		int32_t actualVelocity[6];
		int16_t actualTorque[6];
		uint8_t actualOperationMode[6];

		ReceiveData();
		~ReceiveData();

};
#endif // __RECEIVEDATA_H__