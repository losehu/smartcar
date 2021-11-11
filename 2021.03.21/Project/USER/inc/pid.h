#ifndef __PID_H__
#define __PID_H__
#include "headfile.h"

#define KP 0
#define KI 1
#define KD 2
#define KT 3
#define KB 4
#define KF 5

typedef struct
{
    float X;
    float Y;
    float Z;
} S;
typedef struct PID
{
    float SumError;	//����ۼ�
    int32 LastError;	//Error[-1]
    int32 PrevError;	//Error[-2]
    int32 LastData;	//Speed[-1]
} PID;

extern PID Angle_PID, Ang_Vel_PID, Direct_PID, Turn_PID, MOTOR_PID;
extern float Angle[4], Ang_Vel[4], Direct[4], Turn[5][4], MOTOR[4];

void PID_Parameter_Init(PID *sptr);
// λ��ʽPID����
int32 PID_Realize1(PID *sptr, float *PID, int32 NowData, int32 Point);
int32 PID_Realize2(PID *sptr, float *PID, int32 NowData, int32 Point);
int32 PID_Realize3(PID *sptr, float *PID, int32 NowData, int32 Point);
int32 PID_Increase(PID *sptr, float *PID, int32 NowData, int32 Point);
int32 PlacePID_Control(PID *sprt, float *PID, int NowPiont, int SetPoint);
extern float Kp;
extern int32 iError,	//��ǰ���
       Actual;	//���ó���ʵ�����ֵ

extern float temp;
#endif