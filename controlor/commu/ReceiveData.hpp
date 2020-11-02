#ifndef __RECEIVEDATA_H__
#define __RECEIVEDATA_H__
#include <stdint.h>
#include <pthread.h>

struct receiveData_t
{
	uint16_t statusWrod[6];
	int32_t actualPosition[6];
	int32_t actualVelocity[6];
	int16_t actualTorque[6];
	uint8_t actualOperationMode[6];
};

class ReceiveData
{
	private:
		static receiveData_t data;
		static pthread_mutex_t mutex;

	public:
		ReceiveData();
		~ReceiveData();
        static receiveData_t getData();
        static void writeData(const receiveData_t &wdata);
};
#endif // __RECEIVEDATA_H__
