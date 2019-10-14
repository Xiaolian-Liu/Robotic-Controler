#ifndef ECATDRIVE_H
#define ECATDRIVE_H
#include <stdint.h> /*where int32_t is included  */

#define FREQUENCY 500
#define NSEC_PER_SEC (1000000000L)
#define PERIOD_NS (NSEC_PER_SEC / FREQUENCY)
#define CoolDrive      0x00000748, 0x00000000
#define NUMSl 6
// number of slaves
#define FILT_CNT               17
#define SYNC_MASTER_TO_REF        1
#define Kp  0.005
#define ki  0.0000001
#define kd  0.005

extern int run;
extern int istest;
extern unsigned short N;
// extern RT_TASK my_task;

extern uint8_t ALstate;
extern uint16_t ControlWord[6];
extern uint16_t StatusWord[6];
extern int8_t OperationMode[6];
extern int32_t ActualPosition[6];
extern int32_t TargetPosition[6];
extern int32_t *off_TarPosition;

typedef struct
{
    uint16_t alias; /**< Slave alias address. */
    uint16_t position; /**< Slave position. */
    uint32_t vendor_id; /**< Slave vendor ID. */
    uint32_t product_code;/**< Slave product code. */
}drive_t;

/* int ecat_init(void);
double sync_master_clock(void);
void rt_check_domain_state(ec_domain_t *domain)
void rt_check_master_state(ec_master_t *master);
void endsignal(int sig); */

void ecat_task(void *arg);

#endif