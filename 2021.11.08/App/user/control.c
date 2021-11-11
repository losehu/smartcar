#include "control.h"
int Ackerman_kl = 220;//左右差速系数
int Ackerman_kr = 220;
int PDout;
int kk = 24;
int Radius = 0; //PID偏差
float set_angle = 39;
int  Direct_Parameter = 0;// 转向系数;
int Direct_Last = 0;

int  angle_out = 0, v_out = 0;
float  angle_v_out = 0;
int Speed_diff_l = 0;//左差速
int Speed_diff_r = 0;//右差速
CarInfoTypedef   CarInfo;
PID Angle_PID, Ang_Vel_PID, Vel_PID, Turn_PID, Direct_PID;

//初始化一些设置 302
uint8 CarSystem_Init(void)
{
    //设置标志
    CarInfo.Protect = 'F';
    CarInfo.ControlMode = CAMMODE;
    CarInfo.ClosedLoopMode = AllLoop;
    CarInfo.CAR_PROTECT_MODE = 'F';//开启电磁信号削减保护
    CarInfo.StopTime = 0.000f;
    CarInfo.ReadFlashParams = 'F';
    CarInfo.Camera_FPS = 150;
    return 1;
}
//定时器中断 1ms
static int16 TIMx_Cnt = 0;
int16 DirectOut = 0;
int16 SpeedOut =  0;
int16 ExSpeed = NormalSpeed;
int16 L_ExSpeed = 0;
int16 R_ExSpeed = 0;
int16 L_SingleControlOut = 0, R_SingleControlOut = 0, BalanceControlOut = 0, Balance_L_Out = 0, Balance_R_Out = 0;
uint8 WaveWriteOK = 0;
float Gyro_Z_out = 0;
float GYRO_Z_Last = 0.000f;
int16 RiseHeadOut = 0;

void Timer_IT(void)  //TM6中断
{
	  	rt_interrupt_enter();
    //time
	if(ABS(Wheel.now) >80)
	{
		    pwm_l =0;
    pwm_r = 0;
		 PWM_Write();
     ips114_clear(RED);
		ips114_showstr(0,0,"speed die!!!");
		while(1)
		{
			
		}
	}
    TIMx_Cnt ++;
    CarInfo.UpTime += 0.001f;
    BeepInt();
    KeyScanInt();
    //-----------------------------------直立控制------------------------------------//
    //陀螺仪数值获取
    Data_Filter();//均值滤波
    ICM_I_Int();    //角度积分
    IMUupdate(ICM_Treated.gyro.x * 0.01745, ICM_Treated.gyro.y * 0.01745, ICM_Treated.gyro.z * 0.01745, ICM_Treated.accdata.x, ICM_Treated.accdata.y, ICM_Treated.accdata.z);   //更新角度

    //角速度内环 1ms
    angle_v_out = rubo_PID(&Ang_Vel_PID, Ang_Vel, ICM_State.gyro.y, angle_out);  // 结果为PWM输出
    angle_v_out = range_protect(angle_v_out, -990, 990);
    //角度内环 5ms
    if (TIMx_Cnt % 5 == 0 && CarInfo.Protect == 'F')
    {
        angle_out = -30 * PID_Realize2(&Angle_PID, Angle, Attitude_Angel_P * 300, v_out/*set_angle * 300*/);// 结果为目标角速度
        angle_out = range_protect(angle_out, -3000, 3000);
    }
    //-----------------------------------速度控制------------------------------------//

		
    //速度最外环 10ms
    if (TIMx_Cnt % 10 == 0 && CarInfo.Protect == 'F')
    {
        Encoder_Read();
        v_out = rubo_PID(&Vel_PID, Vel, Wheel.now * 100,600); // 结果为放大300倍的目标角度 350
        v_out += set_angle * 300; // 目标角度叠加在零点上
        v_out = range_protect(v_out, 7500, 13500);  // -5*300=-1500 -65*300=19500, 33~~40
    }
		
		
		
    //-----------------------------------转向控制------------------------------------//
    Direct_Parameter =- PID_Realize1(&Direct_PID, Direct, ICM_Treated.gyro.z ,Wheel.now * Radius/10);	// 转向角速度环左正右负   
		//    Direct_Parameter = PID_Realize1(&Direct_PID, Direct,point_center,0);	 
		 //   Direct_Parameter = -rubo_PID(&Direct_PID, Direct, ICM_Treated.gyro.z ,range_protect( Radius/Wheel.now,0,300) );	// 转向角速度环左正右负   

		Direct_Parameter = range_protect(Direct_Parameter, -500, 500);
    Direct_Last = 0.8 * Direct_Parameter + 0.2 * Direct_Last;	// 更新上次角速度环结果






    /**********************************************************************************/
    //波形发送标志 10ms
    if (TIMx_Cnt % 10 == 0)
    {
        WaveWriteOK = 1;
    }
    //清零计数 1s
    if (TIMx_Cnt >= 1000)
    {
        LED0_TOGGLE;
        TIMx_Cnt = 0;
        CarOutProtectCnt = 0;
    }
    //限幅
    if (CarInfo.Protect == 'F')
    {
        if (CarInfo.ClosedLoopMode == AllLoop || 1)
        {
            pwm_l = angle_v_out -Direct_Parameter;
            pwm_r = angle_v_out+Direct_Parameter;

					
        }
        /* else if (CarInfo.ClosedLoopMode == SpeedLoop)
           {
               pwm_l = + L_SingleControlOut;
               pwm_r = + R_SingleControlOut;
           }
           else if (CarInfo.ClosedLoopMode == DirectLoop)
           {
               pwm_l = 0;
               pwm_r = 0;
           }
           else if (CarInfo.ClosedLoopMode == LR_SingleControl)
           {
               pwm_l = L_SingleControlOut;
               pwm_r = R_SingleControlOut;
        	  }*/
    }
    pwm_l = range_protect(pwm_l, -1000, 1000);
    pwm_r = range_protect(pwm_r, -1000, 1000);
    if (CarInfo.UpTime >= 1.000f)  PWM_Write();
		  	rt_interrupt_leave();
}

//中线计算
int32 point_center;
int32 point_center0;
int32 point_center1;
int32 point_center2;
uint8 Center_Cal(void)
{
    //计算当前行的偏移量
    float excursion[60];
    float sumcenter = 0;
//    //三岔出入中线计算
//    if (Fork.FindFlag == 'T' && (Fork.state == ForkInTurn || Fork.state == ForkOutTurn))
//    {
//        if (Fork.Peak.y <= 15)
//        {
//            if (Fork.In_Direct == 'R')
//                point_center = 60;
//            else
//                point_center = -60;
//            return 1;
//        }
//        for (Ysite = 5; Ysite < 15; Ysite ++)
//        {
//            excursion[Ysite] = (float)(Img.Center_Repair[Ysite] - 64) / (float)(RoadWide0[Ysite] / 2) * 100;
//        }
//        excursion[5] = 0.000f;
//        excursion[6] = 0.000f;
//        excursion[7] = 0.000f;
//        for (Ysite = 5; Ysite < 15 ; Ysite ++)
//        {
//            sumcenter += excursion[Ysite];
//        }
//        if (Fork.state == ForkInTurn)
//            point_center = (int32)(sumcenter / 10 * 3.0f);
//        else if (Fork.state == ForkOutTurn)
//            point_center = (int32)(sumcenter / 10 * 2.2f);
//        if (Fork.In_Direct == 'R')
//            point_center = range_protect(point_center, -100, 300);
//        else
//            point_center = range_protect(point_center, -300, 100);
//    }
//    //出入库中线计算
//    else if (Barn.FindFlag == 'T' && (Barn.state == BarnOutTurn || Barn.state == BarnInTurn))
//    {
//        for (Ysite = 10; Ysite < 25; Ysite ++)
//        {
//            excursion[Ysite] = (float)(Img.Center_Repair[Ysite] - 64) / (float)(RoadWide0[Ysite] / 2) * 100;
//        }
//        excursion[10] = 0.000f;
//        excursion[11] = 0.000f;
//        excursion[12] = 0.000f;
//        for (Ysite = 10; Ysite < 25 ; Ysite ++)
//        {
//            sumcenter += excursion[Ysite];
//        }
//        if (Barn.state == BarnOutTurn)
//            point_center = (int32)(sumcenter / 15 * 1.0f);
//        else if (Barn.state == BarnInTurn)
//        {
//            if (Barn.OutDir == 'R')
//                point_center = 200;
//            else if (Barn.OutDir == 'L')
//                point_center = -200;
//        }

//    }
//    //入环岛转向
//    else if (Circle.FindFlag == 'T' && Circle.state == CircleInTurn)
//    {
//        uint8 StartCalLine = 0, EndCalLine = 0;
//        if (Circle.RightUpInflexion.y >= 16 && Circle.dir == 'R')
//        {
//            EndCalLine = Circle.RightUpInflexion.y;
//            StartCalLine = EndCalLine - 10;
//        }
//        else if (Circle.LeftUpInflexion.y >= 16 && Circle.dir == 'L')
//        {
//            EndCalLine = Circle.LeftUpInflexion.y;
//            StartCalLine = EndCalLine - 10;
//        }
//        else if (Circle.state == CircleInTurn)
//        {
//            if (Circle.dir == 'R')
//                point_center = 100;
//            else
//                point_center = -100;
//            return 1;
//        }
//        //计算
//        for (Ysite = StartCalLine; Ysite < EndCalLine; Ysite ++)
//        {
//            excursion[Ysite] = (float)(Img.Center_Repair[Ysite] - 64) / (float)(RoadWide0[Ysite] / 2) * 100;
//        }
//        excursion[StartCalLine] = 0.000f;
//        excursion[StartCalLine + 1] = 0.000f;
//        excursion[StartCalLine + 2] = 0.000f;
//        for (Ysite = StartCalLine; Ysite < EndCalLine; Ysite ++)
//        {
//            sumcenter += excursion[Ysite];
//        }
//        point_center = (int32)(sumcenter / 10 * Circle.InTurnK);
//        if (Circle.dir == 'R')
//        {
//            point_center = range_protect(point_center, 0, 300);
//        }
//        if (Circle.dir == 'L')
//        {
//            point_center = range_protect(point_center, -300, 0);
//        }
//    }
//    //出环岛
//    else if (Circle.FindFlag == 'T' && Circle.state == CircleOutTurn)
//    {
//        for (Ysite = 10; Ysite < 20; Ysite ++)
//        {
//            excursion[Ysite] = (float)(Img.Center_Repair[Ysite] - 64) / (float)(RoadWide0[Ysite] / 2) * 100;
//        }
//        excursion[10] = 0.000f;
//        excursion[11] = 0.000f;
//        excursion[12] = 0.000f;
//        for (Ysite = 10; Ysite < 20 ; Ysite ++)
//        {
//            sumcenter += excursion[Ysite];
//        }
//        point_center = (int32)(sumcenter / 10 * Circle.OutTurnK);
//        if (Circle.dir == 'R')
//        {
//            point_center = range_protect(point_center, -100, 300);
//        }
//        if (Circle.dir == 'L')
//        {
//            point_center = range_protect(point_center, -300, 100);
//        }
//    }
//    //环内巡线
//    else if (Circle.FindFlag == 'T' && Circle.state == CircleIn)
//    {
//        for (Ysite = 5; Ysite < 15; Ysite ++)
//        {
//            excursion[Ysite] = (float)(Img.Center_Repair[Ysite] - 64) / (float)(RoadWide0[Ysite] / 2) * 100;
//        }
//        excursion[5] = 0.000f;
//        excursion[6] = 0.000f;
//        for (Ysite = 5; Ysite < 15 ; Ysite ++)
//        {
//            sumcenter += excursion[Ysite];
//        }
//        point_center = (int32)(sumcenter / 10 * 2.0f);
//    }
//    //入环直道
//    else if (Circle.FindFlag == 'T' && \
//             (Circle.state == CircleInStraight || Circle.state == CircleOutCenter || Circle.state == CircleOutStraight))/* 前瞻 15 - 25 */
//    {
//        for (Ysite = 15; Ysite < 25; Ysite ++)
//        {
//            excursion[Ysite] = (float)(Img.Center_Repair[Ysite] - 64) / (float)(RoadWide0[Ysite] / 2) * 100;
//        }
//        excursion[15] = 0.000f;
//        excursion[16] = 0.000f;
//        excursion[17] = 0.000f;
//        for (Ysite = 15; Ysite < 25 ; Ysite ++)
//        {
//            sumcenter += excursion[Ysite];
//        }
//        point_center0 = (int32)(sumcenter / 10 * 2.0f);
//        point_center1 = point_center0;
//        point_center2 = point_center1;
//        point_center = point_center0 * 0.7f + point_center1 * 0.2f + point_center2 * 0.1f;
//        point_center *= 0.5f;

//    }
//    //普通赛道计算
//    else/* 前瞻 15 - 25 *///15-30
//    {
        for (Ysite = 15; Ysite < 40; Ysite ++)
        {
            excursion[Ysite] = (float)(Img.Center_Repair[Ysite] - 64) / (float)(RoadWide0[Ysite] / 2) * 100;
        }
        excursion[15] = 0.000f;
        excursion[16] = 0.000f;
        excursion[17] = 0.000f;
        for (Ysite = 15; Ysite < 40 ; Ysite ++)
        {
            sumcenter += excursion[Ysite];
        }
        point_center0 = (int32)(sumcenter / 10 * 2.0f);
        point_center1 = point_center0;
        point_center2 = point_center1;
        point_center = point_center0 * 0.7f + point_center1 * 0.2f + point_center2 * 0.1f;
   // }
    return 1;
}





//冲出赛道电磁削减保护
int16 CarOutProtectCnt = 0;
void Car_Protect(void)
{
    if ((CarInfo.UpTime >= 2.000f) && ((CarInfo.ClosedLoopMode == AllLoop) || \
                                       (CarInfo.ClosedLoopMode == SpeedLoop)))
    {
        //---------------电磁保护--------------//
        if (CarInfo.CAR_PROTECT_MODE == 'T')
        {
            CarOutProtectCnt ++;
        }
        if (CarOutProtectCnt >= 5)
        {
            CarInfo.Protect = 'T';
        }
    }
    if (CarInfo.StopTime != 0.000f && CarInfo.UpTime >= CarInfo.StopTime)
    {
        CarInfo.Protect = 'T';
        Base.element_check = 'F';
    }
}

//显示界面调整参数
const uint8 CarInfoItem0[] = "Protect En:";
const uint8 CarInfoItem1[] = "Control Loop:";
const uint8 CarInfoItem2[] = "Params Read:";

#define CarInfo_MENU_NUM 2
//#define ControlMood_MENU_NUM 4

uint8 CarInfo_Show(void)
{
    while (KeyPageUp != onepress)
    {
        static int8 NowCarInfoItem = 0;
        Menu_Choose(&NowCarInfoItem, CarInfo_MENU_NUM);
        //显示条目
        IPS114_PENCOLOR = RED;
        ips114_showstr(16, 0, CarInfoItem0);
        ips114_showstr(16, 1, CarInfoItem1);
        ips114_showstr(16, 2, CarInfoItem2);
        //显示参数
        IPS114_PENCOLOR = BLUE;
        if (CarInfo.CAR_PROTECT_MODE == 'T')
            ips114_showstr(128, 0, "T");
        else
            ips114_showstr(128, 0, "F");

        if (CarInfo.ClosedLoopMode == AllLoop)
            ips114_showstr(128, 1, "Allloop");
        else if (CarInfo.ClosedLoopMode == SpeedLoop)
            ips114_showstr(128, 1, "SpeedLoop");
        else if (CarInfo.ClosedLoopMode == DirectLoop)
            ips114_showstr(128, 1, "DirectLoop");
        else if (CarInfo.ClosedLoopMode == NOLoop)
            ips114_showstr(128, 1, "NOLoop");
        if (CarInfo.ReadFlashParams == 'T')
            ips114_showstr(128, 2, "T");
        else
            ips114_showstr(128, 2, "F");
        //调参确认
        if (KeyPageDown == onepress)
        {
            KeyPageDown = nopress;
            switch (NowCarInfoItem)
            {
            case (0):
            {
                StateAdjust(&CarInfo.CAR_PROTECT_MODE, 128, 0);
                break;
            }

            case (2):
            {
                StateAdjust(&CarInfo.ReadFlashParams, 128, 2);
                break;
            }
            }
        }
        delay_ms(10);
    }
    KeyPageUp = nopress;
    return 1;
}










