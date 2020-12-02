#ifndef __ETHERCATMASTER_H__
#define __ETHERCATMASTER_H__

#include "Slave.hpp"
#include "commu/ReceiveData.hpp"
#include "commu/TargetData.hpp"
#include "commu/StateData.hpp"
#include <vector>

using std::vector;

class EthercatMaster  
{
	private:
		unsigned int masterIndex;
		uint32_t cycleTime;
        unsigned int nSlaves;
		vector<Slave> slave;
		// ALState allSlavesSate;
		// bool linkUp;
		ec_master_t *master;
        ec_master_state_t masterState;
		ec_domain_t *domain;
		ec_domain_state_t domainState;
		vector<ec_slave_config_t *> slaveConfig;

		uint8_t *domainPtr;
		ec_pdo_entry_reg_t *domainRegist;

		unsigned int *offControlWord;
		unsigned int *offTargetPosition;
		unsigned int *offTargetVelocity;
		unsigned int *offTargetTorque;
		unsigned int *offTargetModeOP;
		unsigned int *offDummyByte1;
		unsigned int *offTouchProbeFunc;

		unsigned int *offSatesWord;
		unsigned int *offActualPosition;
		unsigned int *offActualVelocity;
		unsigned int *offActualTorque;
		unsigned int *offActualModeOP;
		unsigned int *offDummyByte2;
		unsigned int *offFollowError;
		unsigned int *offDigitalInputs;
		unsigned int *offTouchProbeSatte;
		unsigned int *offTouchProbePos1;

		unsigned int *offByte1;
		unsigned int *offByte2;
		
		unsigned int offdata;
		const unsigned int maxByteOffset = 6553600;


		void clear();

	public:
		EthercatMaster(uint32_t cycleTime, unsigned int index = 0);
		~EthercatMaster();
		int init();
		void setApplicationTime(uint64_t appTime);
		int active();
		void refreshData(receiveData_t &receivedata);
		void refreshStata(stateData_t &statedata);
        void receive();
		void sendData(const targetData_t &targetData);
		void send();
		void sync(uint64_t time);

		receiveData_t recvData;
        targetData_t  targData;
        stateData_t   state;

        void resetSlaveFault();
        void enableSlave();
        void shutDownSlave();
};
#endif // __ETHERCATMASTER_H__
