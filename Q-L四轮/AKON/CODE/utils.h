#ifndef _utils_h_
#define _utils_h_

#include "headfile.h"

int clip(int x, int low, int up);

float fclip(float x, float low, float up);

//整型限幅保护
int range_protect(int x, int x_min, int x_max);
float 	range_protect_float(float x, float x_min, float x_max);
int 	ABS(int data);
float   ABS_FLOAT(float data);
void	delay_ms(uint16_t ms);

//============================================LCD==========================================================================================================================//


//============================================CAM==========================================================================================================================//
#define STATICDIFFERENCE(x,y) (((ABS(x))<(y))?0:(x))


extern void lian_xian_zhi(int column1,int row1,int column2,int row2,int flag);


extern const uint8 chinese[][16];


#endif // UTILS_H
