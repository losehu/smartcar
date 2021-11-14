#include "utils.h"

//һЩ�����ຯ��


int clip(int x, int low, int up) {
    return x > up ? up : x < low ? low : x;
}

float fclip(float x, float low, float up) {
    return x > up ? up : x < low ? low : x;
}



//�����޷�����
int range_protect(int x, int x_min, int x_max)
{
	if(x >= x_max) return x_max;
	else if(x <= x_min) return x_min;
	//none
	return x;
}

//�����޷�����
float range_protect_float(float x, float x_min, float x_max)
{
	if(x >= x_max) return x_max;
	else if(x <= x_min) return x_min;
	//none
	return x;
}

//���;���ֵ
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
//һ�κ�������
void lian_xian_zhi(int column1,int row1,int column2,int row2,int flag)
//a,bһ�ӣ�deһ��    a>b   ����֮������    ���κ���
{

    int i=0,ab=0,cd=0;
    for(i=row1; i>=row2; i--)
    {
        cd=(int)(  column2-column1)*(row1-i)/(row1-row2); //ֱ��
        //cd=(column2-column1)*(row1-i)*(row1-i)/(row1-row2)/(row1-row2); //���κ���
        ab=(int)cd+column1;
        ab=range_protect(ab,159,0);
        if(flag==1)
        right_point[i].x=ab;
        if(flag==2)
        left_point[i].x=ab;
    }
}
