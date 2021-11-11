#include "rt_timer.h"
rt_timer_t icm_timer,speed_timer,turn_timer;											// ��ʱ�����ƿ�ָ��
int ICM_CNT=0;

//INIT_APP_EXPORT(timer_create);
void ICM_Timer (void *parameter)//1ms
{
	
	Data_Filter();//��ֵ�˲�
    ICM_I_Int();    //�ǶȻ���
    IMUupdate(ICM_Treated.gyro.x * 0.01745, ICM_Treated.gyro.y * 0.01745, ICM_Treated.gyro.z * 0.01745, ICM_Treated.accdata.x, ICM_Treated.accdata.y, ICM_Treated.accdata.z);   //���½Ƕ�
	    angle_v_out = rubo_PID(&Ang_Vel_PID, Ang_Vel, ICM_State.gyro.y, angle_out);  // ���ΪPWM���
    angle_v_out = range_protect(angle_v_out, -990, 990);
  //�ǶȻ�
	if (ICM_CNT % 5 == 0 && CarInfo.Protect == 'F')
    {
        angle_out = -30 * PID_Realize2(&Angle_PID, Angle, Attitude_Angel_P * 300, v_out/*set_angle * 300*/);// ���ΪĿ����ٶ�
        angle_out = range_protect(angle_out, -3000, 3000);
    }
	
	//	static rt_uint8_t count = 0;
//	if(count++ >= 10)
//	{
//		rt_timer_stop(icm_timer);
//		rt_timer_delete(icm_timer);
//	}
		ICM_CNT++;
		if(ICM_CNT==10000)ICM_CNT=0;
		 pwm_l = angle_v_out -Direct_Parameter;
     pwm_r = angle_v_out+Direct_Parameter;
		    pwm_l = range_protect(pwm_l, -1000, 1000);
    pwm_r = range_protect(pwm_r, -1000, 1000);
    if (CarInfo.UpTime >= 1.000f)  PWM_Write();
}

void TURN_Timer (void *parameter) //1ms
{
		          if (mt9v03x_finish_flag == 1 && Barn.state != BarnIn)
            {
                CameraExtract0();//��ֵ��
                GetBorder();//ͼ����&�ж�&����
                Center_Cal();//�������ڣ�point_center����ƫ��
                mt9v03x_finish_flag = 0;
            //    Radius = PlacePID_Control(&Turn_PID, Turn[2], point_center, 0);	// ��̬PID����ת��ǶȻ�
                  Radius =- rubo_PID(&Turn_PID, rubo_Vel, point_center, 0);	// ��̬PID����ת��ǶȻ�
               Radius=range_protect(Radius,-180,180);
            }
	 Direct_Parameter =- PID_Realize1(&Direct_PID, Direct, ICM_Treated.gyro.z , Radius);	// ת����ٶȻ������Ҹ�   
		//    Direct_Parameter = PID_Realize1(&Direct_PID, Direct,point_center,0);	 
		 //   Direct_Parameter = -rubo_PID(&Direct_PID, Direct, ICM_Treated.gyro.z ,range_protect( Radius/Wheel.now,0,300) );	// ת����ٶȻ������Ҹ�   

		Direct_Parameter = range_protect(Direct_Parameter, -500, 500);
    Direct_Last = 0.8 * Direct_Parameter + 0.2 * Direct_Last;	// �����ϴν��ٶȻ����
	 pwm_l = angle_v_out -Direct_Parameter;
     pwm_r = angle_v_out+Direct_Parameter;
		    pwm_l = range_protect(pwm_l, -1000, 1000);
    pwm_r = range_protect(pwm_r, -1000, 1000);
     PWM_Write();
}
void SPEED_Timer( void *parameter)
{
    KeyScanInt();

  //�ٶ����⻷ 10ms
    	if(ABS(Wheel.now) >80)
	{
							rt_timer_stop(icm_timer);		rt_timer_delete(icm_timer);
									rt_timer_stop(turn_timer);		rt_timer_delete(turn_timer);
		    pwm_l =0;
    pwm_r = 0;
		 PWM_Write();
     ips114_clear(RED);
		ips114_showstr(0,0,"speed die!!!");
		while(1);
	}
        Encoder_Read();
        v_out = rubo_PID(&Vel_PID, Vel, Wheel.now * 100,600); // ���Ϊ�Ŵ�300����Ŀ��Ƕ� 350
        v_out += set_angle * 300; // Ŀ��Ƕȵ����������
        v_out = range_protect(v_out, 7500, 13500);  // -5*300=-1500 -65*300=19500, 33~~40
	}
//------------------------------------------------------------
// @brief		�̴߳����Լ�����
// @param		void		��
// @return		void
// Sample usage:
//------------------------------------------------------------
int timer_create (void)
{
	// ����һ����ʱ�� ����100��tick
	icm_timer = rt_timer_create(
		"T_ICM",											// timer1��ʾ��ʱ�������ƣ�8���ַ��ڡ�
		ICM_Timer,											// timerout1��ʾʱ�䵽��֮����Ҫִ�еĺ���
		RT_NULL,											// RT_NULL��ʾ����Ҫ���ݲ�����
		1,												// 100��ʾ��ʱ���ĳ�ʱʱ��Ϊ100��ϵͳtick��ϵͳ����Ϊ1���룬��100��ʾ100����
		RT_TIMER_FLAG_PERIODIC);							// RT_TIMER_FLAG_PERIODIC��ʾ��ʱ������������  �������ΪRT_TIMER_FLAG_ONE_SHOT��ֻ������һ��
	speed_timer = rt_timer_create(
		"SP_TURN",											// timer1��ʾ��ʱ�������ƣ�8���ַ��ڡ�
		SPEED_Timer,											// timerout1��ʾʱ�䵽��֮����Ҫִ�еĺ���
		RT_NULL,											// RT_NULL��ʾ����Ҫ���ݲ�����
		10,												// 100��ʾ��ʱ���ĳ�ʱʱ��Ϊ100��ϵͳtick��ϵͳ����Ϊ1���룬��100��ʾ100����
		RT_TIMER_FLAG_PERIODIC);							// RT_TIMER_FLAG_PERIODIC��ʾ��ʱ������������  �������ΪRT_TIMER_FLAG_ONE_SHOT��ֻ������һ��
	
	
		turn_timer = rt_timer_create(
		"T_TURN",											// timer1��ʾ��ʱ�������ƣ�8���ַ��ڡ�
		TURN_Timer,											// timerout1��ʾʱ�䵽��֮����Ҫִ�еĺ���
		RT_NULL,											// RT_NULL��ʾ����Ҫ���ݲ�����
		1,												// 100��ʾ��ʱ���ĳ�ʱʱ��Ϊ100��ϵͳtick��ϵͳ����Ϊ1���룬��100��ʾ100����
		RT_TIMER_FLAG_PERIODIC);							// RT_TIMER_FLAG_PERIODIC��ʾ��ʱ������������  �������ΪRT_TIMER_FLAG_ONE_SHOT��ֻ������һ��

	
	
	
	if(icm_timer != RT_NULL)	rt_timer_start(icm_timer);
		if(speed_timer != RT_NULL)		rt_timer_start(speed_timer);
	if(turn_timer != RT_NULL)		rt_timer_start(turn_timer);
	return 0;
}