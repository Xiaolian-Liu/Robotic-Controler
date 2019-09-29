#ifndef ECATDRIVE_H
#define ECATDRIVE_H

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <malloc.h>
#include <math.h>

#include <rtdm/rtdm.h>
#include <native/task.h>
#include <native/sem.h>
#include <native/mutex.h>
#include <native/timer.h>
#include <rtdk.h>
#include <pthread.h>
#include "ecrt.h"

#define FREQUENCY 500
#define NSEC_PER_SEC (1000000000L)
#define PERIOD_NS (NSEC_PER_SEC / FREQUENCY)
#define CoolDrive      0x00000748, 0x00000000

#define Kp  0.05
#define ki
#define kd

extern int run;
extern long cycletime;
extern RT_TASK my_task;

extern uint8_t ALstate;
extern uint16_t ControlWord[6];
extern uint16_t StatusWord[6];
extern int8_t OperationMode[6];

typedef struct
{
    uint16_t alias; /**< Slave alias address. */
    uint16_t position; /**< Slave position. */
    uint32_t vendor_id; /**< Slave vendor ID. */
    uint32_t product_code;/**< Slave product code. */
}drive_t;

int ecat_init(void);
void ecat_task(void *arg);
double sync_master_clock(void);
void rt_check_domain_state(ec_domain_t *domain);
void rt_check_master_state(ec_master_t *master);
void endsignal(int sig);

#endif