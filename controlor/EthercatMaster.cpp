#include "EthercatMaster.hpp"  
#include <iostream>
#include <string>
using std::cerr;
using std::cout;
using std::endl;
using std::string;

EthercatMaster::EthercatMaster(unsigned int index)
{
    master = ecrt_request_master(index);
    cout << "request master: " << index << endl;
    if(!master)
    {
        cerr << "request master: " << index << "failed, EtherCAT Master construct failed" << endl;
        return;
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
                cerr << "Master construct failed, can't get the sync managers" << endl;
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
                    cerr << "Master construct failed, can't get the pdos" << endl;
                }
                nEntries += pdos[index].n_entries;
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
                    cerr << "Master construct failed, can't get the pdo entries" << endl;
                }
                entrysIndex += pdos[index].n_entries;
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
    }

    int totalEntries = 0;
    for (int i = 0; i < nSlaves; i++){
        totalEntries += slave[i].nEntries();
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

    domainRegist = new ec_pdo_entry_reg_t[nSlaves * totalEntries];
    for (int i = 0; i < nSlaves; i++)
    {
        for (int j = 0; j < slave[i].nEntries(); j++)
        {
            unsigned int *offset;
            switch (slave[i].pdoEntry()[j].index)
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
                break;
            }
        }
    }
}

EthercatMaster::~EthercatMaster()
{
    ecrt_release_master(master);
}

