#ifndef _BSP_ENCODER_H_
#define _BSP_ENCODER_H_

#include "headfile.h"

////statement
extern int16 encoder_l;
extern int16 encoder_r;


////function
void Encoder_Init(void);
void Encoder_Read(void);


#endif

