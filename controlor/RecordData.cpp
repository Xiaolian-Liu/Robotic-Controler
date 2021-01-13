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
    ofPos.open("position_FourierFrictionTestAxis1.txt");
    ofVel.open("velocity_FourierFrictionTestAxis1.txt");
    ofTorque.open("torque_FourierFrictionTestAxis1.txt");

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
            control->qBuff.pop();
        }

        while(control->qdBuff.size() > 0)
        {
            JointVec qd = control->qdBuff.front();
            // std::cout << "qd:";
            // printJointVec(qd);
            for (int i = 0; i < 6; i++)
            {
                ofVel << qd[i] << " ";
            }
            ofVel << std::endl;
            control->qdBuff.pop();
        }

        while(control->TorqueBuff.size() > 0)
        {
            // std::cout << "Torque:";
            JointVec Torque = control->TorqueBuff.front();
            // printJointVec(Torque);
            for (int i = 0; i < 6; i++)
            {
                ofTorque << Torque[i] << " ";
            }
            ofTorque << std::endl;
            control->TorqueBuff.pop();
        }

        usleep(100000);
        
    }

    ofPos.close();
    ofVel.close();
    ofTorque.close();
}
