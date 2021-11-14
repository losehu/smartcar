#include "servomotor.h"

int8 servo_sum_flag=0;
/**
  * *摄像头中线偏差处理
  **后期如果可行的话，可以考虑动态中线，比如川大的那个最远有效点列
  **/

float standardMid;    //标准中线列数

float camServoError; //中线加权偏差
float errorAverge;    //中线偏差

//偏差权重值,底部15行舍去
uint16 weight[nHeight]={  0,0,0,0,0,0,0,0,0,0, //40行到49行
                        0,0,0,0,0,5,5,5,5,5, //40行到49行
                        5,5,5,10,10,10,10,10,10,10,  //60行到59行
                        10,10,10,10,10,10,10,10,10,10,   //20行到29行
                        10,10,10,10,10,10,10,10,10,10,   //20行到29行
                        11,12,13,14,15,16,17,18,19,20,  //40行到49行
                        20,20,20,20,20,20,20,20,10,5,  //40行到49行
                        0,0,0,0,0,0,0,0,0,0, //70行到79行
                     };
uint16 weight_circle[nHeight]={ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0,10,10,10,10,10,
                       10,10,10,10,10,10,10,10,10,10,
                       10,10,10,10,10,10,10,10,10,10,
                       10,10,10,10,10,10,10,10,10,10,
                       3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 };
					 
float camServoErrorLast,camServoErrorAdd;
void  Cal_weight_average(void)
{
	   uint8 i;
float sum,weight_sum;  //权重中线和，权重和
float sum_ave,add_ave; //不带权重中线和，加的中线点数  

   standardMid=80;

   for(i=15;i<length;i++)
   {
       if((ELC_Circle_flag==1&&ELC_Circle_turn==1)||(ELC_Circle_flag==1&&ELC_Circle_turn==0))
       {sum+= mid_point[i].x*weight_circle[i] ;      
       weight_sum+=weight_circle[i];}                
       
       
//       else if((forkroad_flag_right ==1
//               ||forkroad_flag_left ==1 ||forkroad_flag_right_out==1
//               ||forkroad_flag_left_out==1) )
//       {
//           sum+= mid_point[i].x*weight[i] ;
//           weight_sum+=weight[i];
//       }
//       else
       {
           sum+= mid_point[i].x*weight[i] ;
           weight_sum+=weight[i];
       }

       if(i>(length*(2/3)))
       {
           sum_ave+= mid_point[i].x;
           add_ave++;
       }
   }
	 camServoErrorLast = camServoError;
   camServoError=standardMid-sum/weight_sum;
	 camServoErrorAdd = camServoError - camServoErrorLast;
   errorAverge   =standardMid-sum_ave/add_ave;
   sum = 0;
   sum_ave = 0;
   add_ave = 0;
   
}
//判断应该使用哪个PID参数
void UsePIDNumJudge()
{
	switch(state.element)
	{
		case normal:
			pid.servo.cam.use = pid.servo.cam.normal;
			break;
		case circle:
			pid.servo.cam.use = pid.servo.cam.circle;
			break;
		default:
			BEEP_ON;
			break;
	}

}
//需要计算出误差以后再进行舵机控制，这个函数仅仅有控制！
void ServoPID()
{

	  ///////////参数求和,方便显示///////////	
	pid.servo.cam.repeat.kp1Sum = camServoError *pid.servo.cam.use.kp1;
    
    pid.servo.cam.repeat.kp2Sum = ((camServoError*(80 - length)*(80 - length)*pid.servo.cam.use.kp2)/1000);
	
    pid.servo.cam.repeat.kpSum = pid.servo.cam.repeat.kp1Sum + pid.servo.cam.repeat.kp2Sum;
    pid.servo.cam.repeat.kdSum = pid.servo.cam.use.kd*(camServoErrorLast - camServoError);
	
    servo.dutyAdd = pid.servo.cam.repeat.kpSum +pid.servo.cam.repeat.kdSum;
}

int32 servo_add_sum=0;
int32 servo_add_count=0;
int32 servo_add,servo_add_l,duty,servo_circle;
void ServoDutyCompute()
{
	if(servo_sum_flag==1&&ELC_Circle_flag==1&&ELC_Circle_turn==0&&angel_count>190&&angel_count<200)
    {
        servo_add_sum=servo_add_sum+servo_add;
        servo_add_count++;
//        BEEP_NO;
    }
    else if(servo_sum_flag==1&&ELC_Circle_flag==1&&ELC_Circle_turn==0&&angel_count>200)
    {
        servo_sum_flag=0;
        servo_circle = servo_add_sum/servo_add_count;
        servo_add_count=0;
        servo_add_sum=0;
        BEEP_OFF;
        if(servo_circle>0)
        servo_circle=range_protect(servo_circle,servo.nowMid-400,servo.nowMid+400-70);
        else if(servo_circle<0)
        servo_circle=range_protect(servo_circle,-(servo.nowMid-400+70),-(servo.nowMid+400));
    }
	servo.duty = servo.nowMid + servo.dutyAdd;
	
	if((circle_out_flag==1||circle_out_flag2==1)&&angel_count<=190&&hd_zuo==1)
            servo.duty  =servo.nowMid+servo_circle;//3700;//3640
    else if((circle_out_flag==1||circle_out_flag2==1)&&angel_count<=190&&hd_you==1)
            servo.duty  =servo.nowMid+servo_circle;
    else if(ELC_Circle_flag==1&&ELC_Circle_turn==1&&hd_zuo==1&&servo.duty<=(servo.nowMid+200))
            servo.duty=servo.nowMid+200;
    else if(ELC_Circle_flag==1&&ELC_Circle_turn==1&&hd_you==1&&servo.duty>=(servo.nowMid-200))
            servo.duty=servo.nowMid-200;
//        else if(ELC_Circle_flag==1&&ELC_Circle_turn==1&&servo.duty<=(SD5_Middle+750))
//            servo.duty=SD5_Middle+750;
    else if(ELC_Circle_flag==1&&ELC_Circle_turn==0&&hd_zuo==1&&servo.duty<=(servo.nowMid+300))
            servo.duty=servo.nowMid+300;
    else if(ELC_Circle_flag==1&&ELC_Circle_turn==0&&hd_you==1&&servo.duty>=(servo.nowMid-300))
            servo.duty=servo.nowMid-300;


	//消除静差
    STATICDIFFERENCE(servo.dutyAdd,8);

}
//----------------------------------------------------------------
//  @brief        舵机模块PWM值保护以及写入
//  @param        void
//  @return       void
//  Sample usage: ServoPWMWrite(); 
//----------------------------------------------------------------
void ServoPWMWrite()
{
    if(task[servoTask].isOpen) 
	{
		servo.duty = range_protect(servo.duty, servo.normalMid - servo.rangeProtect, servo.normalMid + servo.rangeProtect); //临时使用
	}
    else 
	{
		servo.duty = servo.nowMid;
	}
	pwm_duty(S_MOTOR1_PIN, servo.duty);
}

void ServoControl(void)
{
	UsePIDNumJudge(); //判断应该使用哪个PID参数
    ServoPID(); //对于误差进行PID计算
//	AdcControl();
	ServoDutyCompute();//对于最终Duty值大小计算
    ServoPWMWrite();//赋予最终PWM值
}
//----------------------------------------------------------------
//  @brief        舵机模块所有东西。
//  @param        void
//  @return       void
//  Sample usage: ServoModule(); 
//----------------------------------------------------------------
void ServoModule()
{
	Camer();//获取图像，二值化处理
	Cal_weight_average();//舵机误差计算
	ServoControl();//打角控制。
}
