
#include "motion.h"
#include <iostream>
using std::cout;

void faultreset(void)
{
    while(1)
    {
        if(0x08 == (ALstate & 0x08))    
            break;
        printf("Waiting for Ethercat communication to complete......\n");
        sleep(1);
    }
    while(1)
    {   //switch on disable
        if( ((StatusWord[0] & 0x004f) == 0x0040) && ((StatusWord[1] & 0x004f) == 0x0040) &&
            ((StatusWord[2] & 0x004f) == 0x0040) && ((StatusWord[3] & 0x004f) == 0x0040) &&
            ((StatusWord[4] & 0x004f) == 0x0040) && ((StatusWord[5] & 0x004f) == 0x0040) )
            break;
        for(int i=0; i<6; i++)
        {
            if( (StatusWord[i] & 0x0008) == 0x0008 )   ////fault
                ControlWord[i] = FAULT_RESET;
        }
        usleep(cycletime/1000);
    } 
}


void enable(void)
{
    while(1)
    {
        if(0x08 == (ALstate & 0x08))    
            break;
        printf("Waiting for Ethercat communication to complete......\n");
        sleep(1);
    }
    int isfault = 0;
    for(int i=0; i<6; i++)
    {
        if(FAULT == (StatusWord[i] & FAULT))
        {
            isfault = 1;
            cout << "There are faults in drivers " << i <<", can not enable\n";
            break;    
        }   
    }
    while( !isfault )
    {
        static int enable[6]={0};
        static int enableN = 0;
        for(int i=0; i<6; i++)
        {
            if( (StatusWord[i] & 0x006f) == 0x0027 )    // opreation enable
            {
                if(0 == enable[i])
                {
                    enable[i] = 1;
                    enableN += 1;
                }
                break;
            }
            else if( (StatusWord[i] & 0x004f) == 0x0040  ) //switch on disable
                ControlWord[i] = SHUDOWN;
            else if( (StatusWord[i] & 0x006f) == 0x0021  ) // read to switch on
                ControlWord[i] = SWITCH_ON;
            else if( (StatusWord[i] & 0x006f) == 0x0023  )   //switch on enable
                ControlWord[i] = ENABLE_OPERATION;
        } 
        usleep(cycletime/1000);      
    }
}

void shoutdown(void)
{
    while(1)
    {
        if(0x08 == (ALstate & 0x08))    
            break;
        printf("Waiting for Ethercat communication to complete......\n");
        sleep(1);
    }
    while(1)
    {
        if( ((StatusWord[0]&0x006f) == 0x0021) && ((StatusWord[1]&0x006f) == 0x0021) &&
            ((StatusWord[2]&0x006f) == 0x0021) && ((StatusWord[3]&0x006f) == 0x0021) &&
            ((StatusWord[4]&0x006f) == 0x0021) && ((StatusWord[5]&0x006f) == 0x0021) ) // ready to switch on
            break;
        for(int i=0; i<6; i++)
        {
            ControlWord[i] = SHUDOWN;
        }
        usleep(cycletime/1000);
    }
    
}