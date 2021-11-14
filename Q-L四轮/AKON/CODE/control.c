#include "control.h"

//����Ƕ� = add*3.14/5000
//���ӽǶ� = add*3.14/1800������ٶ������ԣ����ǵĻ���ʵ�ʲ���Ϊ׼


//----------------------------------------------------------------
//  @brief        ������ֵ��¼
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
//  @brief        ������ֵ��¼
//  @param        void
//  @return       void
//  Sample usage: MotorWrite(); 
//----------------------------------------------------------------
bool StopRun()
{
	if(adc.sum < 6 && debug.stopProtect)//�ܺͲ���С��4����Ϊ�ɼ���ʱ�򶼷ֱ�+1��
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
//  @brief        //ͣ����⿪ʼ�ܣ���ʱ���ͣ��
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
//  @brief        ���ֵ��¼
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
//  @brief        ���ٴ���
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
    else//����ͷ����
    {*/
//        ak_r=Ackerman_kr;
//        ak_l=Ackerman_kl;
    //}
    if(servo.dutyAdd<=-10)//��ת
    {	//�ٶ����ֵ������ʵ��ת��һ�����
//        motor.left.expectSpeed = (int)(motor.expectSpeed*(1-(tan((float)servo.dutyAdd*3.14/5000)*0.396)));
        motor.left.expectSpeed = motor.left.expectSpeed;
        //v_want_l=(int)(Cam_V_target*(1-((float)ak_r/1000)*tan((float)Cam_Servo_add*3.14/4600)/0.396));
        motor.right.expectSpeed=(int)(motor.right.expectSpeed*(1+(tan((float)servo.dutyAdd*3.14/5100)*0.396)));
		//�����һ�����
		// motor.left.expectSpeed = (int)(motor.expectSpeed*(1-(tan���˴�����ת��ϵ����((float)servo.dutyAdd*3.14/5000)*0.396)));
				
    }
    else if(servo.dutyAdd>=10)//��ת
    {
        motor.left.expectSpeed=(int)(motor.left.expectSpeed*(1-(tan((float)servo.dutyAdd*3.14/5100)*0.396)));
		motor.right.expectSpeed = motor.right.expectSpeed;
//        motor.right.expectSpeed=(int)(motor.expectSpeed*(1+(tan((float)servo.dutyAdd*3.14/5000)*0.396)));
        //v_want_r=(int)(Cam_V_target*(1+((float)ak_r/1000)*tan((float)Cam_Servo_add*3.14/4600)/0.396));
    }
    else
    {
        motor.left.expectSpeed =motor.left.expectSpeed;
        motor.right.expectSpeed=motor.right.expectSpeed;//���ּ������ּ���ȷ���������ٶȲ���
    }
}
//----------------------------------------------------------------
//  @brief        ������ֵ���㣬�������٣�PID
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
	 ////////ǰ��-΢����������ʽPID////////
    motor.left.pwm+=(int32)(/*(Cam_Motor_kvff*Cam_Motor_kvff_k*(v_want_l-v_want_l_last[1]))//������֮ǰ���ٶ�ȷ���Ӽ���Ѹ��(��ǰ��û��)
                           +*/pid.leftMotor.ki*pid.leftMotor.error+pid.leftMotor.kp*(pid.leftMotor.error-pid.leftMotor.errorPre)
                           +pid.leftMotor.kd*(pid.leftMotor.error-2*pid.leftMotor.errorPre+pid.leftMotor.errorPrePre)
                           /*-Cam_Motor_kaff*(V_L_Now-V_L_Now_last)*/);
    motor.right.pwm+=(int32)(/*(Cam_Motor_kvff*Cam_Motor_kvff_k*(v_want_r-v_want_r_last[1]))//�ٶ�ǰ��
                           +*/pid.rightMotor.ki*pid.rightMotor.error+pid.rightMotor.kp*(pid.rightMotor.error-pid.rightMotor.errorPre)
                           +pid.rightMotor.kd*(pid.rightMotor.error-2*pid.rightMotor.errorPre+pid.rightMotor.errorPrePre)//����ʽPID
                           /*-Cam_Motor_kaff*(V_R_Now-V_R_Now_last)*/);//���ٶ�ǰ��
}
void MotorControl(void)//��� ǰ��-΢����������ʽPID ���Ƶ��
{
	MotorSpeedJudge();  //��ñ��������ٶ�
	
	MotorDifferential();//���ټ���
	
	MotorGetNowError();//�õ����θ����ٶ�ƫ��

    MotorPID();//ǰ��-΢����������ʽPID
   
}
//----------------------------------------------------------------
//  @brief        �����ֵ
//  @param        void
//  @return       void
//  Sample usage: MotorWrite(); 
//----------------------------------------------------------------
void MotorWrite()
{
	///////////��Ӧ����ĵ������///////////
    motor.left.pwm = range_protect(motor.left.pwm,-45000,45000);
    motor.right.pwm = range_protect(motor.right.pwm,-45000,45000);
	//�������PWM
    PWM_Write();  
}
//-----------------------------------------------------------------------------
//  @brief        ���������ȡ������ֵ��������¼֮ǰ�����������������������ֵ
//  @param        void
//  @return       void
//  Sample usage: MotorDifferential(); 
//-----------------------------------------------------------------------------
void MotorModule()
{
	MotorSpeedGet();   //������ֵ��ȡ
	MotorErrorRecode();//ƫ���¼
	MotorControl();    //���������
	MotorWrite();      //�������ֵ
}
