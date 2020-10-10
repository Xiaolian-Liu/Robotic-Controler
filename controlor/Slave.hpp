#ifndef __SLAVE_H__
#define __SLAVE_H__

// #define EC_MAX_RXPDO_ENTRIES_NUM	 (10)
// #define EC_MAX_TXPDO_ENTRIES_NUM	 (10)

#include <stdint.h>
#include <string>
#include <ecrt.h>
using std::string;

enum ALState
{
	INIT = 1,
	PREOP = 2,
	SAFOP = 4,
	OP = 8
};

class Slave  
{
	private:
		string name;	/**< Name of the slave. */
		uint16_t alias; /**< The slaves alias if not equal to 0. */
		uint16_t position; /**< Offset of the slave in the ring. */
		uint32_t vendorId; /**< Vendor-ID stored on the slave. */
		uint32_t productCode; /**< Product-Code stored on the slave. */
		ALState alState;

		ec_pdo_entry_info_t *pdoEntries;
		unsigned int nPdoEntries;
		ec_pdo_info_t *pdos;
		unsigned int nPdos;
		ec_sync_info_t *syncMangers;
		unsigned int nSyncMangers;

	public:
		Slave(const string &name,
			  uint16_t alias,
			  uint16_t position,
			  uint32_t vendorId,
			  uint32_t productCode,
			  unsigned int nEntries,
			  unsigned int nPdos,
			  unsigned int nSyncMangers);

		~Slave();
		void setPdoEntries(const ec_pdo_entry_info_t *entries);
		void setPdos(const ec_pdo_info_t *pdos);
		void setSyncManger(const ec_sync_info_t *syncs);
		void setAlState(ALState state);
		ALState Alstate() const;
		string Name() const;
		uint16_t Alias() const;
		uint16_t Position() const;
		uint32_t VenderId() const;
		uint32_t ProductCode() const;
		unsigned int nEntries() const;
		const ec_pdo_entry_info_t *pdoEntry() const;
		const ec_sync_info_t *syncManger() const;
};
#endif // __SLAVE_H__