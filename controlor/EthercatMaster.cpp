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
    allSlavesSate = INIT;
    linkUp = false;
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
    TouchProbeFunc = nullptr;
    offSatesWord = nullptr;
    offActualPosition = nullptr;
    offActualVelocity = nullptr;
    offActualTorque = nullptr;
    offActualModeOP = nullptr;
    offBummyByte2 = nullptr;
    offFollowError = nullptr;
    offDigitalInputs = nullptr;
    offTouchProbeSatte = nullptr;
    offTouchProbePos1 = nullptr;
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

    ecrt_master_state(master, &state);
    nSlaves = state.slaves_responding;
    allSlavesSate = ALState(state.al_states);
    linkUp = state.link_up;

    for (int i = 0; i < nSlaves; i++)
    {
        ec_slave_info_t slaveInfo;
        ecrt_master_get_slave(master, i, &slaveInfo);

        ec_sync_info_t *syncs = new ec_sync_info_t[slaveInfo.sync_count];
        int nPdos = 0;
        for (int j = 0; j < slaveInfo.sync_count; j++)
        {
            int err = ecrt_master_get_sync_manager(master, i, j, syncs + j);
            if(err){
                cerr << "Master init failed, can't get the sync managers" << endl;
                delete[] syncs;
                return -1;
            }
            nPdos += syncs[j].n_pdos;
        }

        ec_pdo_info_t *pdos = new ec_pdo_info_t[nPdos];
        int pdosIndex = 0;
        int nEntries = 0;
        for (int j = 0; j < slaveInfo.sync_count; j++)
        {
            if(syncs[j].n_pdos > 0)
            {
                int err = ecrt_master_get_pdo(master, i, j, pdosIndex, pdos);
                if(err){
                    cerr << "Master init failed, can't get the pdos" << endl;
                    delete[] syncs;
                    delete[] pdos;
                    return -1;
                }
                nEntries += pdos[pdosIndex].n_entries;
                pdosIndex += syncs[j].n_pdos;
            }

        }

        ec_pdo_entry_info_t *entries = new ec_pdo_entry_info_t[nEntries];
        pdosIndex = 0;
        int entrysIndex = 0;
        for (int j = 0; j < slaveInfo.sync_count; j++)
        {
            if(syncs[j].n_pdos > 0)
            {
                int err = ecrt_master_get_pdo_entry(master, i, j, pdosIndex, entrysIndex, entries);
                if(err){
                    cerr << "Master finit failed, can't get the pdo entries" << endl;
                    delete[] syncs;
                    delete[] pdos;
                    delete[] entries;
                    return -1;
                }
                entrysIndex += pdos[pdosIndex].n_entries;
                pdosIndex += syncs[j].n_pdos;
            }
        }

        string slaveName(slaveInfo.name);
        Slave slavei(slaveName,
                     slaveInfo.alias,
                     slaveInfo.position,
                     slaveInfo.vendor_id,
                     slaveInfo.product_code,
                     nEntries,
                     nPdos,
                     slaveInfo.sync_count);

        slavei.setPdoEntries(entries);
        slavei.setPdos(pdos);
        slavei.setSyncManger(syncs);
        slave.push_back(slavei);

        delete[] syncs;
        delete[] pdos;
        delete[] entries;
    }


    offControlWord = new unsigned int[nSlaves];
    offTargetPosition = new unsigned int[nSlaves];
    offTargetVelocity = new unsigned int[nSlaves];
    offTargetTorque = new unsigned int[nSlaves];
    offTargetModeOP = new unsigned int[nSlaves];
    offDummyByte1 = new unsigned int[nSlaves];
    TouchProbeFunc = new unsigned int[nSlaves];

    offSatesWord = new unsigned int[nSlaves];
    offActualPosition = new unsigned int[nSlaves];
    offActualVelocity = new unsigned int[nSlaves];
    offActualTorque = new unsigned int[nSlaves];
    offActualModeOP = new unsigned int[nSlaves];
    offBummyByte2 = new unsigned int[nSlaves];
    offFollowError = new unsigned int[nSlaves];
    offDigitalInputs = new unsigned int[nSlaves];
    offTouchProbeSatte = new unsigned int[nSlaves];
    offTouchProbePos1 = new unsigned int[nSlaves];

    int totalEntries = 0;
    for (int i = 0; i < nSlaves; i++){
        totalEntries += slave[i].nEntries();
    }

    domainRegist = new ec_pdo_entry_reg_t[totalEntries+1];
    for (int i = 0; i < nSlaves; i++)
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
                offset = TouchProbeFunc + i;
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
                offset = offBummyByte2 + i;
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
    domainRegist[totalEntries] = {};

    domain = ecrt_master_create_domain(master);
    if(!domain){
        cerr << "Master construct failed, can't creat domain" << endl;
        initRes = -1;
        goto init_end;
    }

    for (int i = 0; i < nSlaves; i++){
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
        ecrt_slave_config_dc(slaveConfig[i], 0x0300, cycleTime, cycleTime / 2, 0, 0);
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

    cout << "active done! can start the cyclic function." << endl;
}




void EthercatMaster::clear() 
{
    slave.erase(slave.begin(), slave.end());
    if(!master){
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
    delete[] offDummyByte1;
    delete[] TouchProbeFunc;
    delete[] offSatesWord;
    delete[] offActualPosition;
    delete[] offActualVelocity;
    delete[] offActualTorque;
    delete[] offActualModeOP;
    delete[] offBummyByte2;
    delete[] offFollowError;
    delete[] offDigitalInputs;
    delete[] offTouchProbeSatte;
    delete[] offTouchProbePos1;

    offControlWord = nullptr;
    offTargetPosition = nullptr;
    offTargetVelocity = nullptr;
    offTargetTorque = nullptr;
    offTargetModeOP = nullptr;
    offDummyByte1 = nullptr;
    TouchProbeFunc = nullptr;
    offSatesWord = nullptr;
    offActualPosition = nullptr;
    offActualVelocity = nullptr;
    offActualTorque = nullptr;
    offActualModeOP = nullptr;
    offBummyByte2 = nullptr;
    offFollowError = nullptr;
    offDigitalInputs = nullptr;
    offTouchProbeSatte = nullptr;
    offTouchProbePos1 = nullptr;

}
