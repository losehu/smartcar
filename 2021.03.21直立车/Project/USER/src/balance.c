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
      Direct_Parameter = 0,// 转向系数;
      Radius = 0,         // 目标转向半径倒数
      Speed_Now = 0,	// 当前实际速度
      Speed_Now_Last = 0,     // 上次实际速度
      Speed_Min = 0,	// 左右最小速度
      Speed_Set = 300, 	// 目标设定速度
      Run_I = 0,
      O_I = 60;


int32 Theory_Duty = 0;// 理论直立占空比

S_INT32_XYZ
Tar_Ang_Vel,	// 目标角速度
Tar_Ang_Vel_Last;	// 上次目标角速度

/********************* 串级平衡控制 *********************/
void Balance_Control(void)
{
    Target_Angel_P = -2200;
    //	if(roundabout.frequency)//进一次环岛
    //	{
    Huandao();
    //	}
    //	Thressload();
    //	Rampway();
    if(roundabout.In_flag)//进圆环积分
    {
        roundabout.In_I += IMC_Treated.gyro.z;
        roundabout.In_turn_I = roundabout.In_I * Angle_coe;
    }
    if(thressload.In_flag)//进三叉路口开始积分
    {
        thressload.In_I += IMC_Treated.gyro.z;
        thressload.In_turn_I = thressload.In_I * Angle_coe;
    }

    if(Ang_Velocity_Flag)	// 直立角速度环	1ms
    {
        Ang_Velocity_Flag = 0;
        get_icm20602_gyro_spi();
        get_icm20602_accdata_spi();
        Data_Filter();
        /* 角速度环作为最内环控制直立 */
        Theory_Duty -= PID_Increase(&Ang_Vel_PID, Ang_Vel, (IMC_Treated.gyro.y * 10), Tar_Ang_Vel.y);	// 计算直立PWM
        Theory_Duty  = range_protect(Theory_Duty, -990, 990);
			
        Direct_Parameter = -PID_Realize1(&Direct_PID, Direct, IMC_Treated.gyro.z * 100, Speed_Min * Radius);	// 转向环左正右负
        Direct_Parameter = range_protect(Direct_Parameter, -600, 600);

        Direct_Last = 0.8 * Direct_Parameter + 0.2 * Direct_Last;	// 更新上次角速度环结果
        MOTOR_Duty_Left  = - Theory_Duty - Direct_Last;	// 左右电机根据转向系数调整差速 Theory_Duty
        MOTOR_Duty_Right = - Theory_Duty + Direct_Last;
        //		if( ( (IMC_Treated.accdata.x < 0.1) && (IMC_Treated.accdata.x > -0.1) ) || (distance_I < 5000 ) )
        //		{
        //			MOTOR_Duty_Left  = - Theory_Duty - Direct_Last;	// 左右电机根据转向系数调整差速 Theory_Duty
        //			MOTOR_Duty_Right = - Theory_Duty + Direct_Last;
        //		}
        //		else if( (IMC_Treated.accdata.x < 1.5) && (IMC_Treated.accdata.x > 0.1) )
        //		{
        //			MOTOR_Duty_Left  = - Theory_Duty - Direct_Last;	// 左右电机根据转向系数调整差速 Theory_Duty
        //			MOTOR_Duty_Right = - Theory_Duty + Direct_Last + 300;
        //		}
        //		else if( (IMC_Treated.accdata.x < 3.5) && (IMC_Treated.accdata.x > 1.5) )
        //		{
        //			MOTOR_Duty_Left  = - Theory_Duty - Direct_Last;	// 左右电机根据转向系数调整差速 Theory_Duty
        //			MOTOR_Duty_Right = - Theory_Duty + Direct_Last + 300;
        //		}
        //		else if( (IMC_Treated.accdata.x < 5.5) && (IMC_Treated.accdata.x > 3.5) )
        //		{
        //			MOTOR_Duty_Left  = - Theory_Duty - Direct_Last;	// 左右电机根据转向系数调整差速 Theory_Duty
        //			MOTOR_Duty_Right = - Theory_Duty + Direct_Last + 300;
        //		}
        //		else if(IMC_Treated.accdata.x > 5.5)
        //		{
        //			MOTOR_Duty_Left  = - Theory_Duty - Direct_Last;	// 左右电机根据转向系数调整差速 Theory_Duty
        //			MOTOR_Duty_Right = - Theory_Duty + Direct_Last + 300;
        //		}
        //		else if( (IMC_Treated.accdata.x > -1.5) && (IMC_Treated.accdata.x < -0.1) )
        //		{
        //			MOTOR_Duty_Left  = - Theory_Duty - Direct_Last + 300;	// 左右电机根据转向系数调整差速 Theory_Duty
        //			MOTOR_Duty_Right = - Theory_Duty + Direct_Last;
        //		}
        //		else if( (IMC_Treated.accdata.x > -3.5) && (IMC_Treated.accdata.x < -1.5) )
        //		{
        //			MOTOR_Duty_Left  = - Theory_Duty - Direct_Last + 300;	// 左右电机根据转向系数调整差速 Theory_Duty
        //			MOTOR_Duty_Right = - Theory_Duty + Direct_Last;
        //		}
        //		else if( (IMC_Treated.accdata.x > -5.5) && (IMC_Treated.accdata.x < -3.5) )
        //		{
        //			MOTOR_Duty_Left  = - Theory_Duty - Direct_Last + 300;	// 左右电机根据转向系数调整差速 Theory_Duty
        //			MOTOR_Duty_Right = - Theory_Duty + Direct_Last;
        //		}
        //		else if(IMC_Treated.accdata.x < -5.5)
        //		{
        //			MOTOR_Duty_Left  = - Theory_Duty - Direct_Last + 300;	// 左右电机根据转向系数调整差速 Theory_Duty
        //			MOTOR_Duty_Right = - Theory_Duty + Direct_Last;
        //		}

        if(Run_Flag)
        {
            MOTOR_Control(MOTOR_Duty_Left, MOTOR_Duty_Right);	// 控制左右电机
        }
        else
        {
            MOTOR_Control(0, 0);
        }
        Get_Attitude();
    }

    if(Angle_Flag)		// 直立角度环	5ms
    {
        Angle_Flag = 0;
        /* 角度环加到角速度环上串级控制 */
        Tar_Ang_Vel.y = -PID_Realize2(&Angle_PID, Angle, (int32)(Attitude_Angel_P * 100), Target_Angel_P);	// 结果为放大10倍的目标角速度
        Tar_Ang_Vel.y = range_protect(Tar_Ang_Vel.y, -1500, 1500);	// 注意正负号
    }

    if(Speed_Flag)		// 速度环	50ms
    {
        Speed_Flag = 0;
        SpeedS();
        Speed_Measure();
        /* 速度环加到角度环上串级控制 */
        //		Target_Angel_P =  PID_Realize3(&MOTOR_PID, MOTOR,Speed_Now, Speed_Set);	// 结果为放大100倍的目标角度
        //		Target_Angel_P += Zero_Angle*100;	// 目标角度叠加在零点上
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


/* 初始化用到的一些变量 */
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



