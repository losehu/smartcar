#include "rt_timer.h"
rt_timer_t icm_timer,speed_timer,turn_timer;											// 定时器控制块指针
int ICM_CNT=0;

//INIT_APP_EXPORT(timer_create);
void ICM_Timer (void *parameter)//1ms
{
	
	Data_Filter();//均值滤波
    ICM_I_Int();    //角度积分
    IMUupdate(ICM_Treated.gyro.x * 0.01745, ICM_Treated.gyro.y * 0.01745, ICM_Treated.gyro.z * 0.01745, ICM_Treated.accdata.x, ICM_Treated.accdata.y, ICM_Treated.accdata.z);   //更新角度
	    angle_v_out = rubo_PID(&Ang_Vel_PID, Ang_Vel, ICM_State.gyro.y, angle_out);  // 结果为PWM输出
    angle_v_out = range_protect(angle_v_out, -990, 990);
  //角度环
	if (ICM_CNT % 5 == 0 && CarInfo.Protect == 'F')
    {
        angle_out = -30 * PID_Realize2(&Angle_PID, Angle, Attitude_Angel_P * 300, v_out/*set_angle * 300*/);// 结果为目标角速度
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
                CameraExtract0();//二值化
                GetBorder();//图像处理&判断&补线
                Center_Cal();//函数出口：point_center中线偏差
                mt9v03x_finish_flag = 0;
            //    Radius = PlacePID_Control(&Turn_PID, Turn[2], point_center, 0);	// 动态PID控制转向角度环
                  Radius =- rubo_PID(&Turn_PID, rubo_Vel, point_center, 0);	// 动态PID控制转向角度环
               Radius=range_protect(Radius,-180,180);
            }
	 Direct_Parameter =- PID_Realize1(&Direct_PID, Direct, ICM_Treated.gyro.z , Radius);	// 转向角速度环左正右负   
		//    Direct_Parameter = PID_Realize1(&Direct_PID, Direct,point_center,0);	 
		 //   Direct_Parameter = -rubo_PID(&Direct_PID, Direct, ICM_Treated.gyro.z ,range_protect( Radius/Wheel.now,0,300) );	// 转向角速度环左正右负   

		Direct_Parameter = range_protect(Direct_Parameter, -500, 500);
    Direct_Last = 0.8 * Direct_Parameter + 0.2 * Direct_Last;	// 更新上次角速度环结果
	 pwm_l = angle_v_out -Direct_Parameter;
     pwm_r = angle_v_out+Direct_Parameter;
		    pwm_l = range_protect(pwm_l, -1000, 1000);
    pwm_r = range_protect(pwm_r, -1000, 1000);
     PWM_Write();
}
void SPEED_Timer( void *parameter)
{
    KeyScanInt();

  //速度最外环 10ms
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
        v_out = rubo_PID(&Vel_PID, Vel, Wheel.now * 100,600); // 结果为放大300倍的目标角度 350
        v_out += set_angle * 300; // 目标角度叠加在零点上
        v_out = range_protect(v_out, 7500, 13500);  // -5*300=-1500 -65*300=19500, 33~~40
	}
//------------------------------------------------------------
// @brief		线程创建以及启动
// @param		void		空
// @return		void
// Sample usage:
//------------------------------------------------------------
int timer_create (void)
{
	// 创建一个定时器 周期100个tick
	icm_timer = rt_timer_create(
		"T_ICM",											// timer1表示定时器的名称，8个字符内。
		ICM_Timer,											// timerout1表示时间到了之后需要执行的函数
		RT_NULL,											// RT_NULL表示不需要传递参数。
		1,												// 100表示定时器的超时时间为100个系统tick，系统周期为1毫秒，则100表示100毫秒
		RT_TIMER_FLAG_PERIODIC);							// RT_TIMER_FLAG_PERIODIC表示定时器以周期运行  如果设置为RT_TIMER_FLAG_ONE_SHOT则只会运行一次
	speed_timer = rt_timer_create(
		"SP_TURN",											// timer1表示定时器的名称，8个字符内。
		SPEED_Timer,											// timerout1表示时间到了之后需要执行的函数
		RT_NULL,											// RT_NULL表示不需要传递参数。
		10,												// 100表示定时器的超时时间为100个系统tick，系统周期为1毫秒，则100表示100毫秒
		RT_TIMER_FLAG_PERIODIC);							// RT_TIMER_FLAG_PERIODIC表示定时器以周期运行  如果设置为RT_TIMER_FLAG_ONE_SHOT则只会运行一次
	
	
		turn_timer = rt_timer_create(
		"T_TURN",											// timer1表示定时器的名称，8个字符内。
		TURN_Timer,											// timerout1表示时间到了之后需要执行的函数
		RT_NULL,											// RT_NULL表示不需要传递参数。
		1,												// 100表示定时器的超时时间为100个系统tick，系统周期为1毫秒，则100表示100毫秒
		RT_TIMER_FLAG_PERIODIC);							// RT_TIMER_FLAG_PERIODIC表示定时器以周期运行  如果设置为RT_TIMER_FLAG_ONE_SHOT则只会运行一次

	
	
	
	if(icm_timer != RT_NULL)	rt_timer_start(icm_timer);
		if(speed_timer != RT_NULL)		rt_timer_start(speed_timer);
	if(turn_timer != RT_NULL)		rt_timer_start(turn_timer);
	return 0;
}