#ifndef _ALL_INIT_H_
#define _ALL_INIT_H_

#include "headfile.h"








//fun
void 	delay_us(uint16_t us);
void	delay_ms(uint16_t ms);
short 	range_protect(short x, short x_min, short x_max);
float range_protect_float(float x, float x_min, float x_max);
int 	ABS(int data);
float 	ABS_FLOAT(float data);
uint8 	Global_Init(void);
void 	ICM_I_Int(void);




#endif

