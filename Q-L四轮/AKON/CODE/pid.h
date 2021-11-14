#ifndef PID_H
#define PID_H
 

#include "headfile.h"


typedef struct      //λ��ʽpid
{
    float kp;
    float ki;
    float kd;
    float errorPrePre;
    float errorPre;
    float error;
}MotorPid;


typedef struct      //λ��ʽpid
{
    float kp;
    float ki;
    float kd;
    float errorPrePre;
    float errorPre;
    float error;
	float out;
}ElcPid;

typedef struct      //λ��ʽpid
{
    float kp1;
    float kp2;
    float kd;

}ServoPidNum;
typedef struct      //λ��ʽpid
{
    float kp1Sum;
    float kp2Sum;
    float kpSum;
    float kdSum;
}ServoPidRepeat;

typedef struct      //λ��ʽpid
{
	ServoPidNum normal;
	ServoPidNum circle;
	ServoPidNum use;
	ServoPidRepeat repeat;
}CamPid;

typedef struct      //λ��ʽpid
{
	CamPid cam;
	ElcPid elc;
}ServoPid;

typedef struct      
{
    float Eas;   //������a
	float Ebs;	//������b
	float Eam;
	float Ebm;
	int   lis;   
	int   js;
	int   lim;
	int   jm;
}Fuzzy;

typedef struct      //λ��ʽpid
{
    MotorPid leftMotor;
    MotorPid rightMotor;
    ServoPid servo;
	Fuzzy    fuzzy;
}Pid;
extern Pid pid;
extern void PidParamInit(void);


#endif
