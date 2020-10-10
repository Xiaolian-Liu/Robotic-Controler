#include "Slave.hpp"  
#include <string.h>
#include <iostream>
using std::dec;
using std::endl;
using std::hex;

Slave::Slave(const string &name,
             uint16_t alias,
             uint16_t position,
             uint32_t vendorId,
             uint32_t productCode,
             unsigned int nEntries,
             unsigned int nPdos,
             unsigned int nSyncMangers) : alias(alias),
                                          position(position),
                                          vendorId(vendorId),
                                          productCode(productCode),
                                          nPdoEntries(nEntries),
                                          nPdos(nPdos), 
                                          nSyncMangers(nSyncMangers)
{
    this->name = name;
    alState = INIT;

    pdoEntries = new ec_pdo_entry_info_t[nPdoEntries];
    pdos = new ec_pdo_info_t[nPdos];
    syncMangers = new ec_sync_info_t[nSyncMangers + 1];
}

Slave::Slave(const Slave &s) //must need, because destruct error!
{
    this->name = s.name;
    this->alias = s.alias;
    this->position = s.position;
    this->vendorId = s.vendorId;
    this->productCode = s.productCode;
    this->alState = s.alState;
    this->nPdoEntries = s.nPdoEntries;
    this->nPdos = s.nPdos;
    this->nSyncMangers = s.nSyncMangers;

    pdoEntries = new ec_pdo_entry_info_t[nPdoEntries];
    pdos = new ec_pdo_info_t[nPdos];
    syncMangers = new ec_sync_info_t[nSyncMangers + 1];

    setPdoEntries(s.pdoEntries);
    setPdos(s.pdos);
    setSyncManger(s.syncMangers);
}

Slave::~Slave()
{
    delete[] pdoEntries;
    delete[] pdos;
    delete[] syncMangers;
}

void Slave::setPdoEntries(const ec_pdo_entry_info_t *entries) 
{
    for (int i = 0; i < nPdoEntries; i++)
    {
        pdoEntries[i] = entries[i];
    }
}

void Slave::setPdos(const ec_pdo_info_t *pdos) 
{
    int entryIndex = 0;
    for (int i = 0; i < nPdos; i++)
    {
        this->pdos[i] = pdos[i];
        this->pdos[i].entries = this->pdoEntries + entryIndex;
        entryIndex += pdos[i].n_entries;
    }

}

void Slave::setSyncManger(const ec_sync_info_t *syncs) 
{
    int pdoIndex = 0;
    for (int i = 0; i < nSyncMangers; i++)
    {
        this->syncMangers[i] = syncs[i];
        if(0 == syncs[i].n_pdos){
            this->syncMangers[i].pdos = nullptr;
        }else
        {
            this->syncMangers[i].pdos = this->pdos + pdoIndex;
        }

        pdoIndex += this->syncMangers[i].n_pdos;
    }
    this->syncMangers[nSyncMangers] = {0xff};
}


void Slave::setAlState(ALState state)
{
    this->alState = state;
}

ALState Slave::Alstate() const
{
    return this->alState;
}

string Slave::Name() const
{
    return this->name;
}

uint16_t Slave::Alias() const
{
    return this->alias;
}

uint16_t Slave::Position() const
{
    return this->position;
}

uint32_t Slave::VenderId() const
{
    return this->vendorId;
}

uint32_t Slave::ProductCode() const
{
    return this->productCode;
}

unsigned int Slave::nEntries() const
{
    return this->nPdoEntries;
}

const ec_pdo_entry_info_t* Slave::pdoEntry() const
{
    return this->pdoEntries;
}

const ec_sync_info_t* Slave::syncManger() const
{
    return this->syncMangers;
}

void Slave::printEntries(std::ostream &os) 
{
    
}

std::ostream& operator<<(std::ostream &os, const Slave &s) 
{
    os << "name:\t\t\"" << s.name << "\"" << endl
       << "vender id:\t\t" << s.vendorId << endl
       << "product code:\t\t" << s.productCode << endl
       << "pdo entries:" << endl;
    for (int i = 0; i < s.nPdoEntries; i++)
    {
        os << "0x" << hex << s.pdoEntries[i].index << ",\t" << s.pdoEntries[i].subindex << ",\t" << dec << s.pdoEntries[i].bit_length << endl;
    }

    os << "pdos:" << endl;
    for (int i = 0; i < s.nPdos; i++)
    {
        os << "0x" << hex << s.pdos[i].index << ",\t" << dec << s.pdos[i].n_entries << endl;
    }

    os << "sync mangers:" << endl;
    for (int i = 0; i < s.nSyncMangers; i++)
    {
        os << i << "\t" << s.syncMangers[i].dir << "\t" <<  s.syncMangers[i].n_pdos << "\t" << s.syncMangers[i].watchdog_mode << endl;
    }
    return os;
}
