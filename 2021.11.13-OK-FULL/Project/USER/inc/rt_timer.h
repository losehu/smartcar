#include "headfile.h"

rt_timer_t icm_timer;											// ��ʱ�����ƿ�ָ��

extern float  angle_v_out ;
extern int  angle_out , v_out ;
extern int  Direct_Parameter;// ת��ϵ��;
extern int Direct_Last;

int timer_create (void);
void ICM_Timer (void *parameter);