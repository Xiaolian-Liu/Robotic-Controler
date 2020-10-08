#include "Slave.hpp"  
#include <string.h>

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
