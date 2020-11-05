
#include <sys/msg.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include "motion.h"
#include "circle.h"
#include "transform.h"
#include "commu/ReceiveData.hpp"
#include "commu/TargetData.hpp"
#include "commu/PositionQueue.hpp"
#include "commu/StateData.hpp"

using std::ofstream;
extern int run;
extern int beEnable;

const joinpos_t home = {{0, 0, 0, 0, 0, 0}};
const incpos_t  dabao = {-59471, 3373567, 3254210, 3424, 107103, 121782};
const joinpos_t dabao1 = {0, 54, -54, 0, 0, 0};
const double maxangle[6] =
	{
		MAXANG1,
		MAXANG2,
		MAXANG3,
		MAXANG4,
		MAXANG5,
		MAXANG6};

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



int ptp(const joinpos_t & p0, const joinpos_t & pf, int a, int v, int f)
{
    printf("caculating the ptp function ......\n");
	for(int i=0; i<6; i++)
	{
		if( (pf.joi[i]<minangle[i]) || (pf.joi[i]>maxangle[i]) )
		{
            printf("err: the destination of joint %d is out of range\n",i);
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

	vectord namda = julipt(p0.joi[ivmax], 
                           pf.joi[ivmax],
                           acc, 
                           vel,
                           f);

    PositionQueue posqueue1;//
    posqueue1.init();//

	incpos_t inchome = jointangle2increment(pf);
	for(int j=0; j<6; j++)
	{
		printf("home[%d]: %d\n",j, inchome.inc[j]);
	}
    for(unsigned int i=0; i<namda.size(); i++)
    {	
        if(0 == i)
        {
            int b = 0;
        }

		// printf("%f\n", namda[i]);
		joinpos_t jp;
		ofstream of;
		of.open("angles.txt", std::ios::app);
        for(int j=0; j<6; j++)
        {	
            jp.joi[j] = p0.joi[j]+(pf.joi[j]-p0.joi[j])*namda[i];
			of << jp.joi[j] << " ";
        }
		of << std::endl;

        // vjoin.push_back(jp);
		incpos_t ip = jointangle2increment(jp);
		incPos_t iP;
		for (int i = 0; i < 6; i++)
		{
			iP.targetPosition[i] = ip.inc[i];
		}

        if(i == namda.size()-1)
        {
            int a = 0;
        }
        int sendRes = posqueue1.sendPosition(iP);
        if(-1 == sendRes)
        {
            perror("sendPosition error:");
        }
	}
    return 0;
}


int ptp(seqJointVec & jangle, const JointVec & p0, const JointVec & pf, int a, int v, int f)
{
	jangle.clear();
	for(int i=0; i<6; i++)
	{
		if( (pf[i]<minangle[i]) || (pf[i]>maxangle[i]) )
		{
            printf("err: the destination of joint %d is out of range\n",i);
			return -1;
		}
	}

	int ivmax = 0, iamax = 0;
	double gapmaxv = 0, gapmaxa = 0;
	for(int i =0; i<6; i++)
	{
		if(abs(pf[i]-p0[i])/maxvel[i] > gapmaxv)
		{
			gapmaxv = abs(pf[i]-p0[i])/maxvel[i];
			ivmax = i;	//find which axis moves faster ralatively
		}
        if(abs(pf[i]-p0[i])/maxacc[i] > gapmaxa)
		{
			gapmaxa = abs(pf[i]-p0[i])/maxacc[i];
			iamax = i;	//find which axis accerlerate faster ralatively
		}
	}
	double acc = maxacc[iamax]*abs((pf[ivmax]-p0[ivmax])/(pf[iamax]-p0[iamax]))*a/100.0;
	double vel = maxvel[ivmax]*v/100.0;

	vectord s, sd, sdd;
	ulspb(s, sd, sdd, p0[ivmax], pf[ivmax], acc, vel, f);
	size_t N = s.size();
	jangle.resize(N);
    for (size_t i = 0; i < N; i++)
	{
		jangle[i] = p0 + s[i]*(pf - p0);
	}
	return 0;
}

int lin(seqJointVec & jangle, CartPose p0, CartPose pf, int a, int v, int f)
{
	jangle.clear();
	vecHTransform T;
	lintraj(T, p0, pf, a, v, f);
	size_t N = T.size();
	jangle.resize(N);
	JointVec lastangle;
    lastangle << 0, 0, 0, 0, 0, -140;
	invkine(jangle[0], lastangle, p0);

	for (size_t i = 1; i < N; i++)
	{
		invkine(jangle[i], jangle[i - 1], T[i]);
	}
	return 0;
}

int arc(seqJointVec & jangle, Matrix4d T0, Matrix4d Ti, Matrix4d Tf, int a, int v, int f)
{
	jangle.clear();
	vecHTransform T;
	arctraj(T, T0, Ti, Tf, a, v, f);
	size_t N = T.size();
	jangle.resize(N);
	JointVec lastangle;
	lastangle << 0, 0, 0, 0, 0, 0;
	invkine(jangle[0], lastangle, T[0]);

	for (size_t i = 1; i < N; i++)
	{
		invkine(jangle[i], jangle[i - 1], T[i]);
	}
	return 0;
}

int cir(seqJointVec & jangle, Matrix4d T0, Matrix4d Ti, Matrix4d Tf, int a, int v, int f)
{
	jangle.clear();
	vecHTransform T;
	circletraj(T, T0, Ti, Tf, a, v, f);
	size_t N = T.size();
	jangle.resize(N);
	JointVec lastangle;
	lastangle << 0, 0, 0, 0, 0, 0;
	invkine(jangle[0], lastangle, T[0]);

	for (size_t i = 1; i < N; i++)
	{
		invkine(jangle[i], jangle[i - 1], T[i]);
	}
	return 0;
}


void * PTP(void *cookie)
{
    printf("PTP/n");
	PositionQueue posqueue;
	posqueue.init();

	printf("Satrt PTP......\n");

	incpos_t ip0;

    while(run)
	{
		stateData_t sdata = StateData::getData();
        if (1 == beEnable)
		{
			printf("go to break\n");
			break;
		}
        printf("Enable ID: %d\n", StateData::data.isEnable);
        printf("Enable ID: %d\n", beEnable);
		sleep(2);
	}
	
	receiveData_t rdata = ReceiveData::getData();
	for (int i = 0; i < 6; i++)
	{
		ip0.inc[i] = rdata.actualPosition[i];
		printf("ip0.inc[%d] = %d\n", i, ip0.inc[i]);
	}

	joinpos_t ij0 = increment2jointangle(ip0);
	for(int i=0; i<6; i++){
		printf("ij0.joi[%d] = %f\n", i, ij0.joi[i]);
	}


	seqJointVec angles;

	JointVec vij0;
	vij0 << ij0.joi[0], ij0.joi[1], ij0.joi[2], ij0.joi[3], ij0.joi[4], ij0.joi[5];
    joinpos_t ijhome = {{0, 0, 0, 0, 0, 0}};
	JointVec vijhome;
    vijhome << 0, 0, 0, 0, -90, 0;
    ptp(angles, vij0, vijhome, 5, 10,200);

	ofstream of;		//
	of.open("angles.txt");//

	for(size_t i=0; i<angles.size(); i++)
	{
		for (int j = 0; j < 6; j++)//
		{
			of << angles[i][j] << " ";
		}
		of << std::endl;//

		DriveVec ip = Joint2Drive(angles[i]);
		incPos_t iP;
		for(int j=0; j<6; j++)
		{
			iP.targetPosition[j] = ip[j];
		}
		posqueue.sendPosition(iP);
	}
	std::cout << "ptp:j0->jhome succeed!\n";

	// if(ptp(ij0, ijhome, 30, 15) < 0){
	// 	printf("PTP function failed\n");
	// }

	joinpos_t jps;
	cartpos_t cps;
	cps.pe[0] = 1000; cps.pe[1] = 100; cps.pe[2] = 630;
	cps.Rx0 = -180; cps.Ry0 = 0; cps.Rz0 = 0;
	invkine(cps, ijhome.joi, jps.joi);


	JointVec vjps;
	vjps << jps.joi[0], jps.joi[1], jps.joi[2], jps.joi[3], jps.joi[4], jps.joi[5];
    ptp(angles, vijhome, vjps, 15, 15,200);
	for(size_t i=0; i<angles.size(); i++)
	{
		for (int j = 0; j < 6; j++)//
		{
			of << angles[i][j] << " ";
		}
		of << std::endl;//

		DriveVec ip = Joint2Drive(angles[i]);
		incPos_t iP;
		for(int j=0; j<6; j++)
		{
			iP.targetPosition[j] = ip[j];
		}
		posqueue.sendPosition(iP);
	}
	std::cout << "ptp:jhome->js succeed!\n";

    ptp(angles, vjps, vijhome, 5, 10,200);
    for(size_t i=0; i<angles.size(); i++)
    {
        for (int j = 0; j < 6; j++)//
        {
            of << angles[i][j] << " ";
        }
        of << std::endl;//

        DriveVec ip = Joint2Drive(angles[i]);
        incPos_t iP;
        for(int j=0; j<6; j++)
        {
            iP.targetPosition[j] = ip[j];
        }
        posqueue.sendPosition(iP);
    }
    std::cout << "ptp:js->jhome succeed!\n";


    JointVec vdabao1;
    vdabao1 << 0, 54, -54, 0, 0, 0;
    ptp(angles, vijhome, vdabao1, 5, 10,200);
    for(size_t i=0; i<angles.size(); i++)
    {
        for (int j = 0; j < 6; j++)//
        {
            of << angles[i][j] << " ";
        }
        of << std::endl;//

        DriveVec ip = Joint2Drive(angles[i]);
        incPos_t iP;
        for(int j=0; j<6; j++)
        {
            iP.targetPosition[j] = ip[j];
        }
        posqueue.sendPosition(iP);
    }
    std::cout << "ptp:js->jhome succeed!\n";


    return NULL;



    CartPose ps, p0, pi, pf;
	ps.pe << 1000, 100, cps.pe[2];
	ps.rpy << -180, 0, 0;

	p0.pe << 1000, 100, 586;
	p0.rpy << -180, 0, 0;

	pi.pe << 1100, 0, 586;
	pi.rpy << -180, 0, 0;

	pf.pe << 1000, -100, 586;
	pf.rpy << -180, 0, 0;



/*
	lin(angles, ps, p0, 5, 5);
	for(size_t i=0; i<angles.size(); i++)
	{
		DriveVec ip = Joint2Drive(angles[i]);
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

	lin(angles, p0, pi, 20, 10);
	for(size_t i=0; i<angles.size(); i++)
	{
		DriveVec ip = Joint2Drive(angles[i]);
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


	lin(angles, pi, pf, 20, 10);
	for(size_t i=0; i<angles.size(); i++)
	{
		DriveVec ip = Joint2Drive(angles[i]);
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

	lin(angles, pf, p0, 20, 10);
	for(size_t i=0; i<angles.size(); i++)
	{
		DriveVec ip = Joint2Drive(angles[i]);
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

	cir(angles, T0, Ti, Tf, 20, 20);
	for(size_t i=0; i<angles.size(); i++)
	{
		DriveVec ip = Joint2Drive(angles[i]);
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

*/
	p0.rpy << -220, 0, 0;
	pi.rpy << -180, 0, 0;
	pf.rpy << -140, 0, 0;
	Matrix3d R0, Ri, Rf;
	Matrix4d T0, Ti, Tf;
	R0 = rpy2ro(p0.rpy, 'd');
	Ri = rpy2ro(pi.rpy, 'd');
	Rf = rpy2ro(pf.rpy, 'd');
	T0 = transl(p0.pe, R0);
	Ti = transl(pi.pe, Ri);
	Tf = transl(pf.pe, Rf);

    lin(angles, ps, p0, 5, 5,200);

	for(size_t i=0; i<angles.size(); i++)
	{
		for (int j = 0; j < 6; j++)//
		{
			of << angles[i][j] << " ";
		}
		of << std::endl;//

		DriveVec ip = Joint2Drive(angles[i]);
		incPos_t iP;
		for(int j=0; j<6; j++)
		{
			iP.targetPosition[j] = ip[j];
		}
		posqueue.sendPosition(iP);
	}
	std::cout << "lin:ps->p0 succeed!\n";

    lin(angles, p0, pf, 5, 5,200);
	for(size_t i=0; i<angles.size(); i++)
	{
		for (int j = 0; j < 6; j++)//
		{
			of << angles[i][j] << " ";
		}
		of << std::endl;//

		DriveVec ip = Joint2Drive(angles[i]);
		incPos_t iP;
		for(int j=0; j<6; j++)
		{
			iP.targetPosition[j] = ip[j];
		}
		posqueue.sendPosition(iP);
	}
	std::cout << "lin:p0->pf succeed!\n";

	Rf = rpy2ro(pf.rpy, 'd');
	Vector3d kb, k;
	kb << 0, 0, 1;
	k = Rf.inverse()*kb;
	Matrix3d Rf0 = angvec2r(PI, k);
	R0 = Rf * Rf0;
	Ri = Rf * angvec2r(PI_2, k);
	T0 = transl(p0.pe, R0);
	Ti = transl(pi.pe, Ri);
	Tf = transl(pf.pe, Rf);
//	arc(angles, Tf, Ti, T0, 5, 3);

    lin(angles, pf, p0, 5, 5,200);
	for(size_t i=0; i<angles.size(); i++)
	{
		for (int j = 0; j < 6; j++)//
		{
			of << angles[i][j] << " ";
		}
		of << std::endl;//


		DriveVec ip = Joint2Drive(angles[i]);
		incPos_t ipo;
		for(int j=0; j<6; j++)
		{
			ipo.targetPosition[j] = ip[j];
		}
		posqueue.sendPosition(ipo);
	}
	std::cout << "lin:Tf->T0 succeed!\n";

    lin(angles, p0, ps, 5, 5,200);
	for(size_t i=0; i<angles.size(); i++)
	{
		for (int j = 0; j < 6; j++)//
		{
			of << angles[i][j] << " ";
		}
		of << std::endl;//

		DriveVec ip = Joint2Drive(angles[i]);
		incPos_t ipo;
		for(int j=0; j<6; j++)
		{
			ipo.targetPosition[j] = ip[j];
		}
		posqueue.sendPosition(ipo);
	}
	std::cout << "lin:p0->ps succeed!\n";


	// if(ptp(jps, ijhome, 10, 15) < 0){
	// 	printf("PTP function failed\n");
	// }
    ptp(angles, vjps, vijhome, 30, 15,200);
	for(size_t i=0; i<angles.size(); i++)
	{
		for (int j = 0; j < 6; j++)//
		{
			of << angles[i][j] << " ";
		}
		of << std::endl;//

		DriveVec ip = Joint2Drive(angles[i]);
		incPos_t ipo;
		for(int j=0; j<6; j++)
		{
			ipo.targetPosition[j] = ip[j];
		}
		posqueue.sendPosition(ipo);
	}
	std::cout << "lin:ps->phome succeed!\n";



	// if(ptp(ijhome, dabao1, 30, 15) < 0){
	// 	printf("PTP function failed\n");
	// }
    ptp(angles, vijhome, vdabao1, 30, 15,200);
	for(size_t i=0; i<angles.size(); i++)
	{
		for (int j = 0; j < 6; j++)//
		{
			of << angles[i][j] << " ";
		}
		of << std::endl;//

		DriveVec ip = Joint2Drive(angles[i]);
		incPos_t ipo;
		for(int j=0; j<6; j++)
		{
			ipo.targetPosition[j] = ip[j];
		}
		posqueue.sendPosition(ipo);
	}
	std::cout << "lin:phome->pdabao succeed!\n";

	return NULL;
}
