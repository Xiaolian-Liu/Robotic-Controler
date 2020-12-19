#include "EthercatMaster.hpp"  
#include <iostream>
#include <string>
using std::cerr;
using std::cout;
using std::endl;
using std::string;

EthercatMaster::EthercatMaster(uint32_t cycleTime, unsigned int index)
{
    this->cycleTime = cycleTime;
    this->masterIndex = index;
    nSlaves = 0;
    // allSlavesSate = INIT;
    // linkUp = false;
    master = nullptr;
    domain = nullptr;
    domainPtr = nullptr;
    domainRegist = nullptr;

    offControlWord = nullptr;
    offTargetPosition = nullptr;
    offTargetVelocity = nullptr;
    offTargetTorque = nullptr;
    offTargetModeOP = nullptr;
    offDummyByte1 = nullptr;
    offTouchProbeFunc = nullptr;
    offSatesWord = nullptr;
    offActualPosition = nullptr;
    offActualVelocity = nullptr;
    offActualTorque = nullptr;
    offActualModeOP = nullptr;
    offDummyByte2 = nullptr;
    offFollowError = nullptr;
    offDigitalInputs = nullptr;
    offTouchProbeSatte = nullptr;
    offTouchProbePos1 = nullptr;

    offByte1 = nullptr;
    offByte2 = nullptr;

    for(int i = 0; i < 6; i++)
    {
        targData.controlWord[i] = 0;
        targData.targetOperationMode[i] = 8;
        targData.targetVelocity[i] = 0;
        targData.targetPosition[i] = 0;
    }
}

EthercatMaster::~EthercatMaster()
{
    clear();
}

int EthercatMaster::init() 
{
    clear();
    int initRes = 0;

    master = ecrt_request_master(masterIndex);
    cout << "request master: " << masterIndex << endl;
    if(!master)
    {
        cerr << "request master: " << masterIndex << " failed, EtherCAT Master construct failed" << endl;
        return -1;
    }

    ecrt_master_state(master, &masterState);
    nSlaves = masterState.slaves_responding;
    // allSlavesSate = ALState(masterState.al_states);
    // linkUp = masterState.link_up;

    for (unsigned int i = 0; i < nSlaves; i++)
    {
        ec_slave_info_t slaveInfo;
        ecrt_master_get_slave(master, i, &slaveInfo);

        int sync_Count = slaveInfo.sync_count;
        ec_sync_info_t *syncs = new ec_sync_info_t[sync_Count];
        int totalPdos = 0;
        for (int j = 0; j < sync_Count; j++)
        {
            int err = ecrt_master_get_sync_manager(master, i, j, syncs + j);
            if(err){
                cerr << "Master init failed, can't get the sync managers" << endl;
                delete[] syncs;
                return -1;
            }
            totalPdos += syncs[j].n_pdos;
        }

        ec_pdo_info_t *pdos = new ec_pdo_info_t[totalPdos];
        int totalEntries = 0;
        int pdoIndex = 0;
        for (int j = 0; j < sync_Count; j++)
        {
            int n_Pdos = syncs[j].n_pdos;
            if (n_Pdos > 0)
            {
                
                int err = ecrt_master_get_pdo(master, i, j, 0, pdos + pdoIndex);
                if(err){
                    cerr << "Master init failed, can't get the pdos" << endl;
                    delete[] syncs;
                    delete[] pdos;
                    return -1;
                }
                totalEntries += pdos[pdoIndex].n_entries;
                pdoIndex++;
            }
        }

        ec_pdo_entry_info_t *entries = new ec_pdo_entry_info_t[totalEntries];
        int entriesOff = 0;
        int pdoOff= 0;
        for (int j = 0; j < sync_Count; j++)
        {
            int n_Pdos = syncs[j].n_pdos;
            if(n_Pdos > 0)
            {
                int n_Entries = pdos[pdoOff].n_entries;
                for (int entrysIndex = 0; entrysIndex < n_Entries; entrysIndex++)
                {
                    ec_pdo_entry_info_t entriesGet;
//                    int err = ecrt_master_get_pdo_entry(master, i, j, 0, entrysIndex, entries + entriesOff + entrysIndex);
                    int err = ecrt_master_get_pdo_entry(master, i, j, 0, entrysIndex, &entriesGet);
                    entries[entriesOff+entrysIndex] = entriesGet;
                    if(err){
                        cerr << "Master finit failed, can't get the pdo entries" << endl;
                        delete[] syncs;
                        delete[] pdos;
                        delete[] entries;
                        return -1;

                    }
                }
                pdoOff++;
                entriesOff += n_Entries;
            }
        }

        string slaveName(slaveInfo.name);
        Slave slavei(slaveName,
                     slaveInfo.alias,
                     slaveInfo.position,
                     slaveInfo.vendor_id,
                     slaveInfo.product_code,
                     totalEntries,
                     totalPdos,
                     slaveInfo.sync_count);

        slavei.setPdoEntries(entries);
        slavei.setPdos(pdos);
        slavei.setSyncManger(syncs);
        slave.push_back(slavei);

        delete[] syncs;
        delete[] pdos;
        delete[] entries;
    }
    

    // for (unsigned int i = 0; i < nSlaves; i++)
    // {
    //     cout << slave[i] << endl;
    // }

    offControlWord = new unsigned int[nSlaves];
    offTargetPosition = new unsigned int[nSlaves];
    offTargetVelocity = new unsigned int[nSlaves];
    offTargetTorque = new unsigned int[nSlaves];
    offTargetModeOP = new unsigned int[nSlaves];
    offDummyByte1 = new unsigned int[nSlaves];
    offTouchProbeFunc = new unsigned int[nSlaves];

    offSatesWord = new unsigned int[nSlaves];
    offActualPosition = new unsigned int[nSlaves];
    offActualVelocity = new unsigned int[nSlaves];
    offActualTorque = new unsigned int[nSlaves];
    offActualModeOP = new unsigned int[nSlaves];
    offDummyByte2 = new unsigned int[nSlaves];
    offFollowError = new unsigned int[nSlaves];
    offDigitalInputs = new unsigned int[nSlaves];
    offTouchProbeSatte = new unsigned int[nSlaves];
    offTouchProbePos1 = new unsigned int[nSlaves];

    offByte1 = new unsigned int[nSlaves];
    offByte2 = new unsigned int[nSlaves];

    for (unsigned int i = 0; i < nSlaves; i++)
    {
        offControlWord[i]       = maxByteOffset;
        offTargetPosition[i]    = maxByteOffset;
        offTargetVelocity[i]    = maxByteOffset;
        offTargetTorque[i]      = maxByteOffset;
        offTargetModeOP[i]      = maxByteOffset;
        offDummyByte1[i]        = maxByteOffset;
        offTouchProbeFunc[i]       = maxByteOffset;

        offSatesWord[i]         = maxByteOffset;
        offActualPosition[i]    = maxByteOffset;
        offActualVelocity[i]    = maxByteOffset;
        offActualTorque[i]      = maxByteOffset;
        offActualModeOP[i]      = maxByteOffset;
        offDummyByte2[i]        = maxByteOffset;
        offFollowError[i]       = maxByteOffset;
        offDigitalInputs[i]     = maxByteOffset;
        offTouchProbeSatte[i]   = maxByteOffset;
        offTouchProbePos1[i]    = maxByteOffset;

        offByte1[i]             = maxByteOffset;
        offByte2[i]             = maxByteOffset;
    }

    int entriesNum = 0;
    for (unsigned int i = 0; i < nSlaves; i++){
        entriesNum += slave[i].nEntries();
    }

    domainRegist = new ec_pdo_entry_reg_t[entriesNum+1];

    int ByteIndex = 0;
    for (unsigned int i = 0; i < nSlaves; i++)
    {
        int n = slave[i].nEntries();
        for (int j = 0; j < n; j++)
        {
            unsigned int *offset;
            uint16_t index = slave[i].pdoEntry()[j].index;
            uint8_t subIndex = slave[i].pdoEntry()[j].subindex;

            switch (index)
            {
            case 0x6040:
                offset = offControlWord + i;
                break;
            case 0x607a:
                offset = offTargetPosition + i;
                break;
            case 0x60ff:
                offset = offTargetVelocity + i;
                break;
            case 0x6071:
                offset = offTargetTorque + i;
                break;
            case 0x6060:
                offset = offTargetModeOP + i;
                break;
            case 0x5ffe:
                offset = offDummyByte1 + i;
                break;
            case 0x60b8:
                offset = offTouchProbeFunc + i;
                break;


            
            case 0x6041:
                offset = offSatesWord + i;
                break;
            case 0x6064:
                offset = offActualPosition + i;
                break;
            case 0x606c:
                offset = offActualVelocity + i;
                break;
            case 0x6077:
                offset = offActualTorque + i;
                break;
            case 0x6061:
                offset = offActualModeOP + i;
                break;
            case 0x5fff:
                offset = offDummyByte2 + i;
                break;
            case 0x60f4:
                offset = offFollowError + i;
                break;
            case 0x60fd:
                offset = offDigitalInputs + i;
                break;
            case 0x60b9:
                offset = offTouchProbeSatte + i;
                break;
            case 0x60ba:
                offset = offTouchProbePos1 + i;
                break;
            
            case 0x0000:
                if(ByteIndex %2 )
                {
                    offset = offByte2 + i;
                }
                else
                {
                    offset = offByte1 + i;
                }
                ByteIndex++;
                break;
            default:
                offset = &offdata;
                break;
            }

            domainRegist[n * i + j] = {
                slave[i].Alias(),
                slave[i].Position(),
                slave[i].VenderId(),
                slave[i].ProductCode(),
                index, subIndex, offset};

        }
    }
    domainRegist[entriesNum] = {};

    domain = ecrt_master_create_domain(master);
    if(!domain){
        cerr << "Master construct failed, can't creat domain" << endl;
        initRes = -1;
        goto init_end;
    }

    for (unsigned int i = 0; i < nSlaves; i++){
        slaveConfig.push_back(nullptr);
    }

    for (int i = 0; i < nSlaves; i++)
    {
        if(!(slaveConfig[i] = ecrt_master_slave_config(
            master,
            slave[i].Alias(),
            slave[i].Position(),
            slave[i].VenderId(),
            slave[i].ProductCode()))){
            cerr << "Master init failed, can't get slave: " << i << "configuration" << endl;
            initRes = -1;
            goto init_end;
        }

        if (ecrt_slave_config_sdo8(slaveConfig[i], 0x6060, 0, 8))  /*set the operation mode 8 RSP */
        {
            cerr << "Failed to configure sdo. \n";
            goto init_end;
        }

        cout << "Configuring Slave" << i << " PDOs...\n";
        if(ecrt_slave_config_pdos(slaveConfig[i], EC_END, slave[i].syncManger())){
            cerr << "Master init failed, fail to configure Slave " << i << "PDOS" << endl;
            initRes = -1;
            goto init_end;
        }
        cout << "configureing Slave " << i << "PDOs is completed!\n";
    }

    if(ecrt_domain_reg_pdo_entry_list(domain, domainRegist)){
        cerr << "PDO entries registration filed! \n";
        initRes = -1;
        goto init_end;
    }
    else
    {
        cout << "PDO entry registration succeed!\n";
    }

    for (int i = 0; i < nSlaves; i++){
        ecrt_slave_config_dc(slaveConfig[i], 0x0300, cycleTime, 2000000, 0, 0);
    }

init_end:
    if(-1 == initRes){
        clear();
    }
    return 0;
}

void EthercatMaster::setApplicationTime(uint64_t appTime) 
{
    ecrt_master_application_time(master, appTime);
}

int EthercatMaster::active() 
{
    cout << "Activating master...\n";
    if (ecrt_master_activate(master)){
		return -1;
    }

    if(!(domainPtr = ecrt_domain_data(domain))){
        return -1;
    }

    cout << "ControlWord  " << offControlWord[0] << endl;
    cout << "TargetPos  " << offTargetPosition[0] << endl;
    cout << "TargetVel  " << offTargetVelocity[0] << endl;
    cout << "ModeofOP  \n" << offTargetModeOP[0] << endl;

    cout << "StatusWord  " << offSatesWord[0] << endl;
    cout << "actualpos  " << offActualPosition[0] << endl;

    cout << "active done! can start the cyclic function." << endl;
}

void EthercatMaster::refreshData(receiveData_t & receivedata)
{
    ecrt_master_receive(master);
    ecrt_domain_process(domain);
    ec_domain_state_t domainState;
    ecrt_domain_state(domain, &domainState);
    for (int i = 0; i < nSlaves; i++)
    {
        if(offSatesWord[i] != maxByteOffset)
        {
            receivedata.statusWrod[i] = EC_READ_U16(domainPtr + offSatesWord[i]);
        }

        if(offActualPosition[i] != maxByteOffset)
        {
            receivedata.actualPosition[i] = EC_READ_S32(domainPtr + offActualPosition[i]);
        }

        if(offActualVelocity[i] != maxByteOffset)
        {
            receivedata.actualVelocity[i] = EC_READ_S32(domainPtr + offActualVelocity[i]);
        }

        if(offActualTorque[i] != maxByteOffset)
        {
            receivedata.actualTorque[i] = EC_READ_S16(domainPtr + offActualTorque[i]);
        }

        if(offActualModeOP[i] != maxByteOffset)
        {
            receivedata.actualOperationMode[i] = EC_READ_U8(domainPtr + offActualModeOP[i]);
        }
    }
}

void EthercatMaster::refreshStata(stateData_t &state) 
{
    ec_domain_state_t ds;
    ecrt_domain_state(domain, &ds);

    ec_master_state_t ms;
    ecrt_master_state(master, &ms);

    state.al_states = ms.al_states;
    state.link_up = ms.link_up;
    state.slaves_responding = ms.slaves_responding;

    state.WKC = ds.working_counter;
    state.WC_state = ds.wc_state;
    state.redundancy_active = ds.redundancy_active;

}

void EthercatMaster::receive() 
{
    ecrt_master_receive(master);
    ecrt_domain_process(domain);
    ec_domain_state_t domainState;
    ecrt_domain_state(domain, &domainState);

    state.isEnable = true;
    state.isErrExist = false;
    for (int i = 0; i < nSlaves; i++)
    {
        if(offSatesWord[i] != maxByteOffset)
        {
            recvData.statusWrod[i] = EC_READ_U16(domainPtr + offSatesWord[i]);
        }
//        state.isEnable = state.isEnable && ((recvData.statusWrod[i] & 0x006f) == 0x0027);
        state.isEnable = state.isEnable && ((recvData.statusWrod[i] & 0x006f) == 0x0027);
        state.isErrExist = state.isErrExist || ((recvData.statusWrod[i] & 0x0008) == 0x0008);

        if(offActualPosition[i] != maxByteOffset)
        {
            recvData.actualPosition[i] = EC_READ_S32(domainPtr + offActualPosition[i]);
        }

        if(offActualVelocity[i] != maxByteOffset)
        {
            recvData.actualVelocity[i] = EC_READ_S32(domainPtr + offActualVelocity[i]);
        }

        if(offActualTorque[i] != maxByteOffset)
        {
            recvData.actualTorque[i] = EC_READ_S16(domainPtr + offActualTorque[i]);
        }

        if(offActualModeOP[i] != maxByteOffset)
        {
            recvData.actualOperationMode[i] = EC_READ_U8(domainPtr + offActualModeOP[i]);
        }
    }

    ec_domain_state_t ds;
    ecrt_domain_state(domain, &ds);

    ec_master_state_t ms;
    ecrt_master_state(master, &ms);

    state.al_states = ms.al_states;
    state.link_up = ms.link_up;
    state.slaves_responding = ms.slaves_responding;

    state.WKC = ds.working_counter;
    state.WC_state = ds.wc_state;
    state.redundancy_active = ds.redundancy_active;
}


void EthercatMaster::sendData(const targetData_t &targetData)
{

    for (int i = 0; i < nSlaves; i++)
    {
        if(offControlWord[i] != maxByteOffset)
        {
            EC_WRITE_U16(domainPtr + offControlWord[i], targetData.controlWord[i]);
        }

        if(offTargetPosition[i] != maxByteOffset)
        {
            EC_WRITE_S32(domainPtr + offTargetPosition[i], targetData.targetPosition[i]);
        }

        if(offTargetVelocity[i] != maxByteOffset)
        {
            EC_WRITE_S32(domainPtr + offTargetVelocity[i], targetData.targetVelocity[i]);
        }

        if(offTargetTorque[i] != maxByteOffset)
        {
            EC_WRITE_S16(domainPtr + offTargetTorque[i], targetData.targetTorque[i]);
        }

        if(offTargetModeOP[i] != maxByteOffset)
        {
            EC_WRITE_U8(domainPtr + offTargetModeOP[i], targetData.targetOperationMode[i]);
        }

    }
    ecrt_domain_queue(domain);
    ecrt_master_send(master);
}

void EthercatMaster::send() 
{
    for (int i = 0; i < nSlaves; i++)
    {
        if(offControlWord[i] != maxByteOffset)
        {
            EC_WRITE_U16(domainPtr + offControlWord[i], targData.controlWord[i]);
        }

        if(offTargetPosition[i] != maxByteOffset)
        {
            EC_WRITE_S32(domainPtr + offTargetPosition[i], targData.targetPosition[i]);
        }

        if(offTargetVelocity[i] != maxByteOffset)
        {
            EC_WRITE_S32(domainPtr + offTargetVelocity[i], targData.targetVelocity[i]);
        }

        if(offTargetTorque[i] != maxByteOffset)
        {
            EC_WRITE_S16(domainPtr + offTargetTorque[i], targData.targetTorque[i]);
        }

        if(offTargetModeOP[i] != maxByteOffset)
        {
            EC_WRITE_U8(domainPtr + offTargetModeOP[i], targData.targetOperationMode[i]);
        }
    }
    ecrt_domain_queue(domain);
    ecrt_master_send(master);
}

void EthercatMaster::sync(uint64_t time) 
{
    ecrt_master_sync_reference_clock_to(master, time);
    ecrt_master_sync_slave_clocks(master);
}

void EthercatMaster::resetSlaveFault() 
{
    for (unsigned int i = 0; i < nSlaves; i++)
    {
        targData.controlWord[i] = 0x0080;
    }
}

void EthercatMaster::enableSlave() 
{
    for (unsigned int i = 0; i < 6; i++)
    {
        if ((recvData.statusWrod[i] & 0x006f) == 0x0027) // opreation enable
        {
            
        }
        else if( (recvData.statusWrod[i] & 0x004f) == 0x0040  ) //switch on disable
            targData.controlWord[i] =  0x0006;
        else if( (recvData.statusWrod[i] & 0x006f) == 0x0021  ) // read to switch on
            targData.controlWord[i] = 0x0007;
        else if( (recvData.statusWrod[i] & 0x006f) == 0x0023  )   //switch on enable
            targData.controlWord[i] = 0x000F;
    } 
}

void EthercatMaster::shutDownSlave() 
{
    for (unsigned int i = 0; i < 6; i++)
    {
        targData.controlWord[i] = 0x0006;
    }
}

void EthercatMaster::clear() 
{
    slave.erase(slave.begin(), slave.end());
    if(master){
        ecrt_release_master(master);
    }
    master = nullptr;

    free(domain);
    domain = nullptr;

    for (int i = 0; i < slaveConfig.size(); i++)
    {
        free(slaveConfig[i]);
        slaveConfig[i] = nullptr;
    }
    slaveConfig.erase(slaveConfig.begin(), slaveConfig.end());

    delete[] domainRegist;
    domainRegist = nullptr;

    delete[] offControlWord;
    delete[] offTargetPosition;
    delete[] offTargetVelocity;
    delete[] offTargetTorque;
    delete[] offTargetModeOP;
    delete[] offTouchProbeFunc;
    delete[] offSatesWord;
    delete[] offActualPosition;
    delete[] offActualVelocity;
    delete[] offActualTorque;
    delete[] offActualModeOP;
    delete[] offFollowError;
    delete[] offDigitalInputs;
    delete[] offTouchProbeSatte;
    delete[] offTouchProbePos1;

    delete[] offByte1;
    delete[] offByte2;

    offControlWord = nullptr;
    offTargetPosition = nullptr;
    offTargetVelocity = nullptr;
    offTargetTorque = nullptr;
    offTargetModeOP = nullptr;
    offDummyByte1 = nullptr;
    offTouchProbeFunc = nullptr;
    offSatesWord = nullptr;
    offActualPosition = nullptr;
    offActualVelocity = nullptr;
    offActualTorque = nullptr;
    offActualModeOP = nullptr;
    offDummyByte2 = nullptr;
    offFollowError = nullptr;
    offDigitalInputs = nullptr;
    offTouchProbeSatte = nullptr;
    offTouchProbePos1 = nullptr;

    offByte1 = nullptr;
    offByte2 = nullptr;
}
