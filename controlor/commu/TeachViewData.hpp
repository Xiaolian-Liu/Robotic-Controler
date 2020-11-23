#ifndef __TEACHVIEWDATA_H__
#define __TEACHVIEWDATA_H__

#include <stdint.h>
#include <pthread.h>


struct teachViewData_t
{
//    uint16_t controlWord[6];
    int32_t targetPosition[6];
    int32_t targetVelocity[6];
    int16_t targetTorque[6];
    uint8_t targetOperationMode[6];

    bool startSignal;
    bool stopSignal;
    bool enable;
    bool shutDown;
    bool stop;
    bool faultReset;

    int8_t motionMode;  //1 jog up 2 jog down
                        //3 PTP
                        //4 line
                        //5 circle
//    Point endPoint;
//    Point middlePoint;

    uint8_t ethercatStateMechine[6];
    int32_t ethercatErrorCode[6];
};


class TeachViewData
{
    private:
        static pthread_mutex_t mutex;

    public:
        TeachViewData();
        ~TeachViewData();

        static teachViewData_t data;
        static teachViewData_t getData();
        static void writeData(const teachViewData_t &data);
};

#endif // __TEACHVIEWDATA_H__
