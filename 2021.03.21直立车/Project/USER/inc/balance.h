#ifndef __BALANCE_H__
#define __BALANCE_H__

#define Zero_Angle  -59
#define Angle_coe   0.001059f                         //�Ƕ�ת��ϵ��0.002154581f 

#include "headfile.h"

typedef struct
{
    uint16 x;
    uint16 y;
} Site_t;

typedef struct
{
    float x;
    float y;
    float z;
} S_FLOAT_XYZ;

typedef struct
{
    int32 x;
    int32 y;
    int32 z;
} S_INT32_XYZ;

typedef struct
{
    int16 x;
    int16 y;
    int16 z;
} S_INT16_XYZ;

extern S_INT32_XYZ
Tar_Ang_Vel,	// Ŀ����ٶ�
Tar_Ang_Vel_Last;	// �ϴ�Ŀ����ٶ�

extern float Target_Angel_P;
extern float Attitude_Angel_P;        //��ǰpitch

extern char In_Roundabout_Flag3;

extern int32 Thressload_cnt;

extern char Communication_G_Flag;
extern char Communication_S_Flag;
extern int32 Communication_I;





extern int32 O_I;


void Balance_Control(void);
void Balance_Init(void);
void Communication(void);

extern char Speed_Flag, Angle_Flag, Ang_Velocity_Flag, Turn_Flag;
extern int32 Direct_Last, Direct_Parameter, Radius, Speed_Now, Speed_Now_Last, Speed_Min, Speed_Set;
extern int32 Theory_Duty;



#endif