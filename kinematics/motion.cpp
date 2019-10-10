#include "motion.h"
#include <iostream>

using std::cout;
using std::endl;

const double maxangle[6] =
{
	MAXANG1,
	MAXANG2,
	MAXANG3,
	MAXANG4,
	MAXANG5,
	MAXANG6
};

const double minangle[6] =
{
	MINANG1,
	MINANG2,
	MINANG3,
	MINANG4,
	MINANG5,
	MINANG6
};

const double maxvel[6] = 
{
	MAXVEL1,
	MAXVEL2,
	MAXVEL3,
	MAXVEL4,
	MAXVEL5,
	MAXVEL6
};

const double maxacc[6] = 
{
	MAXACC1,
	MAXACC2,
	MAXACC3,
	MAXACC4,
	MAXACC5,
	MAXACC6
};



int ptp(const joinpos_t & p0, const joinpos_t & pf, vectorj & vjoin, int a, int v, int f)
{
	cout << "caculating the ptp function ......\n";
	for(int i=0; i<6; i++)
	{
		if( (pf.joi[i]<minangle[i]) || (pf.joi[i]>maxangle[i]) )
		{
			cout << "err: the destination of joint " << i << " is out of range\n";
			return -1;
		}
	}


	int ivmax = 0, iamax = 0;
	double gapmaxv = 0, gapmaxa = 0;
	for(int i =0; i<6; i++)
	{
		if(abs(pf.joi[i]-p0.joi[i])/maxvel[i] > gapmaxv)
		{
			gapmaxv = abs(pf.joi[i]-p0.joi[i])/maxvel[i];
			ivmax = i;	//find which axis moves faster ralatively
		}
        if(abs(pf.joi[i]-p0.joi[i])/maxacc[i] > gapmaxa)
		{
			gapmaxa = abs(pf.joi[i]-p0.joi[i])/maxacc[i];
			iamax = i;	//find which axis accerlerate faster ralatively
		}
	}
	vectord namda = julipt(p0.joi[ivmax], 
                           pf.joi[ivmax],
    maxacc[iamax]*abs((pf.joi[ivmax]-p0.joi[ivmax])/(pf.joi[iamax]-p0.joi[iamax]))*a/100.0, 
                           maxvel[ivmax]*v/100.0,
                           f);

    for(unsigned int i=0; i<namda.size(); i++)
    {	
		joinpos_t jp;
        for(int j=0; j<6; j++)
        {	
            jp.joi[j] = p0.joi[j]+(pf.joi[j]-p0.joi[j])*namda[i];
        }
        vjoin.push_back(jp);
    }
    return 0;
}

int ptp(const joinpos_t & p0, const joinpos_t & pf, vectori & vinc, int a, int v, int f )
{
	cout << "caculating the ptp function ......\n";
	for(int i=0; i<6; i++)
	{
		if( (pf.joi[i]<minangle[i]) || (pf.joi[i]>maxangle[i]) )
		{
			cout << "err: the destination of joint " << i << " is out of range\n";
			return -1;
		}
	}
    
	int ivmax = 0, iamax = 0;
	double gapmaxv = 0, gapmaxa = 0;
	for(int i =0; i<6; i++)
	{
		if(abs(pf.joi[i]-p0.joi[i])/maxvel[i] > gapmaxv)
		{
			gapmaxv = abs(pf.joi[i]-p0.joi[i])/maxvel[i];
			ivmax = i;	//find which axis moves faster ralatively
		}
        if(abs(pf.joi[i]-p0.joi[i])/maxacc[i] > gapmaxa)
		{
			gapmaxa = abs(pf.joi[i]-p0.joi[i])/maxacc[i];
			iamax = i;	//find which axis accerlerate faster ralatively
		}
	}
	vectord namda = julipt(p0.joi[ivmax], 
                           pf.joi[ivmax],
    maxacc[iamax]*abs((pf.joi[ivmax]-p0.joi[ivmax])/(pf.joi[iamax]-p0.joi[iamax]))*a/100.0, 
                           maxvel[ivmax]*v/100.0,
                           f);

    for(unsigned int i=0; i<namda.size(); i++)
    {	
		joinpos_t jp;
        for(int j=0; j<6; j++)
        {	
            jp.joi[j] = p0.joi[j]+(pf.joi[j]-p0.joi[j])*namda[i];
        }
        vinc.push_back(jointangle2increment(jp));
    }
    return 0;
}


int ptp(const incpos_t & inc0, const incpos_t & incf, vectori & vinc, int a, int v, int f)
{
    cout << "caculating the ptp function ......\n";
    joinpos_t p0 = increment2jointangle(inc0);
    joinpos_t pf = increment2jointangle(incf);
	for(int i=0; i<6; i++)
	{
		if( (pf.joi[i]<minangle[i]) || (pf.joi[i]>maxangle[i]) )
		{
			cout << "err: the destination of joint " << i << " is out of range\n";
			return -1;
		}
	}
	int ivmax = 0, iamax = 0;
	double gapmaxv = 0, gapmaxa = 0;
	for(int i =0; i<6; i++)
	{
		if(abs(pf.joi[i]-p0.joi[i])/maxvel[i] > gapmaxv)
		{
			gapmaxv = abs(pf.joi[i]-p0.joi[i])/maxvel[i];
			ivmax = i;	//find which axis moves faster ralatively
		}
        if(abs(pf.joi[i]-p0.joi[i])/maxacc[i] > gapmaxa)
		{
			gapmaxa = abs(pf.joi[i]-p0.joi[i])/maxacc[i];
			iamax = i;	//find which axis accerlerate faster ralatively
		}
	}
	vectord namda = julipt( p0.joi[ivmax], 
                            pf.joi[ivmax], 
    maxacc[iamax]*abs((pf.joi[ivmax]-p0.joi[ivmax])/(pf.joi[iamax]-p0.joi[iamax]))*a/100.0, 
                            maxvel[ivmax]*v/100.0,
                            f);

    for(unsigned int i=0; i<namda.size(); i++)
    {	
		joinpos_t jp;
        for(int j=0; j<6; j++)
        {	
            jp.joi[j] = p0.joi[j]+(pf.joi[j]-p0.joi[j])*namda[i];
        }
        vinc.push_back(jointangle2increment(jp));
    }
    return 0;
}

int ptp( const joinpos_t & p0, const cartpos_t & cpf, vectori & vinc, int a, int v, int f )
{
    cout << "caculating the ptp function ......\n";
    joinpos_t pf;
    if(-1 == invkine(cpf, p0.joi, pf.joi))
	{
		cout << "inverse kinematics err\n";
		return -1;
	}

	for(int i=0; i<6; i++)
	{
		if( (pf.joi[i]<minangle[i]) || (pf.joi[i]>maxangle[i]) )
		{
			cout << "err: the destination of joint " << i << " is out of range\n";
			return -1;
		}
	}
	int ivmax = 0, iamax = 0;
	double gapmaxv = 0, gapmaxa = 0;
	for(int i =0; i<6; i++)
	{
		if(abs(pf.joi[i]-p0.joi[i])/maxvel[i] > gapmaxv)
		{
			gapmaxv = abs(pf.joi[i]-p0.joi[i])/maxvel[i];
			ivmax = i;	//find which axis moves faster ralatively
		}
        if(abs(pf.joi[i]-p0.joi[i])/maxacc[i] > gapmaxa)
		{
			gapmaxa = abs(pf.joi[i]-p0.joi[i])/maxacc[i];
			iamax = i;	//find which axis accerlerate faster ralatively
		}
	}
	vectord namda = julipt( p0.joi[ivmax], 
                            pf.joi[ivmax], 
    maxacc[iamax]*abs((pf.joi[ivmax]-p0.joi[ivmax])/(pf.joi[iamax]-p0.joi[iamax]))*a/100.0, 
                            maxvel[ivmax]*v/100.0,
                            f );

    for(unsigned int i=0; i<namda.size(); i++)
    {	
		joinpos_t jp;
        for(int j=0; j<6; j++)
        {	
            jp.joi[j] = p0.joi[j]+(pf.joi[j]-p0.joi[j])*namda[i];
        }
        vinc.push_back(jointangle2increment(jp));
    }
    return 0;
}


