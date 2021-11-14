#ifndef PID_H
#define PID_H
 

#include "headfile.h"


typedef struct      //位置式pid
{
    float kp;
    float ki;
    float kd;
    float errorPrePre;
    float errorPre;
    float error;
}MotorPid;


typedef struct      //位置式pid
{
    float kp;
    float ki;
    float kd;
    float errorPrePre;
    float errorPre;
    float error;
	float out;
}ElcPid;

typedef struct      //位置式pid
{
    float kp1;
    float kp2;
    float kd;

}ServoPidNum;
typedef struct      //位置式pid
{
    float kp1Sum;
    float kp2Sum;
    float kpSum;
    float kdSum;
}ServoPidRepeat;

typedef struct      //位置式pid
{
	ServoPidNum normal;
	ServoPidNum circle;
	ServoPidNum use;
	ServoPidRepeat repeat;
}CamPid;

typedef struct      //位置式pid
{
	CamPid cam;
	ElcPid elc;
}ServoPid;

typedef struct      
{
    float Eas;   //隶属度a
	float Ebs;	//隶属度b
	float Eam;
	float Ebm;
	int   lis;   
	int   js;
	int   lim;
	int   jm;
}Fuzzy;

typedef struct      //位置式pid
{
    MotorPid leftMotor;
    MotorPid rightMotor;
    ServoPid servo;
	Fuzzy    fuzzy;
}Pid;
extern Pid pid;
extern void PidParamInit(void);


#endif
