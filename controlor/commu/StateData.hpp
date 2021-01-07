
#pragma once
#include <stdint.h>
#include <pthread.h>
#include <ecrt.h>

struct stateData_t
{
	unsigned int        WKC;
    ec_wc_state_t       WC_state;
    unsigned int        redundancy_active;

    unsigned int        slaves_responding; /**< Sum of responding slaves on all
                                      Ethernet devices. */
    unsigned int        al_states; /**< Application-layer states of all slaves.
                                  The states are coded in the lower 4 bits.
                                  If a bit is set, it means that at least one
                                  slave in the bus is in the corresponding
                                  state:
                                  - Bit 0: \a INIT
                                  - Bit 1: \a PREOP
                                  - Bit 2: \a SAFEOP
                                  - Bit 3: \a OP */
    unsigned int        link_up; /**< \a true, if at least one Ethernet link is
                                up. */
    bool                isEnable;
    bool                isErrExist;
};

class StateData
{
	private:
        static pthread_mutex_t mutex;

    public:
        static stateData_t data;
        StateData();
		static stateData_t getData();
		static void writeData(const stateData_t &data);
        ~StateData();
};
