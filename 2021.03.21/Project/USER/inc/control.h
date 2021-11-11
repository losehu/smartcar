#ifndef __CONTROL_H__
#define __CONTROL_H__
#include "headfile.h"
#define PAI             3.14159265358979f            //圆周率
#define BodyR           10.0f                        //轴向半径

extern int32 MOTOR_Duty_Left;
extern int32 MOTOR_Duty_Right;
extern int32 Speed;
extern int Fres;


extern int32 MOTOR_Speed_Left;
extern int32 MOTOR_Speed_Right;
extern int32 distance_I;
extern int32 left, right;
extern char distance_I_flag;

extern void MOTOR_Control(int32 LDuty, int32 RDuty);
void Speed_Measure(void);
void SpeedS(void);
extern int32 range_protect(int32 duty, int32 min, int32 max); //限幅保护

#endif

