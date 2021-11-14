#include "utils.h"

//一些工具类函数


int clip(int x, int low, int up) {
    return x > up ? up : x < low ? low : x;
}

float fclip(float x, float low, float up) {
    return x > up ? up : x < low ? low : x;
}



//整型限幅保护
int range_protect(int x, int x_min, int x_max)
{
	if(x >= x_max) return x_max;
	else if(x <= x_min) return x_min;
	//none
	return x;
}

//整型限幅保护
float range_protect_float(float x, float x_min, float x_max)
{
	if(x >= x_max) return x_max;
	else if(x <= x_min) return x_min;
	//none
	return x;
}

//整型绝对值
int ABS(int data)
{
  if(data <= 0)
    data = -data;
  
  return data;
}

float ABS_FLOAT(float data)
{
  if(data <= 0)
    data = -data;
  
  return data;
}
//delay_us
void delay_ms(unsigned short ms)
{
	systick_delay_ms(ms);
}


//============================================LCD==========================================================================================================================//


//============================================CAM==========================================================================================================================//
//一次函数连线
void lian_xian_zhi(int column1,int row1,int column2,int row2,int flag)
//a,b一队，de一队    a>b   两点之间连线    二次函数
{

    int i=0,ab=0,cd=0;
    for(i=row1; i>=row2; i--)
    {
        cd=(int)(  column2-column1)*(row1-i)/(row1-row2); //直线
        //cd=(column2-column1)*(row1-i)*(row1-i)/(row1-row2)/(row1-row2); //二次函数
        ab=(int)cd+column1;
        ab=range_protect(ab,159,0);
        if(flag==1)
        right_point[i].x=ab;
        if(flag==2)
        left_point[i].x=ab;
    }
}
