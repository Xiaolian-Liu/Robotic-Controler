#ifndef  COMMU_H
#define COMMU_H
/* that is the headers for communication data between 
tasks of Robotic controllor */

#define NUMSL   6
#include <stdint.h>


/* this the information about the heap */
typedef struct
{
    uint16_t    ControlWord[NUMSL];
    int32_t     TargetPosition[NUMSL];
    int32_t     TargetVelocity[NUMSL];
    int16_t     TargetTorque[NUMSL];
    int8_t      ModeOperation[NUMSL];

    uint16_t    StatusWord[NUMSL];
    int32_t     ActualPosition[NUMSL];
    int32_t     ActualVelocity[NUMSL];
    int16_t     ActualTorque[NUMSL];
    int8_t      CurrentMode[NUMSL];
    int32_t     ActualFollowErr[NUMSL];
    
}driverdata_t;
#define DRIVE_DATA_HEAP_NAME    "drivedata"


typedef struct
{
    unsigned int    WKC;
    uint8_t   WC_state;
    unsigned int    redundancy_active;

    unsigned int slaves_responding; /**< Sum of responding slaves on all
                                      Ethernet devices. */
    unsigned int al_states : 4; /**< Application-layer states of all slaves.
                                  The states are coded in the lower 4 bits.
                                  If a bit is set, it means that at least one
                                  slave in the bus is in the corresponding
                                  state:
                                  - Bit 0: \a INIT
                                  - Bit 1: \a PREOP
                                  - Bit 2: \a SAFEOP
                                  - Bit 3: \a OP */
    unsigned int link_up : 1; /**< \a true, if at least one Ethernet link is
                                up. */

    int8_t  stop;
}driverstate_t;
#define DRIVE_STATE_HEAP_NAME   "drivestate"


/* this is the information about the tarpos queue */

#define TARPOS_QUEUE_NAME   "targetposition"



#endif