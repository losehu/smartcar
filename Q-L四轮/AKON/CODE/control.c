#include "control.h"

//舵机角度 = add*3.14/5000
//轮子角度 = add*3.14/1800，这里假定呈线性，不是的话以实际测量为准


//----------------------------------------------------------------
//  @brief        电机误差值记录
//  @param        void
//  @return       void
//  Sample usage: MotorWrite(); 
//----------------------------------------------------------------
bool StartRun()
{
	if((key.right == onePress || key.right == alwaysPress) && lcd.page == 0)
	{
		key.right = noPress;
		state.isRun = true;
		return true;
	}
	state.isRun = false;
	return false;
}
//----------------------------------------------------------------
//  @brief        电机误差值记录
//  @param        void
//  @return       void
//  Sample usage: MotorWrite(); 
//----------------------------------------------------------------
bool StopRun()
{
	if(adc.sum < 6 && debug.stopProtect)//总和不能小于4，因为采集的时候都分别+1了
	{
		state.isRun = false;
		return true;
	}
	state.isRun = true;
	return false;
}
void AutoOpenTask()
{
	OpenTask(lcdTask);
	OpenTask(keyTask);

}
void AutoCloseTask()
{
	CloseTask(lcdTask);
	CloseTask(keyTask);

}
//----------------------------------------------------------------
//  @brief        //停车检测开始跑，跑时检测停车
//  @param        void
//  @return       void
//  Sample usage: MotorWrite(); 
//----------------------------------------------------------------
void StartRunAndStopRun()
{
	if(state.isRun) 
	{
		AutoCloseTask();
		StopRun();
	}
	else if(!state.isRun)
	{
		AutoOpenTask();
		StartRun();
	}
}
void MotorSpeedGet()
{
	EncoderRead();
	motor.left.speed  = wheel.leftSpeed;
	motor.right.speed = wheel.rightSpeed; 
}
//----------------------------------------------------------------
//  @brief        误差值记录
//  @param        void
//  @return       void
//  Sample usage: MotorWrite(); 
//----------------------------------------------------------------
void MotorErrorRecode()
{
	pid.leftMotor.errorPrePre = pid.leftMotor.errorPre;
    pid.rightMotor.errorPrePre = pid.rightMotor.errorPre;
//    Cam_Motor_error_pre_pre_L=Cam_Motor_error_pre_L;
//    Cam_Motor_error_pre_pre_R=Cam_Motor_error_pre_R;
    
//    Cam_Motor_error_pre_L=Cam_Motor_error_L;
//    Cam_Motor_error_pre_R=Cam_Motor_error_R;
    pid.leftMotor.errorPre = pid.leftMotor.error;
    pid.rightMotor.errorPre = pid.rightMotor.error;
}
void MotorSpeedJudge()
{
	if(state.isRun)
	{
		switch(state.element)
		{
			case normal:
				motor.expectSpeed.use = motor.expectSpeed.normal;
				break;
			case circle:
				motor.expectSpeed.use = motor.expectSpeed.circle;
				break;
			default:
				BEEP_ON;
				break;
			}
	}
	else 
	{
		motor.expectSpeed.use = 0;	
	}
	motor.left.expectSpeed = motor.expectSpeed.use;
	motor.right.expectSpeed = motor.expectSpeed.use;
}
//----------------------------------------------------------------
//  @brief        差速处理
//  @param        void
//  @return       void
//  Sample usage: MotorDifferential(); 
//----------------------------------------------------------------
void MotorDifferential()
{
	    /*if(ELC_Circle_flag==1)//253 253
    {
        ak_r=250;
        ak_l=250;
    }
    else//摄像头差速
    {*/
//        ak_r=Ackerman_kr;
//        ak_l=Ackerman_kl;
    //}
    if(servo.dutyAdd<=-10)//右转
    {	//假定舵机值与轮子实际转角一次相关
//        motor.left.expectSpeed = (int)(motor.expectSpeed*(1-(tan((float)servo.dutyAdd*3.14/5000)*0.396)));
        motor.left.expectSpeed = motor.left.expectSpeed;
        //v_want_l=(int)(Cam_V_target*(1-((float)ak_r/1000)*tan((float)Cam_Servo_add*3.14/4600)/0.396));
        motor.right.expectSpeed=(int)(motor.right.expectSpeed*(1+(tan((float)servo.dutyAdd*3.14/5100)*0.396)));
		//如果非一次相关
		// motor.left.expectSpeed = (int)(motor.expectSpeed*(1-(tan（此处放入转换系数）((float)servo.dutyAdd*3.14/5000)*0.396)));
				
    }
    else if(servo.dutyAdd>=10)//左转
    {
        motor.left.expectSpeed=(int)(motor.left.expectSpeed*(1-(tan((float)servo.dutyAdd*3.14/5100)*0.396)));
		motor.right.expectSpeed = motor.right.expectSpeed;
//        motor.right.expectSpeed=(int)(motor.expectSpeed*(1+(tan((float)servo.dutyAdd*3.14/5000)*0.396)));
        //v_want_r=(int)(Cam_V_target*(1+((float)ak_r/1000)*tan((float)Cam_Servo_add*3.14/4600)/0.396));
    }
    else
    {
        motor.left.expectSpeed =motor.left.expectSpeed;
        motor.right.expectSpeed=motor.right.expectSpeed;//内轮减速外轮加速确保中轴线速度不变
    }
}
//----------------------------------------------------------------
//  @brief        电机输出值计算，包含差速，PID
//  @param        void
//  @return       void
//  Sample usage: MotorWrite(); 
//----------------------------------------------------------------
void MotorGetNowError()
{
	pid.leftMotor.error=motor.left.expectSpeed-motor.left.speed;
    pid.rightMotor.error=motor.right.expectSpeed-motor.right.speed;
}
void MotorPID()
{
	 ////////前馈-微分先行增量式PID////////
    motor.left.pwm+=(int32)(/*(Cam_Motor_kvff*Cam_Motor_kvff_k*(v_want_l-v_want_l_last[1]))//乘三次之前的速度确保加减速迅速(这前馈没用)
                           +*/pid.leftMotor.ki*pid.leftMotor.error+pid.leftMotor.kp*(pid.leftMotor.error-pid.leftMotor.errorPre)
                           +pid.leftMotor.kd*(pid.leftMotor.error-2*pid.leftMotor.errorPre+pid.leftMotor.errorPrePre)
                           /*-Cam_Motor_kaff*(V_L_Now-V_L_Now_last)*/);
    motor.right.pwm+=(int32)(/*(Cam_Motor_kvff*Cam_Motor_kvff_k*(v_want_r-v_want_r_last[1]))//速度前馈
                           +*/pid.rightMotor.ki*pid.rightMotor.error+pid.rightMotor.kp*(pid.rightMotor.error-pid.rightMotor.errorPre)
                           +pid.rightMotor.kd*(pid.rightMotor.error-2*pid.rightMotor.errorPre+pid.rightMotor.errorPrePre)//增量式PID
                           /*-Cam_Motor_kaff*(V_R_Now-V_R_Now_last)*/);//加速度前馈
}
void MotorControl(void)//电磁 前馈-微分先行增量式PID 控制电机
{
	MotorSpeedJudge();  //获得本次期望速度
	
	MotorDifferential();//差速计算
	
	MotorGetNowError();//得到本次各轮速度偏差

    MotorPID();//前馈-微分先行增量式PID
   
}
//----------------------------------------------------------------
//  @brief        电机赋值
//  @param        void
//  @return       void
//  Sample usage: MotorWrite(); 
//----------------------------------------------------------------
void MotorWrite()
{
	///////////对应输出的电机控制///////////
    motor.left.pwm = range_protect(motor.left.pwm,-45000,45000);
    motor.right.pwm = range_protect(motor.right.pwm,-45000,45000);
	//给电机赋PWM
    PWM_Write();  
}
//-----------------------------------------------------------------------------
//  @brief        电机处理，获取编码器值——》记录之前误差——》计算输出量——》赋值
//  @param        void
//  @return       void
//  Sample usage: MotorDifferential(); 
//-----------------------------------------------------------------------------
void MotorModule()
{
	MotorSpeedGet();   //编码器值获取
	MotorErrorRecode();//偏差记录
	MotorControl();    //输出量计算
	MotorWrite();      //输出量赋值
}
