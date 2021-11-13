#include "headfile.h"

rt_timer_t icm_timer;											// 定时器控制块指针

extern float  angle_v_out ;
extern int  angle_out , v_out ;
extern int  Direct_Parameter;// 转向系数;
extern int Direct_Last;

int timer_create (void);
void ICM_Timer (void *parameter);