#ifndef _INT_H_
#define _INT_H_


#include "headfile.h"
#define MOTOR_MAX   1000

extern uint8 Run_Flag;

extern float Attitude_Angel_P;        //当前pitch
extern float Target_Angel_P;          //目标pitch

void System_Init();
void initdata(void);



#endif

