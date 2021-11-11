#include "pid.h"


//PID参数变化

#define 	SPEEDLOOP_Kp		0
#define 	SPEEDLOOP_Ki    	1
#define 	SPEEDLOOP_Kd    	2
#define 	SPEEDLOOP_MaxO		3
#define 	SPEEDLOOP_MinO		4

/*******************速度环********************/
float SpeedPIDParam[][5] =
{
    /*   Kp       Ki       Kd     MaxO    MinO */
    {  3.000f , 0.300f , 0.100f , 0400.0f , -0400.0f  },//0.120f , 0.150f , 1.600f , 0400.0f , -0400.0f
    {  2.000f , 0.600f , 0.100f , 0400.0f , -0400.0f  },
};

/**************速度环(单电机控制)*************/
//左电机
float LeftMotorSpeedPIDParam[][5] =
{
    /*   Kp       Ki       Kd     MaxO    MinO */
    {  3.000f , 0.300f , 0.000f , 1000.0f , -1000.0f }
};


//右电机
float RightMotorSpeedPIDParam[][5] =
{
    /*   Kp       Ki       Kd     MaxO    MinO */
    {  2.000f , 0.600f , 2.000f , 1000.0f , -1000.0f  }
};

/**************舵机控制************************/

#define		SERVOCONTROL_Kp1	0
#define		SERVOCONTROL_Kp2	1
#define		SERVOCONTROL_Kd		2
#define		SERVOCONTROL_Ki		3
#define		SERVOCONTROL_SumMax 4
#define		SERVOCONTROL_SumMin 5
#define		SERVOCONTROL_MaxO	6
#define		SERVOCONTROL_MinO	7

float ServoPIDParam[][8] =
{
    /*    Kp1       Kp2         Ki         Kd        SumMax       SumMin       MaxO      MinO */
    {  2.50000f , 1.00000f , 0.00000f , 000.00000f ,   0100.0f ,   -0100.0f  , 800.0f , -800.0f  },
    // 2.50000f , 5.50000f , 0.00000f , 000.00000f ,   0100.0f ,   -0100.0f  , 800.0f , -800.0f
    {  3.50000f , 1.75000f , 0.00000f , 080.00000f ,   0100.0f ,   -0100.0f  , 600.0f , -600.0f },
    {  12.0000f , 0.00000f , 0.00000f , 000.00000f ,   0100.0f ,   -0100.0f  , 600.0f , -600.0f },

};

/**************转向环(左加右减式)*************/

#define 	TURNLOOP_Kp1		0
#define 	TURNLOOP_Kp2    	1
#define 	TURNLOOP_Kp3    	2
#define 	TURNLOOP_Ki			3
#define		TURNLOOP_Kd			4
#define     TUNRLOOP_SumMax		5
#define 	TURNLOOP_SumMin		6
#define 	TURNLOOP_MaxO		7
#define 	TURNLOOP_MinO		8

float TurnPIDParam[][9] =
{
    /*    Kp1       Kp2       kp3         Ki         Kd        SumMax       SumMin       MaxO      MinO */
    {  2.50000f , 4.50000f , 0.04000f , 0.00000f , 090.00000f ,   0100.0f ,   -0100.0f  , 800.0f , -800.0f },
    //				   Kp2 5.00							Kd 40
    {  3.50000f , 1.75000f , 0.07000f , 0.00000f , 080.00000f ,   0100.0f ,   -0100.0f  , 800.0f , -800.0f },
    {  12.0000f , 0.00000f , 0.00000f , 0.00000f , 000.00000f ,   0100.0f ,   -0100.0f  , 800.0f , -800.0f },
};



//初始化PID的参数
void PID_Init(void)
{
    //--------------速度环---------------------//
    SpeedPID.maxout = 600;
    SpeedPID.minout = -600;
    SpeedPID.kp = 0.06f;
    SpeedPID.ki = 0.1f;
    SpeedPID.kd = 0.800f;
    //--------------控制停车用-----------------//
    SpeedPIDLeft.maxout = 1000;
    SpeedPIDLeft.minout = -1000;
    SpeedPIDLeft.kp = 2.0f;
    SpeedPIDLeft.ki = 0.3f;
    SpeedPIDRight.maxout = 1000;
    SpeedPIDRight.minout = -1000;
    SpeedPIDRight.kp = 3.0f;
    SpeedPIDRight.ki = 0.6f;
    SpeedPIDRight.lastError = 0;
    SpeedPIDRight.beforeError = 0;
    //--------------舵机控制环  摄像头----------------//
    ServoControlCam.maxout = 0;
    ServoControlCam.minout = 0;
    ServoControlCam.kp = 0;
    ServoControlCam.kp1 = 0;
    ServoControlCam.kp2 = 0;
    ServoControlCam.ki = 0;
    ServoControlCam.kd = 0;
    ServoControlCam.sumErrormax = 0;
    ServoControlCam.sumErrormin = 0;
    ServoControlCam.nowError = 0;
    ServoControlCam.lastError = 0;
    ServoControlCam.beforeError = 0;
    //--------------转向环  摄像头控制-----------------//
    TurnLoopCam.kp1 = 3.50000f;//3.1
    TurnLoopCam.kp2 = 4.40000f;//3.2
    TurnLoopCam.kp2_K = 1.000f;
    TurnLoopCam.kp3 = 0.03500f;//归一到 -6 ~ 6 0.00035
    TurnLoopCam.ki =  0.00000f;
    TurnLoopCam.kd = 40.00000f;
    TurnLoopCam.maxout = 1000;
    TurnLoopCam.minout = -1000;
    TurnLoopCam.sumError = 0;
    TurnLoopCam.sumErrormax = 100;
    TurnLoopCam.sumErrormin = -100;
    TurnLoopCam.Ud = 0.000f;
    TurnLoopCam.Ud_a = 0.200f;
    TurnLoopCam.Ud_last = 0.000f;
    TurnLoopCam.lastError = 0;
    TurnLoopCam.beforeError = 0;
    //---------------模糊PID------------//
    FuzzyPID_Init();
}


//------------------------普通增量式pid-------------------------//
SpeedPIDTypedef SpeedPID, SpeedPIDLeft, SpeedPIDRight;
int16 SimpleIncremental_PID(float *pid_param, SpeedPIDTypedef *pid, int16 _exval, int16 _nowval)
{
    //读取参数
    pid->kp = pid_param[SPEEDLOOP_Kp];
    pid->ki = pid_param[SPEEDLOOP_Ki];
    pid->kd = pid_param[SPEEDLOOP_Kd];
    pid->maxout = (int16)pid_param[SPEEDLOOP_MaxO];
    pid->minout = (int16)pid_param[SPEEDLOOP_MinO];

    //运算
    pid->exval = _exval;
    pid->nowval = _nowval;
    //当前误差
    pid->nowError =  pid->exval - pid->nowval;
    //增量
    pid->increase =
        pid->kp * (float)(pid->nowError - pid->lastError) +
        pid->ki * (float)pid->nowError +
        pid->kd * (float)(pid->nowError - 2 * pid->lastError + pid->beforeError);
    //更新误差
    pid->lastError = pid->nowError;
    pid->beforeError = pid->lastError;
    pid->out += (int16)pid->increase;
    //限制幅度
    pid->out = range_protect(pid->out, (int32)pid->minout, (int32)pid->maxout);
    //输出
    return pid->out;
}


//----------------------彬哥舵机pid控制 摄像头有效行---------------------------//
//非线性
ServoControlCamTypedef ServoControlCam;

int16 ServoPosition_PID(float *pid_param, ServoControlCamTypedef *pid, int16 _exval, int16 _nowval)
{
    //读取参数
    pid->kp1 = pid_param[SERVOCONTROL_Kp1];
    pid->kp2 = pid_param[SERVOCONTROL_Kp2];
    pid->ki = pid_param[SERVOCONTROL_Ki];
    pid->kd = pid_param[SERVOCONTROL_Kd];
    pid->sumErrormax = (int16)pid_param[SERVOCONTROL_SumMax];
    pid->sumErrormin = (int16)pid_param[SERVOCONTROL_SumMin];
    pid->maxout = (int16)pid_param[SERVOCONTROL_MaxO];
    pid->minout = (int16) pid_param[SERVOCONTROL_MinO];
    pid->exval = _exval;
    pid->nowval = _nowval;
    //当前误差
    pid->nowError =  pid->exval - pid->nowval;
    //误差积分
    pid->sumError += pid->nowError;
    //误差积分限幅
    pid->sumError = range_protect(pid->sumError, (int32)pid->sumErrormin, (int32)pid->sumErrormax);
    //计算Kp
    pid->kp = (float)(pid->kp1 * pid->nowError) + \
              (float)((pid->kp2 * (ROW - Base.Topline) * (ROW - Base.Topline) * pid->nowError) / 1000);
    //计算Ki
    //	pid->ki = (float)(pid->ki * (pid->nowError + pid->lastError + pid->beforeError));
    //计算Kd
    //	pid->kd = (float)(pid->kd * (pid->nowError - pid->lastError));
    //计算输出
    pid->out = (int16)(
                   pid->kp + \
                   (float)(pid->ki * (pid->nowError + pid->lastError + pid->beforeError)) + \
                   (float)(pid->kd * (pid->nowError - pid->lastError)));
    //更新误差
    pid->lastError = pid->nowError;
    pid->beforeError = pid->lastError;
    //限制幅度
    pid->out = range_protect(pid->out, (int32)pid->minout, (int32)pid->maxout);
    //输出
    return pid->out;
}


//----------------------位置式pid控制 摄像头---------------------------//
//非线性
TurnLoopCamTypedef TurnLoopCam;

int16 Position_PID(float *pid_param, TurnLoopCamTypedef *pid, int16 _exval, int16 _nowval)
{
    //读取参数
    pid->kp1 = pid_param[TURNLOOP_Kp1];
    pid->kp2 = pid_param[TURNLOOP_Kp2];
    pid->kp3 = pid_param[TURNLOOP_Kp3];
    pid->ki = pid_param[TURNLOOP_Ki];
    pid->kd = pid_param[TURNLOOP_Kd];
    pid->sumErrormax = (int16)pid_param[TUNRLOOP_SumMax];
    pid->sumErrormin = (int16)pid_param[TURNLOOP_SumMin];
    pid->maxout = (int16)pid_param[TURNLOOP_MaxO];
    pid->minout = (int16) pid_param[TURNLOOP_MinO];

    //运算
    pid->exval = _exval;
    pid->nowval = _nowval;
    //当前误差
    pid->nowError =  pid->exval - pid->nowval;
    //误差积分
    pid->sumError += pid->nowError;
    //误差积分限幅
    pid->sumError = range_protect(pid->sumError, (int32)pid->sumErrormin, (int32)pid->sumErrormax);
    //计算kp2
    pid->kp2Real = pid->kp2_K * pid->kp2;
    //计算kp_sum
    pid->kp = pid->kp1 + pid->kp2 * (1 - (2 / ( exp(pid->kp3 * pid->nowError) + \
                                          exp(- pid->kp3 * pid->nowError) ) ) );
    //计算Ud
    pid->Ud = pid->kd * (1.000f - pid->Ud_a) * (float)(pid->nowError - pid->lastError) +
              pid->Ud_a * pid->Ud_last;
    //计算输出
    pid->out = (int16)(
                   pid->kp * (float)pid->nowError + \
                   pid->ki * (float)pid->sumError + \
                   pid->Ud);
    //更新误差
    pid->lastError = pid->nowError;
    pid->beforeError = pid->lastError;
    //限制幅度
    pid->out = range_protect(pid->out, (int32)pid->minout, (int32)pid->maxout);
    //记录Ud
    pid->Ud_last = pid->out;
    //输出
    return pid->out;
}



//------------------------------------ 模糊PID ---------------------------------------------//

/*论域*/
#define 	NB		-6
#define 	NM 		-4
#define 	NS		-2
#define 	ZO		0
#define 	PS 		2
#define 	PM		4
#define 	PB		6


/*Kp规则表*/
int8 ruleKp[7][7] =
{

    /*  \ Ec */ 	 /* NB 负大   NM 负中  NS 负小   ZO 零  PS 正小  PM 正中  PB 正大 */
    /* E \ */		  	/* -6        -4        -2      0        2        4         6*/
    /* NB 负大 -6 */	  PB	,	 PB    ,  PM    ,  PB   ,   PM   ,   ZO   ,   ZO   ,
    /* NM 负中 -4 */	  PB	,	 PB    ,  PM    ,  PM   ,   PS   ,   ZO   ,   NS   ,
    /* NS 负小 -2 */	  PM	,	 PM    ,  PS    ,  PS   ,   ZO   ,   NS   ,   NS   ,
    /* ZO 零    0 */	  PM	,	 PM    ,  PS    ,  ZO   ,   NS   ,   NM   ,   NM   ,
    /* PS 正小  2 */	  PS	,	 PS    ,  ZO    ,  NS   ,   NS   ,   NM   ,   NM   ,
    /* PM 正中  4 */	  PS	,	 ZO    ,  NS    ,  NM   ,   NM   ,   NB   ,   NB   ,
    /* PB 正大  6 */	  ZO	,	 ZO    ,  NM    ,  NB   ,   NM   ,   NB   ,   NB   ,

};

/*Ki规则表*/
int8 ruleKi[7][7] =
{

    /*  \ Ec */ 	 /* NB 负大   NM 负中  NS 负小   ZO 零  PS 正小  PM 正中  PB 正大 */
    /* E \ */		  	/* -6        -4        -2      0        2        4         6*/
    /* NB 负大 -6 */	  PS	,	 NS    ,  NB    ,  NB   ,   NB   ,   NM   ,   PS   ,
    /* NM 负中 -4 */	  PS	,	 NS    ,  NB    ,  NM   ,   NM   ,   NS   ,   PS   ,
    /* NS 负小 -2 */	  ZO	,	 NS    ,  NM    ,  NM   ,   NS   ,   NS   ,   ZO   ,
    /* ZO 零    0 */	  ZO	,	 NS    ,  NS    ,  NS   ,   NS   ,   NS   ,   ZO   ,
    /* PS 正小  2 */	  ZO	,	 ZO    ,  ZO    ,  ZO   ,   ZO   ,   ZO   ,   ZO   ,
    /* PM 正中  4 */	  PB	,	 NS    ,  PS    ,  PS   ,   PS   ,   PS   ,   PB   ,
    /* PB 正大  6 */	  PB	,	 PM    ,  PM    ,  PM   ,   PS   ,   PS   ,   PB   ,

};


/*Kd规则表*/
int8 ruleKd[7][7] =
{

    /*  \ Ec */ 	 /* NB 负大   NM 负中  NS 负小   ZO 零  PS 正小  PM 正中  PB 正大 */
    /* E \ */		  	/* -6        -4        -2      0        2        4         6*/
    /* NB 负大 -6 */	  PB	,	 PB    ,  PM    ,  PB   ,   PM   ,   ZO   ,   ZO   ,
    /* NM 负中 -4 */	  PB	,	 PB    ,  PM    ,  PM   ,   PS   ,   ZO   ,   NS   ,
    /* NS 负小 -2 */	  PM	,	 PM    ,  PS    ,  PS   ,   ZO   ,   NS   ,   NS   ,
    /* ZO 零    0 */	  PM	,	 PM    ,  PS    ,  ZO   ,   NS   ,   NM   ,   NM   ,
    /* PS 正小  2 */	  PS	,	 PS    ,  ZO    ,  NS   ,   NS   ,   NM   ,   NM   ,
    /* PM 正中  4 */	  PS	,	 ZO    ,  NS    ,  NM   ,   NM   ,   NB   ,   NB   ,
    /* PB 正大  6 */	  ZO	,	 ZO    ,  NM    ,  NB   ,   NM   ,   NB   ,   NB   ,

};


//statement
FuzzyPID_Typedef  AngleLoop_Fuzzy;


/*初始化fuzzy pid*/
void FuzzyPID_Init(void)
{
    AngleLoop_Fuzzy.nowErrmax = 160;
    AngleLoop_Fuzzy.nowErrmin = -160;
    AngleLoop_Fuzzy.ECmax = 10;
    AngleLoop_Fuzzy.ECmin = -10;

    AngleLoop_Fuzzy.sumError = 0;
    AngleLoop_Fuzzy.outmax = 600;
    AngleLoop_Fuzzy.outmin = -600;
    AngleLoop_Fuzzy.kp1 = 4.3;//3.3
    AngleLoop_Fuzzy.ki1 = 0.0;//0.0
    AngleLoop_Fuzzy.kd1 = 0.15;//0.15
    AngleLoop_Fuzzy.kp2 = 8.0;//8.0
    AngleLoop_Fuzzy.ki2 = 0.0;//0.0
    AngleLoop_Fuzzy.kd2 = 0.0;//0.0

}


/*隶属度计算函数*/
void Calculate_Membership(float qv, int *index, float *ms)
{
    if((qv >= NB) && (qv < NM))
    {
        index[0] = 0;
        index[1] = 1;
        ms[0] = -0.5 * qv - 2.0; //y=-0.5x-2.0
        ms[1] = 0.5 * qv + 3.0; //y=0.5x+3.0
    }
    else if((qv >= NM) && (qv < NS))
    {
        index[0] = 1;
        index[1] = 2;
        ms[0] = -0.5 * qv - 1.0; //y=-0.5x-1.0
        ms[1] = 0.5 * qv + 2.0; //y=0.5x+2.0
    }
    else if((qv >= NS) && (qv < ZO))
    {
        index[0] = 2;
        index[1] = 3;
        ms[0] = -0.5 * qv;  //y=-0.5x
        ms[1] = 0.5 * qv + 1.0; //y=0.5x+1.0
    }
    else if((qv >= ZO) && (qv < PS))
    {
        index[0] = 3;
        index[1] = 4;
        ms[0] = -0.5 * qv + 1.0; //y=-0.5x+1.0
        ms[1] = 0.5 * qv;   //y=0.5x
    }
    else if((qv >= PS) && (qv < PM))
    {
        index[0] = 4;
        index[1] = 5;
        ms[0] = -0.5 * qv + 2.0; //y=-0.5x+2.0
        ms[1] = 0.5 * qv - 1.0; //y=0.5x-1.0
    }
    else if((qv >= PM) && (qv <= PB))
    {
        index[0] = 5;
        index[1] = 6;
        ms[0] = -0.5 * qv + 3.0; //y=-0.5x+3.0
        ms[1] = 0.5 * qv - 2.0; //y=0.5x-2.0
    }
}

//模糊PID实现
float FuzzyPID(FuzzyPID_Typedef *pid, float _nowval, float _exval)
{
    //读取数值
    pid->exval = _exval;
    pid->nowval = _nowval;
    //误差以及误差增量限幅
    pid->nowError = range_protect(pid->nowError, pid->nowErrmin, pid->nowErrmax);
    pid->ErrorInc = range_protect(pid->ErrorInc, pid->ECmin, pid->ECmax);
    //更新当前误差
    pid->nowError =  pid->exval - pid->nowval;
    pid->ErrorInc = pid->nowError - pid->lastError;
    /********  模糊化  *******/
    //量化 论域{-6，-5，-4，-3，-2，-1，0，1，2，3，4，5，6}
    pid->E_Quantized  = 6.0 * pid->nowError / pid->nowErrmax;
    pid->EC_Quantized = 6.0 * pid->ErrorInc / pid->ECmax;
    //计算隶属度
    Calculate_Membership(pid->E_Quantized, pid->E_Index, pid->E_Membership);
    Calculate_Membership(pid->EC_Quantized, pid->EC_Index, pid->EC_Membership);
    /********  解模糊  *******/
    //计算kx2的系数
    pid->kp2k =
        pid->E_Membership[0] * pid->EC_Membership[0] * ruleKp[pid->E_Index[0]][pid->EC_Index[0]] + \
        pid->E_Membership[0] * pid->EC_Membership[1] * ruleKp[pid->E_Index[0]][pid->EC_Index[1]] + \
        pid->E_Membership[1] * pid->EC_Membership[0] * ruleKp[pid->E_Index[1]][pid->EC_Index[0]] + \
        pid->E_Membership[1] * pid->EC_Membership[1] * ruleKp[pid->E_Index[1]][pid->EC_Index[1]] ;
    pid->ki2k =
        pid->E_Membership[0] * pid->EC_Membership[0] * ruleKi[pid->E_Index[0]][pid->EC_Index[0]] + \
        pid->E_Membership[0] * pid->EC_Membership[1] * ruleKi[pid->E_Index[0]][pid->EC_Index[1]] + \
        pid->E_Membership[1] * pid->EC_Membership[0] * ruleKi[pid->E_Index[1]][pid->EC_Index[0]] + \
        pid->E_Membership[1] * pid->EC_Membership[1] * ruleKi[pid->E_Index[1]][pid->EC_Index[1]] ;
    pid->kd2k =
        pid->E_Membership[0] * pid->EC_Membership[0] * ruleKd[pid->E_Index[0]][pid->EC_Index[0]] + \
        pid->E_Membership[0] * pid->EC_Membership[1] * ruleKd[pid->E_Index[0]][pid->EC_Index[1]] + \
        pid->E_Membership[1] * pid->EC_Membership[0] * ruleKd[pid->E_Index[1]][pid->EC_Index[0]] + \
        pid->E_Membership[1] * pid->EC_Membership[1] * ruleKd[pid->E_Index[1]][pid->EC_Index[1]] ;
    //计算总的kx
    pid->kp = pid->kp1 + pid->kp2 * (ABS_FLOAT(pid->kp2k) / 6.0);//除以论域极值 计算的是kp增量百分比
    pid->kd = pid->kd1 + pid->kd2 * (ABS_FLOAT(pid->kd2k) / 6.0);//除以论域极值 计算的是kp增量百分比
    //计算输出
    pid->out =
        pid->kp * (float)pid->nowError + \
        pid->kd * (float)(pid->nowError - pid->lastError);
    //更新误差
    pid->lastError = pid->nowError;
    pid->preError = pid->lastError;
    //输出限幅
    pid->out = range_protect(pid->out, (int32)pid->outmin, (int32)pid->outmax);
    //输出
    return pid->out;
}

//void Fuzzy_PID_Test(void)
//{
//	FuzzyPID(&AngleLoop_Fuzzy,Wheel.RealSpeedNow,280);
//}


