#ifndef _PID_H_
#define _PID_H_

#include "headfile.h"

#define KP 0
#define KI 1
#define KD 2
#define KT 3
#define KB 4
#define KF 5
extern float BalancePIDParam[][7];
extern float BalancePIDParam_angle_v[][7];

extern float SpeedPIDParam[][5];
extern float LeftMotorSpeedPIDParam[1][7];
extern float RightMotorSpeedPIDParam[1][7];
extern float ServoPIDParam[][8];
extern float TurnPIDParam[][9];


//普通速度环 基本速度控制
//简易增量式PID
typedef struct
{
    int16 exval;
    int16 nowval;
    float kp;
    float ki;
    float kd;
    int16 nowError;
    float increase;
    int16 lastError;
    int16 beforeError;
    int16 out;
    float maxout;
    float minout;
} SpeedPIDTypedef;

extern SpeedPIDTypedef SpeedPID, SpeedPIDLeft, SpeedPIDRight;
int16 SimpleIncremental_PID(float *pid_param, SpeedPIDTypedef *pid, int16 _exval, int16 _nowval);


//直立环控制

typedef struct PID
{
    float SumError;   //误差累计
    int32 LastError;  //Error[-1]
    int32 PrevError;  //Error[-2]
    int32 LastData;   //Speed[-1]
} PID;

extern PID Angle_PID, Ang_Vel_PID;
extern float Angle[4], Ang_Vel[4];
//转向环 非线性位置式pid 用于摄像头控制
typedef struct
{
    int16 exval;
    int16 nowval;
    float kp;
    float kp1;
    float kp2;
    float kp2Real;
    float kp2_K;
    float kp3;
    float ki;
    float kd;
    int16 nowError;
    int16 lastError;
    int16 beforeError;
    int32 sumError;
    float sumErrormax;
    float sumErrormin;
    float Ud;//当前微分值
    float Ud_last;//上一次微分值
    float Ud_a;//滤波系数
    int16 out;
    float maxout;
    float minout;
} TurnLoopCamTypedef;

extern TurnLoopCamTypedef TurnLoopCam;
int16 Position_PID(float *pid_param, TurnLoopCamTypedef *pid, int16 _exval, int16 _nowval);

float Float_SimpleIncremental_PID(float *pid_param, TurnLoopCamTypedef *pid, float _exval, float _nowval);

//function
void  PID_Init(void);




//----------------------------------------模糊PID------------------------------------------//
/*定义结构体和公用体*/
typedef struct
{
    float nowval;                 /*当前值*/
    float exval;                  /*期望值*/
    float nowError;               /*当前偏差*/
    float E_Quantized;            /*量化后的误差*/
    int   E_Index[2];             /*隶属区间*/
    float E_Membership[2];        /*隶属度*/
    float ErrorInc;               /*偏差增量*/
    float EC_Quantized;           /*量化后的误差变化*/
    int   EC_Index[2];            /*隶属区间*/
    float EC_Membership[2];       /*隶属度*/

    float kp1;                    /*比例系数*/
    float ki1;                    /*积分系数*/
    float kd1;                    /*微分系数*/
    float kp2;                    /*比例系数*/
    float ki2;                    /*积分系数*/
    float kd2;                    /*微分系数*/
    float kp2k;                   /*kp2模糊增益系数*/
    float ki2k;                   /*ki2模糊增益系数*/
    float kd2k;                   /*kd2模糊增益系数*/
    float kp;                    /*比例系数*/
    float ki;                    /*积分系数*/
    float kd;                    /*微分系数*/
    float sumError;               /*积分和*/
    float sumErrormax;            /*上限*/
    float sumErrormin;            /*下限*/

    float nowErrmax;              /*上限*/
    float nowErrmin;              /*下限*/
    float lastError;              /*前一拍偏差*/
    float preError;               /*前两拍偏差*/

    float ECmax;                  /*上限*/
    float ECmin;                  /*下限*/

    float out;                    /*输出值*/
    float outmax;                 /*输出值的上限*/
    float outmin;                 /*输出值的下限*/
} FuzzyPID_Typedef;

extern  FuzzyPID_Typedef  AngleLoop_Fuzzy;
int PID_Realize2(PID *sptr, float *PID, int NowData, int Point);

int PID_Increase(PID *sptr, float *PID, int NowData, int Point);

//
void FuzzyPID_Init(void);
void Fuzzy_PID_Test(void);
float FuzzyPID(FuzzyPID_Typedef *pid, float _nowval, float _exval);
#endif

