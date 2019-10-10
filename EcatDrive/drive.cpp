
#include "drive.h"
#include <iostream>
using std::cout;
using std::endl;

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
        TargetPosition[i] = ActualPosition[i];
        if(FAULT == (StatusWord[i] & FAULT))
        {
            isfault = 1;
            cout << "There are faults in drivers " << i <<", can not enable\n";
            break;    
        }   
    }

    int enable[6]={0,0,0,0,0,0};
    int enableN = 0;
    while( !isfault )
    {
        for(int i=0; i<6; i++)
        {
            if( (StatusWord[i] & 0x006f) == 0x0027 )    // opreation enable
            {
                if(0 == enable[i])
                {
                    enable[i] = 1;
                    enableN += 1;
                    cout << "Driver " << i << " is enabled" << endl;
                }
            }
            else if( (StatusWord[i] & 0x004f) == 0x0040  ) //switch on disable
                ControlWord[i] = SHUDOWN;
            else if( (StatusWord[i] & 0x006f) == 0x0021  ) // read to switch on
                ControlWord[i] = SWITCH_ON;
            else if( (StatusWord[i] & 0x006f) == 0x0023  )   //switch on enable
                ControlWord[i] = ENABLE_OPERATION;
        } 
        if(6 == enableN)
        {
            cout << "6 Drivers are enabled" << endl;
            break;
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

/*  void test(int32_t a, int32_t v, int32_t x)    
    In this place, double is first considered, because 
    when we caculate, when we multiply, when we divide,
    the double data is more suitable       */
void test(double a, double v, double x)
{
    for(int j=0; j<6; j++)
        TargetPosition[j] = ActualPosition[j];
    double T = 1.0/FREQUENCY;
    if(double(v*v)/a < x)
    {
        double t1 = double(v)/a;
        double t2 = double(x)/v;
        double t3 = t2 + 2*t1;
        double t4 = t3 + 2*double(x)/v - double(v)/a;
        double t5 = t4 + 2*double(v)/a;
        double t6 = t5 + double(x)/v - double(v)/a;
        double tf = t6 + double(v)/a;
        N = tf*FREQUENCY+1;
        off_TarPosition = new int32_t [N];
        for(unsigned short i=0; i<N; i++)
        {
            double t = i*T;
            if(t< t1)
            {
                off_TarPosition[i] = a*t*t/2.0;
            }
            else if((t >= t1) && (t < t2))
            {
                off_TarPosition[i] = (v*v/2.0)/a + v*(t - t1);
                /*  in this place, the (v*v/2.0) may shrink due to
                **  v*v storing in int type. if v*v is biger than 2147483647,
                we can't get the right value, so the best way is use the double
                type.
                */
            }
            else if((t >= t2) && (t < t3))
            {
                off_TarPosition[i] = x - a*(t-(t2+t3)/2)*(t-(t2+t3)/2)/2;
            }
            else if((t >= t3) && (t < t4))
            {
                off_TarPosition[i] = v * ((t3+t4)/2 - t);
            }
            else if((t >= t4) && (t < t5))
            {
                off_TarPosition[i] = -x + a*(t-(t4+t5)/2)*(t-(t4+t5)/2)/2;
            }
            else if((t >= t5) && (t < t6))
            {
                off_TarPosition[i] = -(v*v/2.0)/a + v*(t - t6);
            }
            else
            {
                off_TarPosition[i] = -a*(t-tf)*(t-tf)/2.0;
            }
        }
    }
    else if((x<=double(v*v)/a) && (x>double(v*v/2)/a))
    {
        double t1 = sqrt(double(x)/a);
        double t2 = 2*t1;
        double t3 = t2 + double(v)/a;
        double t4 = t3 + 2*(double(x)- double(v*v/2)/a) / v;
        double t5 = t4 + double(v)/a;
        double t6 = t5 + sqrt(double(x)/a);
        double tf = t6 + sqrt(double(x)/a);
        N = tf*FREQUENCY+1;
        off_TarPosition = new int32_t [N];
        for(unsigned short i=0; i<N; i++)
        {
            double t = i*T;
            if(t< t1)
            {
                off_TarPosition[i] = a*t*t/2.0;
            }
            else if((t >= t1) && (t < t2))
            {
                off_TarPosition[i] = x - a*(t2-t)*(t2-t)/2.0;
            }
            else if((t >= t2) && (t < t3))
            {
                off_TarPosition[i] = x - a*(t-t2)*(t-t2)/2.0;
            }
            else if((t >= t3) && (t < t4))
            {
                off_TarPosition[i] = v * ((t3+t4)/2 - t);
            }
            else if((t >= t4) && (t < t5))
            {
                off_TarPosition[i] = -x + a*(t5-t)*(t5-t)/2;
            }
            else if((t >= t5) && (t < t6))
            {
                off_TarPosition[i] = -x + a*(t-t5)*(t-t5)/2;
            }
            else
            {
                off_TarPosition[i] = -a*(tf-t)*(tf-t)/2.0;
            }
        }
    }
    else
    {
        double t1 = sqrt(double(x)/a);
        double t2 = 2*t1;
        double t3 = t2 + sqrt(double(2*x)/a);
        double t4 = t3 + sqrt(double(2*x)/a);
        double t5 = t4 + sqrt(double(x)/a);
        double tf = t5 + sqrt(double(x)/a);
        N = tf*FREQUENCY+1;
        off_TarPosition = new int32_t [N];
        for(unsigned short i=0; i<N; i++)
        {
            double t = i*T;
            if(t< t1)
            {
                off_TarPosition[i] = a*t*t/2.0;
            }
            else if((t >= t1) && (t < t2))
            {
                off_TarPosition[i] = x - a*(t2-t)*(t2-t)/2.0;
            }
            else if((t >= t2) && (t < t3))
            {
                off_TarPosition[i] = x - a*(t-t2)*(t-t2)/2.0;
            }
            else if((t >= t3) && (t < t4))
            {
                off_TarPosition[i] = -x + a*(t4-t)*(t4-t)/2;
            }
            else if((t >= t4) && (t < t5))
            {
                off_TarPosition[i] = -x + a*(t-t4)*(t-t4)/2;
            }
            else
            {
                off_TarPosition[i] = -a*(tf-t)*(tf-t)/2;
            }
        }
    }
    istest = 1;
    cout << istest << std::endl;
    sleep(120);
    istest = 0;
    usleep(2*cycletime/1000);
    delete [] off_TarPosition;
}