#include "Control.h"
#include "isr.h"

////d0d1是左。d2d3是右

int32 MOTOR_Duty_Left  = 0;
int32 MOTOR_Duty_Right = 0;
int32 MOTOR_Speed_Left = 0;
int32 MOTOR_Speed_Right = 0;
int32 MOTOR_Speed_Left_Last = 0;
int32 MOTOR_Speed_Right_Last = 0;
int32 MOTOR_Left_Acc = 0;
int32 MOTOR_Right_Acc = 0;
int32 Speed = 0;

int32 left, right;

int32 distance_I = 0;

char distance_I_flag = 0;

float SpeedRw, SpeedLw;
int Fres = 0;	// 动态参数变量

void Speed_Measure(void)
{
    Encoder_Read();

    /******* 右电机速度相关控制 ********/
    MOTOR_Speed_Right = encoder_r * 0.042982; //右轮   MOTOR_Speed_Right = encoder_r*3528*PAI/19456;//右轮
    if(MOTOR_Speed_Right < 0)
    {
        MOTOR_Speed_Right = -MOTOR_Speed_Right;
    }
    MOTOR_Right_Acc = MOTOR_Speed_Right - MOTOR_Speed_Right_Last;	// 计算加速度
    if(MOTOR_Right_Acc > 100)
    {
        MOTOR_Speed_Right += 100;
    }
    else if(MOTOR_Right_Acc < -100)
    {
        MOTOR_Speed_Right -= 100;
    }
    else
    {
        MOTOR_Speed_Right = MOTOR_Speed_Right;
    }
    MOTOR_Speed_Right = range_protect(MOTOR_Speed_Right, 0, 500); //限幅保护
    MOTOR_Speed_Right_Last = MOTOR_Speed_Right;
    /******* 右电机速度相关控制结束 ********/
    /******* 左电机速度相关控制 ********/
    MOTOR_Speed_Left = encoder_l * 0.04252; //左轮
    if(MOTOR_Speed_Left < 0)
    {
        MOTOR_Speed_Left = -MOTOR_Speed_Left;
    }
    MOTOR_Left_Acc = MOTOR_Speed_Left - MOTOR_Speed_Left_Last;	// 计算加速度
    if(MOTOR_Left_Acc > 100)
    {
        MOTOR_Speed_Left += 100;
    }
    else if(MOTOR_Left_Acc < -100)
    {
        MOTOR_Speed_Left -= 100;
    }
    else
    {
        MOTOR_Speed_Left = MOTOR_Speed_Left;
    }
    MOTOR_Speed_Left = range_protect(MOTOR_Speed_Left, 0, 500); //限幅保护
    MOTOR_Speed_Left_Last = MOTOR_Speed_Left;
    /*****************转向和直道真实速度计算*****************/
    SpeedLw = MOTOR_Speed_Left - BodyR * IMC_Treated.gyro.z * PAI / 180;
    SpeedRw = MOTOR_Speed_Right + BodyR * IMC_Treated.gyro.z * PAI / 180;
    if(IMC_Treated.gyro.z < -50)
    {
        Speed_Now = SpeedRw;       // 左转，用右边
    }
    else if(IMC_Treated.gyro.z > 50)
    {
        Speed_Now = SpeedLw;       // 右转，用左边
    }
    else
    {
        Speed_Now = (MOTOR_Speed_Right + MOTOR_Speed_Left) / 2;    // 不怎么转，用两边
    }
    if( (Speed_Now - Speed_Now_Last) > 100)
    {
        Speed_Now = Speed_Now_Last + 100;
    }
    else if( (Speed_Now - Speed_Now_Last) < -100)
    {
        Speed_Now = Speed_Now_Last - 100;
    }
    else
    {
        Speed_Now = Speed_Now;
    }
    Speed_Now = range_protect(Speed_Now, 0, 300); //限幅保护
    Speed_Now_Last = Speed_Now;

    //	if(Communication_G_Flag)
    //	{
    //		Communication_I += Speed_Now;
    //	}

    if( (IMC_Treated.accdata.x < 1) && (IMC_Treated.accdata.x > -1) )  //编码器积分
    {
        distance_I = distance_I + (encoder_r + encoder_l) / 2 * 0.04252;
    }

    /**********坡道距离积分**********/
    if(rampway.In_flag)
    {
        rampway.In_I = rampway.In_I + (encoder_r + encoder_l) / 2 * 0.04252;
    }

    /**********出环岛距离积分**********/
    if(roundabout.Out_flag)
    {
        roundabout.Out_I += Speed_Now;
    }
    if(roundabout.In_dis_flag)//进圆环开始积分
    {
        roundabout.In_dis_I += Speed_Now;
    }
    /**********出三叉路口距离积分**********/
    if( !thressload.Out_flag )//出三叉路口开始积分
    {
        thressload.Out_I += Speed_Now;
    }

    /**********转向最小速度计算**********/
    Speed_Min = Speed_Min * 0.1 + Speed_Now * 0.9;
    Speed_Min = range_protect(Speed_Min, 100, 290); //限幅保护
    /**********转向参数选择**********/
    if(Speed_Min <= Speed_Set * 0.3)
    {
        Fres = 2;
    }
    else if(Speed_Min <= Speed_Set * 0.7)
    {
        Fres = 2;
    }
    else if(Speed_Min <= Speed_Set)
    {
        Fres = 2;
    }
    else
    {
        Fres = 2;
    }
}

void MOTOR_Control(int32 LDuty, int32 RDuty)
{
    range_protect(LDuty, -1000, 1000);
    range_protect(RDuty, -1000, 1000);
    /*******************************左**************************/
    if(LDuty > 0)//向前走
    {
        gpio_set(A11, 1);
        pwm_duty_updata(TIM_2, TIM_2_CH3_C09, LDuty);
    }
    if(LDuty < 0)//向后走
    {
        gpio_set(A11, 0);
        pwm_duty_updata(TIM_2, TIM_2_CH3_C09, -LDuty);
    }
    /*******************************右**************************/
    if(RDuty > 0)//向前走
    {
        gpio_set(B11, 0);
        pwm_duty_updata(TIM_2, TIM_2_CH2_A12, RDuty);
    }
    if(RDuty < 0)//向后走
    {
        gpio_set(B11, 1);
        pwm_duty_updata(TIM_2, TIM_2_CH2_A12, -RDuty);
    }
    if(LDuty == 0)
    {
        pwm_duty_updata(TIM_2, TIM_2_CH3_C09, 0);
    }
    if(RDuty == 0)
    {
        pwm_duty_updata(TIM_2, TIM_2_CH2_A12, 0);
    }
}

//void MOTOR_Control(int32 LDuty, int32 RDuty)
//{
//	if (RDuty >= 0)
//	{
//		RDuty = range_protect(RDuty, 0, MOTOR_MAX);	// 限幅保护
//		pwm_duty_updata(TIM_2, TIM_2_CH3_C09, 0);
//		pwm_duty_updata(TIM_2, TIM_2_CH4_B11, RDuty);
//	}
//	else
//	{
//		RDuty = range_protect(-RDuty, 0, MOTOR_MAX);// 限幅保护
//	    pwm_duty_updata(TIM_2, TIM_2_CH4_B11, 0);
//	    pwm_duty_updata(TIM_2, TIM_2_CH3_C09, RDuty);
//	}
//
//	if (LDuty >= 0)
//	{
//		LDuty = range_protect(LDuty, 0, MOTOR_MAX);	// 限幅保护
//		pwm_duty_updata(TIM_2, TIM_2_CH1_A11, 0);
//		pwm_duty_updata(TIM_2, TIM_2_CH2_A12, LDuty);
//    }
//	else
//	{
//		LDuty = range_protect(-LDuty, 0, MOTOR_MAX);// 限幅保护
//		pwm_duty_updata(TIM_2, TIM_2_CH1_A11, LDuty);
//		pwm_duty_updata(TIM_2, TIM_2_CH2_A12, 0);
//	}
//}

void SpeedS(void)
{
    if(!roundabout.Speed) //进圆环和圆环的时候减速    没用
    {

        //		if(Direct_Last > 300 || Direct_Last < -300)
        //		{
        //			Speed_Set = 220;
        //		}
        if(Direct_Last > 100 || Direct_Last < -100)
        {
            Speed_Set = 255;
        }
        else
        {
            Speed_Set = 270;
        }
    }
    else
    {
        Target_Angel_P = -2600;

    }
}

int32 range_protect(int32 duty, int32 min, int32 max)//限幅保护
{
    if (duty >= max)
    {
        return max;
    }
    if (duty <= min)
    {
        return min;
    }
    else
    {
        return duty;
    }
}

