#ifndef  COMMU_H
#define COMMU_H
/* that is the headers for communication data between 
tasks of Robotic controllor */

#define NUMSL   6
#include <stdint.h>

extern int run;

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
#define DRIVE_DATA_HEAP_NAME    "drive_data"

typedef struct
{
    unsigned int        WKC;
    uint8_t             WC_state;
    unsigned int        redundancy_active;

    unsigned int        slaves_responding; /**< Sum of responding slaves on all
                                      Ethernet devices. */
    uint8_t             al_states; /**< Application-layer states of all slaves.
                                  The states are coded in the lower 4 bits.
                                  If a bit is set, it means that at least one
                                  slave in the bus is in the corresponding
                                  state:
                                  - Bit 0: \a INIT
                                  - Bit 1: \a PREOP
                                  - Bit 2: \a SAFEOP
                                  - Bit 3: \a OP */
    uint8_t             link_up; /**< \a true, if at least one Ethernet link is
                                up. */
}driverstate_t;
#define DRIVE_STATE_HEAP_NAME   "drive_state"


/* this is the information about the tarpos queue */
#define TARPOS_QUEUE_NAME   "target_position"
#define TARPOS_QUEUE_SIZE   4*6*1024*1024


#define TIMELY_DATA_PIPE_NAME   "timely_data"
#define TIMELY_DATA_PIPE_SIZE   8*1024*1024

// void techview_link(void *cookie);

struct stat_bits
{
    unsigned long al_states : 4; 
/* - Bit 0: \a INIT - Bit 1: \a PREOP - Bit 2: \a SAFEOP - Bit 3: \a OP */
    unsigned long unused    : 8*sizeof(unsigned long) - 4;
};

typedef union
{
    unsigned long       mask;
    struct stat_bits    bit;
}stat_event;
#define STATE_EVENT_NAME   "state_event"


#endif