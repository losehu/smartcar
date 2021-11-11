#include "Balance.h"
#include "isr.h"

char Speed_Flag = 0;
char	Angle_Flag;
char	Ang_Velocity_Flag;
char In_Roundabout_Flag3 = 1;

char Communication_G_Flag = 0;
char Communication_S_Flag = 0;
int32 Communication_I = 0;



int32 Thressload_cnt = 0;

int32 Direct_Last = 0,
      Direct_Parameter = 0,// ת��ϵ��;
      Radius = 0,         // Ŀ��ת��뾶����
      Speed_Now = 0,	// ��ǰʵ���ٶ�
      Speed_Now_Last = 0,     // �ϴ�ʵ���ٶ�
      Speed_Min = 0,	// ������С�ٶ�
      Speed_Set = 300, 	// Ŀ���趨�ٶ�
      Run_I = 0,
      O_I = 60;


int32 Theory_Duty = 0;// ����ֱ��ռ�ձ�

S_INT32_XYZ
Tar_Ang_Vel,	// Ŀ����ٶ�
Tar_Ang_Vel_Last;	// �ϴ�Ŀ����ٶ�

/********************* ����ƽ����� *********************/
void Balance_Control(void)
{
    Target_Angel_P = -2200;
    //	if(roundabout.frequency)//��һ�λ���
    //	{
    Huandao();
    //	}
    //	Thressload();
    //	Rampway();
    if(roundabout.In_flag)//��Բ������
    {
        roundabout.In_I += IMC_Treated.gyro.z;
        roundabout.In_turn_I = roundabout.In_I * Angle_coe;
    }
    if(thressload.In_flag)//������·�ڿ�ʼ����
    {
        thressload.In_I += IMC_Treated.gyro.z;
        thressload.In_turn_I = thressload.In_I * Angle_coe;
    }

    if(Ang_Velocity_Flag)	// ֱ�����ٶȻ�	1ms
    {
        Ang_Velocity_Flag = 0;
        get_icm20602_gyro_spi();
        get_icm20602_accdata_spi();
        Data_Filter();
        /* ���ٶȻ���Ϊ���ڻ�����ֱ�� */
        Theory_Duty -= PID_Increase(&Ang_Vel_PID, Ang_Vel, (IMC_Treated.gyro.y * 10), Tar_Ang_Vel.y);	// ����ֱ��PWM
        Theory_Duty  = range_protect(Theory_Duty, -990, 990);
			
        Direct_Parameter = -PID_Realize1(&Direct_PID, Direct, IMC_Treated.gyro.z * 100, Speed_Min * Radius);	// ת�������Ҹ�
        Direct_Parameter = range_protect(Direct_Parameter, -600, 600);

        Direct_Last = 0.8 * Direct_Parameter + 0.2 * Direct_Last;	// �����ϴν��ٶȻ����
        MOTOR_Duty_Left  = - Theory_Duty - Direct_Last;	// ���ҵ������ת��ϵ���������� Theory_Duty
        MOTOR_Duty_Right = - Theory_Duty + Direct_Last;
        //		if( ( (IMC_Treated.accdata.x < 0.1) && (IMC_Treated.accdata.x > -0.1) ) || (distance_I < 5000 ) )
        //		{
        //			MOTOR_Duty_Left  = - Theory_Duty - Direct_Last;	// ���ҵ������ת��ϵ���������� Theory_Duty
        //			MOTOR_Duty_Right = - Theory_Duty + Direct_Last;
        //		}
        //		else if( (IMC_Treated.accdata.x < 1.5) && (IMC_Treated.accdata.x > 0.1) )
        //		{
        //			MOTOR_Duty_Left  = - Theory_Duty - Direct_Last;	// ���ҵ������ת��ϵ���������� Theory_Duty
        //			MOTOR_Duty_Right = - Theory_Duty + Direct_Last + 300;
        //		}
        //		else if( (IMC_Treated.accdata.x < 3.5) && (IMC_Treated.accdata.x > 1.5) )
        //		{
        //			MOTOR_Duty_Left  = - Theory_Duty - Direct_Last;	// ���ҵ������ת��ϵ���������� Theory_Duty
        //			MOTOR_Duty_Right = - Theory_Duty + Direct_Last + 300;
        //		}
        //		else if( (IMC_Treated.accdata.x < 5.5) && (IMC_Treated.accdata.x > 3.5) )
        //		{
        //			MOTOR_Duty_Left  = - Theory_Duty - Direct_Last;	// ���ҵ������ת��ϵ���������� Theory_Duty
        //			MOTOR_Duty_Right = - Theory_Duty + Direct_Last + 300;
        //		}
        //		else if(IMC_Treated.accdata.x > 5.5)
        //		{
        //			MOTOR_Duty_Left  = - Theory_Duty - Direct_Last;	// ���ҵ������ת��ϵ���������� Theory_Duty
        //			MOTOR_Duty_Right = - Theory_Duty + Direct_Last + 300;
        //		}
        //		else if( (IMC_Treated.accdata.x > -1.5) && (IMC_Treated.accdata.x < -0.1) )
        //		{
        //			MOTOR_Duty_Left  = - Theory_Duty - Direct_Last + 300;	// ���ҵ������ת��ϵ���������� Theory_Duty
        //			MOTOR_Duty_Right = - Theory_Duty + Direct_Last;
        //		}
        //		else if( (IMC_Treated.accdata.x > -3.5) && (IMC_Treated.accdata.x < -1.5) )
        //		{
        //			MOTOR_Duty_Left  = - Theory_Duty - Direct_Last + 300;	// ���ҵ������ת��ϵ���������� Theory_Duty
        //			MOTOR_Duty_Right = - Theory_Duty + Direct_Last;
        //		}
        //		else if( (IMC_Treated.accdata.x > -5.5) && (IMC_Treated.accdata.x < -3.5) )
        //		{
        //			MOTOR_Duty_Left  = - Theory_Duty - Direct_Last + 300;	// ���ҵ������ת��ϵ���������� Theory_Duty
        //			MOTOR_Duty_Right = - Theory_Duty + Direct_Last;
        //		}
        //		else if(IMC_Treated.accdata.x < -5.5)
        //		{
        //			MOTOR_Duty_Left  = - Theory_Duty - Direct_Last + 300;	// ���ҵ������ת��ϵ���������� Theory_Duty
        //			MOTOR_Duty_Right = - Theory_Duty + Direct_Last;
        //		}

        if(Run_Flag)
        {
            MOTOR_Control(MOTOR_Duty_Left, MOTOR_Duty_Right);	// �������ҵ��
        }
        else
        {
            MOTOR_Control(0, 0);
        }
        Get_Attitude();
    }

    if(Angle_Flag)		// ֱ���ǶȻ�	5ms
    {
        Angle_Flag = 0;
        /* �ǶȻ��ӵ����ٶȻ��ϴ������� */
        Tar_Ang_Vel.y = -PID_Realize2(&Angle_PID, Angle, (int32)(Attitude_Angel_P * 100), Target_Angel_P);	// ���Ϊ�Ŵ�10����Ŀ����ٶ�
        Tar_Ang_Vel.y = range_protect(Tar_Ang_Vel.y, -1500, 1500);	// ע��������
    }

    if(Speed_Flag)		// �ٶȻ�	50ms
    {
        Speed_Flag = 0;
        SpeedS();
        Speed_Measure();
        /* �ٶȻ��ӵ��ǶȻ��ϴ������� */
        //		Target_Angel_P =  PID_Realize3(&MOTOR_PID, MOTOR,Speed_Now, Speed_Set);	// ���Ϊ�Ŵ�100����Ŀ��Ƕ�
        //		Target_Angel_P += Zero_Angle*100;	// Ŀ��Ƕȵ����������
        //		Target_Angel_P = range_protect((int32)Target_Angel_P, -2601, -2600);	// -44 22
        ////		if(distance_I >39000 && distance_I <40000 )
        //		{
        //
        //		}
        //		if( ( distance_I >37000) && ( distance_I < 39000 ) )
        //		{
        //			gpio_set(A15,1);
        //		}
        //		if( distance_I > 45000 )
        //		{
        //			gpio_set(A15,1);
        //			Target_Angel_P=	4500;
        //		}
        //		if(thressload.Target_flag)
        //		{
        //			Target_Angel_P = 5000;
        //		}
    }
}


/* ��ʼ���õ���һЩ���� */
void Balance_Init(void)
{
    Attitude_Angel_P = 0;
    Target_Angel_P = 0;
    Tar_Ang_Vel.y = 0;
    Tar_Ang_Vel.z = 0;
}

void Communication(void)
{
    if(CommuInfo.InfoState == 'T')
    {
        if(CommuInfo.ID == 1)
        {
            if(CommuInfo.ConmmuState == 1)
            {
                Communication_G_Flag = 1;
            }
        }
    }
    if(Communication_I > 10000) //Communication_S_Flag)
    {
        CommuInfo.ConmmuState = 2;
        InformationSend(&CommuTxBuff[0], &CommuInfo);
    }
}



