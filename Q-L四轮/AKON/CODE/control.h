#ifndef CONTRAL_H
#define CONTRAL_H

#include "headfile.h"


#define DIR_1 D0
#define DIR_2 D1
#define PWM_1 PWM2_MODULE3_CHA_D2
#define PWM_2 PWM2_MODULE3_CHB_D3

// ǰ��ת�Ǻͷ�����ת�ǵı�����ϵ
#define SMOTOR_RATE     (2.4)







extern void StartRunAndStopRun();
extern void MotorModule();


#endif
