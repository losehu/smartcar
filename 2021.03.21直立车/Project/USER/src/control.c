#include "Control.h"
#include "isr.h"

////d0d1����d2d3����

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
int Fres = 0;	// ��̬��������

void Speed_Measure(void)
{
    Encoder_Read();

    /******* �ҵ���ٶ���ؿ��� ********/
    MOTOR_Speed_Right = encoder_r * 0.042982; //����   MOTOR_Speed_Right = encoder_r*3528*PAI/19456;//����
    if(MOTOR_Speed_Right < 0)
    {
        MOTOR_Speed_Right = -MOTOR_Speed_Right;
    }
    MOTOR_Right_Acc = MOTOR_Speed_Right - MOTOR_Speed_Right_Last;	// ������ٶ�
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
    MOTOR_Speed_Right = range_protect(MOTOR_Speed_Right, 0, 500); //�޷�����
    MOTOR_Speed_Right_Last = MOTOR_Speed_Right;
    /******* �ҵ���ٶ���ؿ��ƽ��� ********/
    /******* �����ٶ���ؿ��� ********/
    MOTOR_Speed_Left = encoder_l * 0.04252; //����
    if(MOTOR_Speed_Left < 0)
    {
        MOTOR_Speed_Left = -MOTOR_Speed_Left;
    }
    MOTOR_Left_Acc = MOTOR_Speed_Left - MOTOR_Speed_Left_Last;	// ������ٶ�
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
    MOTOR_Speed_Left = range_protect(MOTOR_Speed_Left, 0, 500); //�޷�����
    MOTOR_Speed_Left_Last = MOTOR_Speed_Left;
    /*****************ת���ֱ����ʵ�ٶȼ���*****************/
    SpeedLw = MOTOR_Speed_Left - BodyR * IMC_Treated.gyro.z * PAI / 180;
    SpeedRw = MOTOR_Speed_Right + BodyR * IMC_Treated.gyro.z * PAI / 180;
    if(IMC_Treated.gyro.z < -50)
    {
        Speed_Now = SpeedRw;       // ��ת�����ұ�
    }
    else if(IMC_Treated.gyro.z > 50)
    {
        Speed_Now = SpeedLw;       // ��ת�������
    }
    else
    {
        Speed_Now = (MOTOR_Speed_Right + MOTOR_Speed_Left) / 2;    // ����ôת��������
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
    Speed_Now = range_protect(Speed_Now, 0, 300); //�޷�����
    Speed_Now_Last = Speed_Now;

    //	if(Communication_G_Flag)
    //	{
    //		Communication_I += Speed_Now;
    //	}

    if( (IMC_Treated.accdata.x < 1) && (IMC_Treated.accdata.x > -1) )  //����������
    {
        distance_I = distance_I + (encoder_r + encoder_l) / 2 * 0.04252;
    }

    /**********�µ��������**********/
    if(rampway.In_flag)
    {
        rampway.In_I = rampway.In_I + (encoder_r + encoder_l) / 2 * 0.04252;
    }

    /**********�������������**********/
    if(roundabout.Out_flag)
    {
        roundabout.Out_I += Speed_Now;
    }
    if(roundabout.In_dis_flag)//��Բ����ʼ����
    {
        roundabout.In_dis_I += Speed_Now;
    }
    /**********������·�ھ������**********/
    if( !thressload.Out_flag )//������·�ڿ�ʼ����
    {
        thressload.Out_I += Speed_Now;
    }

    /**********ת����С�ٶȼ���**********/
    Speed_Min = Speed_Min * 0.1 + Speed_Now * 0.9;
    Speed_Min = range_protect(Speed_Min, 100, 290); //�޷�����
    /**********ת�����ѡ��**********/
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
    /*******************************��**************************/
    if(LDuty > 0)//��ǰ��
    {
        gpio_set(A11, 1);
        pwm_duty_updata(TIM_2, TIM_2_CH3_C09, LDuty);
    }
    if(LDuty < 0)//�����
    {
        gpio_set(A11, 0);
        pwm_duty_updata(TIM_2, TIM_2_CH3_C09, -LDuty);
    }
    /*******************************��**************************/
    if(RDuty > 0)//��ǰ��
    {
        gpio_set(B11, 0);
        pwm_duty_updata(TIM_2, TIM_2_CH2_A12, RDuty);
    }
    if(RDuty < 0)//�����
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
//		RDuty = range_protect(RDuty, 0, MOTOR_MAX);	// �޷�����
//		pwm_duty_updata(TIM_2, TIM_2_CH3_C09, 0);
//		pwm_duty_updata(TIM_2, TIM_2_CH4_B11, RDuty);
//	}
//	else
//	{
//		RDuty = range_protect(-RDuty, 0, MOTOR_MAX);// �޷�����
//	    pwm_duty_updata(TIM_2, TIM_2_CH4_B11, 0);
//	    pwm_duty_updata(TIM_2, TIM_2_CH3_C09, RDuty);
//	}
//
//	if (LDuty >= 0)
//	{
//		LDuty = range_protect(LDuty, 0, MOTOR_MAX);	// �޷�����
//		pwm_duty_updata(TIM_2, TIM_2_CH1_A11, 0);
//		pwm_duty_updata(TIM_2, TIM_2_CH2_A12, LDuty);
//    }
//	else
//	{
//		LDuty = range_protect(-LDuty, 0, MOTOR_MAX);// �޷�����
//		pwm_duty_updata(TIM_2, TIM_2_CH1_A11, LDuty);
//		pwm_duty_updata(TIM_2, TIM_2_CH2_A12, 0);
//	}
//}

void SpeedS(void)
{
    if(!roundabout.Speed) //��Բ����Բ����ʱ�����    û��
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

int32 range_protect(int32 duty, int32 min, int32 max)//�޷�����
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

