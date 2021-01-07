#include "RecordData.hpp"  
#include "fstream"
#include <unistd.h>

using std::ofstream;

RecordData::RecordData(Controller * c) : Thread("RecordData"), control(c)
{

}

RecordData::~RecordData()
{

}
void RecordData::run() 
{
    ofstream ofPos, ofVel, ofTorque;
    ofPos.open("position.txt");
    ofVel.open("velocity.txt");
    ofTorque.open("torque.txt");

    while (isRun)
    {
        while(control->qBuff.size() > 0)
        {
            JointVec q = control->qBuff.front();
            for (int i = 0; i < 6; i++)
            {
                ofPos << q[i] << " ";
            }
            ofPos << std::endl;
        }

        while(control->qdBuff.size() > 0)
        {
            JointVec qd = control->qdBuff.front();
            for (int i = 0; i < 6; i++)
            {
                ofVel << qd[i] << " ";
            }
            ofVel << std::endl;
        }

        while(control->TorqueBuff.size() > 0)
        {
            JointVec Torque = control->TorqueBuff.front();
            for (int i = 0; i < 6; i++)
            {
                ofTorque << Torque[i] << " ";
            }
            ofTorque << std::endl;
        }

        usleep(100000);
        
    }

    ofPos.close();
    ofVel.close();
    ofTorque.close();
}
