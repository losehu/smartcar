#ifndef SERVOMOTOR_H
#define SERVOMOTOR_H

#include "headfile.h"

#define S_MOTOR1_PIN   PWM4_MODULE2_CHA_C30



extern void ServoModule();

extern float standardMid;    //标准中线列数
extern float camServoError; //中线加权偏差
extern float errorAverge;    //中线偏差
extern float camServoErrorAdd;
extern float camServoErrorLast;

extern int8 servo_sum_flag;


#endif
