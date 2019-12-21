
#include <rtdk.h>
#include <native/queue.h>
#include <native/heap.h>
#include <native/task.h>
#include <commu.h>
#include "motion.h"
#include "circle.h"

const joinpos_t  home = {{0, 0 , 0, 0, 0, 0}};
const incpos_t  dabao = {-59471, 3373567, 3254210, 3424, 107103, 121782};
const joinpos_t dabao1 = {0, 54, -54, 0, 0, 0};
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



int ptp(const joinpos_t & p0, const joinpos_t & pf, const char * queue_name, int a, int v, int f)
{
	rt_printf("caculating the ptp function ......\n");
	for(int i=0; i<6; i++)
	{
		if( (pf.joi[i]<minangle[i]) || (pf.joi[i]>maxangle[i]) )
		{
			rt_printf("err: the destination of joint %d is out of range\n",i);
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
	double acc = maxacc[iamax]*abs((pf.joi[ivmax]-p0.joi[ivmax])/(pf.joi[iamax]-p0.joi[iamax]))*a/100.0;
	double vel = maxvel[ivmax]*v/100.0;
	rt_printf("p0: %f\n", p0.joi[ivmax]);
	rt_printf("pf: %f\n", pf.joi[ivmax]);
	rt_printf("pf: %f\n", acc);
	rt_printf("pf: %f\n", vel);
	rt_printf("pf: %d\n", f);

	vectord namda = julipt(p0.joi[ivmax], 
                           pf.joi[ivmax],
                           acc, 
                           vel,
                           f);
	RT_QUEUE tarpos;					   
	int err = rt_queue_bind(&tarpos, queue_name, TM_INFINITE);
	if(err < 0){
		rt_fprintf(stderr, "target position queue bind failed in motion.cpp:ptp() : %d", err);
	}
	incpos_t inchome = jointangle2increment(pf);
	for(int j=0; j<6; j++)
	{
		rt_printf("home[%d]: %d\n",j, inchome.inc[j]);
	}
    for(unsigned int i=0; i<namda.size(); i++)
    {	
		// rt_printf("%f\n", namda[i]);
		joinpos_t jp;
        for(int j=0; j<6; j++)
        {	
            jp.joi[j] = p0.joi[j]+(pf.joi[j]-p0.joi[j])*namda[i];
        }
        // vjoin.push_back(jp);
		incpos_t ip = jointangle2increment(jp);
		err = rt_queue_write(&tarpos, &ip, sizeof(incpos_t), Q_NORMAL);
		if(err < 0)
		{
			rt_fprintf(stderr, "target position queue write failed in motion.cpp:ptp() : %d", err);
			if(-ENOMEM == err)
			{
				rt_task_sleep(10000000);
				i--;
			}
		}	
    }
	err = rt_queue_unbind(&tarpos);
	if(err < 0){
		rt_fprintf(stderr, "target position queue unbind failed in motion.cpp:ptp() : %d", err);
	}
    return 0;
}
/* 
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
 */

void PTP(void *cookie)
{
	RT_HEAP heap_desc;
	RT_HEAP stat_heap;
	static void * data_sharm;
	static void * stat_sharm;
	driverdata_t * data;
	driverstate_t * state;
	rt_print_auto_init(1);
	rt_printf("Satrt PTP......\n");

	int err = rt_heap_bind(&heap_desc, DRIVE_DATA_HEAP_NAME, TM_INFINITE);
	if(err < 0){
		rt_printf("data heap bind fail in motion.cpp:PTP() : %d\n", err);
	}
	err = rt_heap_bind(&stat_heap, DRIVE_STATE_HEAP_NAME, TM_INFINITE);
		if(err < 0){
			rt_printf("state heap bind fail in motion.cpp:PTP() : %d\n", err);
		}
	
	incpos_t ip0;

	while(1)
	{
		err = rt_heap_alloc(&stat_heap, 0, TM_INFINITE, &stat_sharm);
    	if(err < 0){
			rt_fprintf(stderr, "state heap alloc faile in motion.cpp:PTP() : %d", err);
			continue;
		}

		state = (driverstate_t *) stat_sharm;

		if(1 == state->isEnable){
			err = rt_heap_free(&stat_heap, stat_sharm);
			if(err < 0){
				rt_fprintf(stderr, "state heap free fail in motion.cpp:PTP() : %d", err);
			}
			rt_printf("go to break\n");
			break;
		}

		rt_task_sleep(2000000000);

		err = rt_heap_free(&stat_heap, stat_sharm);
		if(err < 0){
			rt_fprintf(stderr, "state heap free fail in motion.cpp:PTP() : %d", err);
		}
	}
	
	err = rt_heap_alloc(&heap_desc, 0, TM_INFINITE, &data_sharm);
    if(err < 0){
		rt_fprintf(stderr, "data heap alloc faile in motion.cpp:PTP() : %d", err);
    }
	data = (driverdata_t *) data_sharm;
	for(int i=0; i<6; i++){
		ip0.inc[i] = data->ActualPosition[i];
		rt_printf("ip0.inc[%d] = %d\n", i, ip0.inc[i]);
	}
	err = rt_heap_free(&heap_desc, data_sharm);
	if(err < 0){
		rt_fprintf(stderr, "data heap free fail in motion.cpp:PTP() : %d", err);
	}
	joinpos_t ij0 = increment2jointangle(ip0);
	for(int i=0; i<6; i++){
		rt_printf("ij0.joi[%d] = %f\n", i, ij0.joi[i]);
	}
	/* joinpos_t ij1 = {{0,0,0,0,90,0}};
	if(ptp(ij0, ij1, TARPOS_QUEUE_NAME, 10, 10) < 0){
		rt_printf("PTP function failed\n");
	} */
	// cartpos_t cp1;
	// forkine(ij1.joi, &cp1);
	cartpos_t cp1;
	cp1.pe[0] = 1000; cp1.pe[1] = 500; cp1.pe[2] = 800;
	cp1.Rx0 = 140; cp1.Ry0 = 0; cp1.Rz0 = 0;
	joinpos_t jp1;
	invkine(cp1, ij0.joi, jp1.joi);
	if(ptp(ij0, jp1, TARPOS_QUEUE_NAME, 5, 5) < 0){
		rt_printf("PTP function failed\n");
	}
/* 
	cartpos_t cp2;
	cp2.pe[0] = cp1.pe[0];
	cp2.pe[1] = -1000;
	cp2.pe[2] = cp1.pe[2];
	cp2.Rx0 = cp1.Rx0;
	cp2.Ry0 = cp1.Ry0;
	cp2.Rz0 = cp1.Rz0;
	vectorangle jang;
	lintraj(jang, ij1, cp2, 20, 10);
*/

	point p0, pi, pf;
	p0 << 1000, 500, 800;
	pi << 1500, 0, 800;
	pf << 1000, -500, 800;
	Vector3d rpy0, rpyf;
	rpy0 << 140, 0, 0;
	rpyf << 220, 0, 0;
	seqJointVec jointangles;
	cirtraj(jointangles, rpy0, rpyf, p0, pi, pf, 5, 5);

	RT_QUEUE tarpos;					   
	err = rt_queue_bind(&tarpos, TARPOS_QUEUE_NAME, TM_INFINITE);
	if(err < 0){
		rt_fprintf(stderr, "target position queue bind failed in motion.cpp:ptp() : %d", err);
	}

	for(size_t i=0; i<jointangles.size(); i++)
	{
		DriveVec ip = Joint2Drive(jointangles[i]);
		incpos_t ipo;
		for(int j=0; j<6; j++)
		{
			ipo.inc[j] = ip[j];
		}
		err = rt_queue_write(&tarpos, &ipo, sizeof(incpos_t), Q_NORMAL);
		if(err < 0)
		{
			rt_fprintf(stderr, "target position queue write failed in motion.cpp:PTP() : %d", err);
			if(-ENOMEM == err){
				rt_task_sleep(10000000);
				i--;
			}
		}
	}
 
	cirtraj(jointangles, rpyf, rpy0, pf, pi, p0, 5, 5);
	for(size_t i=0; i<jointangles.size(); i++)
	{
		DriveVec ip = Joint2Drive(jointangles[i]);
		incpos_t ipo;
		for(int j=0; j<6; j++)
		{
			ipo.inc[j] = ip[j];
		}
		err = rt_queue_write(&tarpos, &ipo, sizeof(incpos_t), Q_NORMAL);
		if(err < 0)
		{
			rt_fprintf(stderr, "target position queue write failed in motion.cpp:PTP() : %d", err);
			if(-ENOMEM == err){
				rt_task_sleep(10000000);
				i--;
			}
		}
	}


/*
	for(unsigned int i=0; i<jang.size(); i++)
	{
		incpos_t ip = jointangle2increment(jang[i]);	
		err = rt_queue_write(&tarpos, &ip, sizeof(incpos_t), Q_NORMAL);
		if(err < 0){
			rt_fprintf(stderr, "target position queue write failed in motion.cpp:PTP() : %d", err);
			if(-ENOMEM == err){
				rt_task_sleep(10000000);
				i--;
			}
		}
	}
*/

	// err = rt_queue_unbind(&tarpos);
	// if(err < 0){
	// 	rt_fprintf(stderr, "target position queue unbind failed in motion.cpp:PTP() : %d", err);
	// }
/* 
	if(ptp(jang[jang.size()-1], ij0, TARPOS_QUEUE_NAME, 10, 10) < 0){
		rt_printf("PTP function failed\n");
	}
 */
	if(ptp(jp1, dabao1, TARPOS_QUEUE_NAME, 5, 5) < 0){
		rt_printf("PTP function failed\n");
	}

	rt_heap_unbind(&heap_desc);
	rt_heap_unbind(&stat_heap);
}
