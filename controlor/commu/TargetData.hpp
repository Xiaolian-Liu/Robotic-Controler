#ifndef __TARGETDATA_H__
#define __TARGETDATA_H__
#include <stdint.h>
#include <pthread.h>


struct targetData_t
{
	uint16_t controlWord[6];
	int32_t targetPosition[6];
	int32_t targetVelocity[6];
	int16_t targetTorque[6];
	uint8_t targetOperationMode[6];

};


class TargetData
{
	private:
        static targetData_t data;
		static pthread_mutex_t mutex;

	public:
		TargetData();
		~TargetData();

		static targetData_t getData();
		static void writeData(const targetData_t &data);
};
#endif // __TARGETDATA_H__
