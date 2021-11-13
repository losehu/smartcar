#include "pid.h"


//PID参数变化

#define     SPEEDLOOP_Kp        0
#define     SPEEDLOOP_Ki        1
#define     SPEEDLOOP_Kd        2
#define     SPEEDLOOP_MaxO      3
#define     SPEEDLOOP_MinO      4
//P  I  D    MAX_SUM_ERROR
float Angle[4]   = {1.3, 0.01, 0.0010, 1};      // 角度环PID{0.2, 0, 0.03, 1000};
float Ang_Vel[4] = {0.06, 0.003, 0.00000, 500};   // 角速度环PID {0.2, 0.007, 0, 500};
float Vel[4] = {6, 0.017, 0.001, 1000};   // 速度环PID {0.2, 0.007, 0, 500};
float rubo_Vel[4]= {0.52, 0.00150, 0.01, 100}; //转向角度环
//float Direct[4]  = {5, 0.01, 0.000, 70};	// 转向角速度环PID 位置{0.018, 0.00215, 0.023, 55};
float Direct[4]  = {4, 0.028, 0.000, 200};	// 转向角速度环PID 位置{0.018, 0.00215, 0.023, 55};

//环岛转向参数PID
/*******************速度环********************/
float SpeedPIDParam[][5] =
{
    /*   Kp       Ki       Kd     MaxO    MinO */
    {  3.000f, 0.300f, 0.100f, 0400.0f, -0400.0f  },    //0.120f , 0.150f , 1.600f , 0400.0f , -0400.0f
    {  2.000f, 0.600f, 0.100f, 0400.0f, -0400.0f  },
};

/**************速度环(单电机控制)*************/
//左电机
float LeftMotorSpeedPIDParam[][7] =
{
    /*   Kp       Ki       Kd     MaxO    MinO */
    {  3.000f, 6.00f, 0.000f, 300, -300, 1000.0f, -1000.0f }
};


//右电机
float RightMotorSpeedPIDParam[][7] =
{
    /*   Kp       Ki       Kd     MaxO    MinO */
    {  3.000f, 0.600f, 2.000f, 300, -300, 1000.0f, -1000.0f }
};

float BalancePIDParam[][7] =
{

    /*    Kp1         Ki         Kd        SumMax       SumMin       MaxO      MinO */
    //  {  8.00000f , 0.6f , 2.00000f , 0.00000f , 0.00000f ,   050.0f ,   -050.0f   },
    //                 Kp2 5.00             0           Kd 40
    { 0.0120000f, 0.0f, 0.0000f,    0100.0f,   -0100.0f, 900.0f, -900.0f  },

    {  0.00000f, 0.0000f, 0.00000f, 0.00000f, 080.00000f,   0000.0f,   -000.0f   },
    {  00.0000f, 0.00000f, 0.00000f, 0.00000f, 000.00000f,   0000.0f,   -000.0f  },

};
float BalancePIDParam_angle_v[][7] =
{

    /*    Kp1       Kp2       kp3         Ki         Kd        SumMax       SumMin       MaxO      MinO */
    //  {  8.00000f , 0.6f , 2.00000f , 0.00000f , 0.00000f ,   050.0f ,   -050.0f   },
    //                 Kp2 5.00                         Kd 40
    { 0.01000f, 0.0f, 0.00000f,    0100.0f,   -0100.0f, 900.0f, -900.0f  },

    {  0.00000f, 0.0000f, 0.00000f, 0.00000f, 080.00000f,   0000.0f,   -000.0f   },
    {  00.0000f, 0.00000f, 0.00000f, 0.00000f, 000.00000f,   0000.0f,   -000.0f  },

};
int PID_Realize1(PID *sptr, float *PID, int NowData, int Point)
{
    //当前误差，定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
    int iError,	// 当前误差
        Realize;	// 最后得出的实际增量

    iError = Point - NowData;	// 计算当前误差
    //	iError = range_protect(iError, -25000, 25000);
    sptr->SumError += PID[KI] * iError;	// 误差积分
    if (sptr->SumError >= PID[KT])
    {
        sptr->SumError = PID[KT];
    }
    else if (sptr->SumError <= -PID[KT])
    {
        sptr->SumError = -PID[KT];
    }

    Realize = PID[KP] * iError
              + sptr->SumError
              + PID[KD] * (iError - sptr->LastError);
    sptr->PrevError = sptr->LastError;	// 更新前次误差
    sptr->LastError = iError;		  	// 更新上次误差
    sptr->LastData  = NowData;			// 更新上次数据

    return Realize;	// 返回实际值
}
int PlacePID_Control(PID *sprt, float *PID, int NowPiont, int SetPoint)
{
    //定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
    int iError,	//当前误差
        Actual;	//最后得出的实际输出值
    float Kp;		//动态P
    iError = SetPoint - NowPiont;	//计算当前误差
    sprt->SumError += iError * 0.01;
    if (sprt->SumError >= PID[KT])
    {
        sprt->SumError = PID[KT];
    }
    else if (sprt->SumError <= -PID[KT])
    {
        sprt->SumError = -PID[KT];
    }
    //        Kp = 1.0 * abs(iError) / PID[KP] + PID[KI];
    Kp = 1.0 * (iError * iError) / PID[KP] + PID[KI];	//P值与差值成二次函数关系，此处P和I不是PID参数，而是动态PID参数
    Actual = Kp * iError
             + PID[KD] * ((0.8 * iError + 0.2 * sprt->LastError) - sprt->LastError); //只用PD
    sprt->LastError = iError;		//更新上次误差
    //	Actual += sprt->SumError*0.1;
    Actual = range_protect(Actual, -180, 180);
    return Actual;
}

//初始化PID的参数
void PID_Init(void)
{
    //---------------模糊PID------------//
    FuzzyPID_Init();
}
int Position2_PID(int Encoder, int Target)

{

    static float Bias, Pwm, Integral_bias, Last_Bias;

    Bias = Encoder - Target; //计算偏差

    Integral_bias += Bias; //求出偏差的积分

    Pwm = 0.65 * Bias + 0.0 * Integral_bias + 0 * (Bias - Last_Bias); //位置式PID控制器

    Last_Bias = Bias; //保存上一次偏差

    return Pwm; //增量输出

}
int32 PID_Realize3(PID *sptr, float *PID, int32 NowData, int32 Point)
{
    //当前误差，定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
    int32 iError,   // 当前误差
          Realize;  // 最后得出的实际增量

    iError = Point - NowData;   // 计算当前误差
    iError = range_protect(iError, -90, 90);
    sptr->SumError += PID[KI] * iError; // 误差积分
    if (sptr->SumError >= PID[KT])
    {
        sptr->SumError = PID[KT];

    }
    else if (sptr->SumError <= -PID[KT])
    {
        sptr->SumError = -PID[KT];
    }

    Realize = PID[KP] * iError
              + sptr->SumError
              + PID[KD] * (iError - sptr->LastError);
    sptr->PrevError = sptr->LastError;  // 更新前次误差
    sptr->LastError = iError;           // 更新上次误差
    sptr->LastData  = NowData;          // 更新上次数据

    return Realize; // 返回实际值
}
int rubo_PID(PID *sptr, float *PID, int NowData, int Point)
{
    //当前误差，定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
    float iError,   Realize;    //  当前误差, 最后得出的实际增量
    iError = Point - NowData;   // 计算当前误差
    sptr->SumError += iError * PID[KI]; // 误差积分
    if (sptr->SumError >= PID[KT])  sptr->SumError = PID[KT];
    else if (sptr->SumError <= -PID[KT])    sptr->SumError = -PID[KT];
    Realize = PID[KP]  * iError +  sptr->SumError + PID[KD] * (iError - sptr->LastError);
    sptr->PrevError = sptr->LastError;  // 更新前次误差
    sptr->LastError = iError;           // 更新上次误差
    sptr->LastData  = NowData;          // 更新上次数据
    //if(sptr->PrevError*sptr->LastError<0) sptr->SumError =0;
    return (int)Realize;    // 返回实际值
}

//---------------直立车增量/位置PID-----------------//
int PID_Increase(PID *sptr, float *PID, int NowData, int Point)
{
    //当前误差，定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
    int iError, //当前误差
        Increase;   //最后得出的实际增量

    iError = Point - NowData;   // 计算当前误差

    Increase =  PID[KP] * (iError - sptr->LastError)
                + PID[KI] * iError
                + PID[KD] * (iError - 2 * sptr->LastError + sptr->PrevError);

    sptr->PrevError = sptr->LastError;  // 更新前次误差
    sptr->LastError = iError;           // 更新上次误差
    sptr->LastData  = NowData;          // 更新上次数据
    return Increase;    // 返回增量
}
int PID_Realize2(PID *sptr, float *PID, int NowData, int Point)
{
    //当前误差，定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
    int iError, // 当前误差
        Realize;    // 最后得出的实际增量

    iError = Point - NowData;   // 计算当前误差
    sptr->SumError += PID[KI] * iError; // 误差积分
    if (sptr->SumError >= PID[KT])
    {
        sptr->SumError = PID[KT];
    }
    else if (sptr->SumError <= -PID[KT])
    {
        sptr->SumError = -PID[KT];
    }

    Realize = PID[KP] * iError * iError / 100000 * (iError > 0 ? 1 : -1)
              + sptr->SumError
              + PID[KD] * (iError - sptr->LastError);
    sptr->PrevError = sptr->LastError;  // 更新前次误差
    sptr->LastError = iError;           // 更新上次误差
    sptr->LastData  = NowData;          // 更新上次数据
    //  if(sptr->PrevError*sptr->LastError<0) sptr->SumError =0;

    return Realize; // 返回实际值
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
float Float_SimpleIncremental_PID(float *pid_param, TurnLoopCamTypedef *pid, float _exval, float _nowval)
{
    //读取参数
    pid->kp1 = pid_param[0];

    pid->ki = pid_param[1];
    pid->kd = pid_param[2];
    pid->sumErrormax = (int16)pid_param[3];
    pid->sumErrormin = (int16)pid_param[4];
    pid->maxout = (int16)pid_param[5];
    pid->minout = (int16) pid_param[6];

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
    pid->kp = pid->kp1; //+ pid->kp2*(1 - (2 / ( exp(pid->kp3 * pid->nowError) +exp(- pid->kp3 * pid->nowError) ) ) );
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




//----------------------位置式pid控制 摄像头---------------------------//
//非线性


//------------------------------------ 模糊PID ---------------------------------------------//

/*论域*/
#define     NB      -6
#define     NM      -4
#define     NS      -2
#define     ZO      0
#define     PS      2
#define     PM      4
#define     PB      6


/*Kp规则表*/
int8 ruleKp[7][7] =
{

    /*  \ Ec */      /* NB 负大   NM 负中  NS 负小   ZO 零  PS 正小  PM 正中  PB 正大 */
    /* E \ */           /* -6        -4        -2      0        2        4         6*/
    /* NB 负大 -6 */      PB,    PB,  PM,  PB,   PM,   ZO,   ZO,
    /* NM 负中 -4 */      PB,    PB,  PM,  PM,   PS,   ZO,   NS,
    /* NS 负小 -2 */      PM,    PM,  PS,  PS,   ZO,   NS,   NS,
    /* ZO 零    0 */      PM,    PM,  PS,  ZO,   NS,   NM,   NM,
    /* PS 正小  2 */      PS,    PS,  ZO,  NS,   NS,   NM,   NM,
    /* PM 正中  4 */      PS,    ZO,  NS,  NM,   NM,   NB,   NB,
    /* PB 正大  6 */      ZO,    ZO,  NM,  NB,   NM,   NB,   NB,

};

/*Ki规则表*/
int8 ruleKi[7][7] =
{

    /*  \ Ec */      /* NB 负大   NM 负中  NS 负小   ZO 零  PS 正小  PM 正中  PB 正大 */
    /* E \ */           /* -6        -4        -2      0        2        4         6*/
    /* NB 负大 -6 */      PS,    NS,  NB,  NB,   NB,   NM,   PS,
    /* NM 负中 -4 */      PS,    NS,  NB,  NM,   NM,   NS,   PS,
    /* NS 负小 -2 */      ZO,    NS,  NM,  NM,   NS,   NS,   ZO,
    /* ZO 零    0 */      ZO,    NS,  NS,  NS,   NS,   NS,   ZO,
    /* PS 正小  2 */      ZO,    ZO,  ZO,  ZO,   ZO,   ZO,   ZO,
    /* PM 正中  4 */      PB,    NS,  PS,  PS,   PS,   PS,   PB,
    /* PB 正大  6 */      PB,    PM,  PM,  PM,   PS,   PS,   PB,

};


/*Kd规则表*/
int8 ruleKd[7][7] =
{

    /*  \ Ec */      /* NB 负大   NM 负中  NS 负小   ZO 零  PS 正小  PM 正中  PB 正大 */
    /* E \ */           /* -6        -4        -2      0        2        4         6*/
    /* NB 负大 -6 */      PB,    PB,  PM,  PB,   PM,   ZO,   ZO,
    /* NM 负中 -4 */      PB,    PB,  PM,  PM,   PS,   ZO,   NS,
    /* NS 负小 -2 */      PM,    PM,  PS,  PS,   ZO,   NS,   NS,
    /* ZO 零    0 */      PM,    PM,  PS,  ZO,   NS,   NM,   NM,
    /* PS 正小  2 */      PS,    PS,  ZO,  NS,   NS,   NM,   NM,
    /* PM 正中  4 */      PS,    ZO,  NS,  NM,   NM,   NB,   NB,
    /* PB 正大  6 */      ZO,    ZO,  NM,  NB,   NM,   NB,   NB,

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
    if ((qv >= NB) && (qv < NM))
    {
        index[0] = 0;
        index[1] = 1;
        ms[0] = -0.5 * qv - 2.0; //y=-0.5x-2.0
        ms[1] = 0.5 * qv + 3.0; //y=0.5x+3.0
    }
    else if ((qv >= NM) && (qv < NS))
    {
        index[0] = 1;
        index[1] = 2;
        ms[0] = -0.5 * qv - 1.0; //y=-0.5x-1.0
        ms[1] = 0.5 * qv + 2.0; //y=0.5x+2.0
    }
    else if ((qv >= NS) && (qv < ZO))
    {
        index[0] = 2;
        index[1] = 3;
        ms[0] = -0.5 * qv;  //y=-0.5x
        ms[1] = 0.5 * qv + 1.0; //y=0.5x+1.0
    }
    else if ((qv >= ZO) && (qv < PS))
    {
        index[0] = 3;
        index[1] = 4;
        ms[0] = -0.5 * qv + 1.0; //y=-0.5x+1.0
        ms[1] = 0.5 * qv;   //y=0.5x
    }
    else if ((qv >= PS) && (qv < PM))
    {
        index[0] = 4;
        index[1] = 5;
        ms[0] = -0.5 * qv + 2.0; //y=-0.5x+2.0
        ms[1] = 0.5 * qv - 1.0; //y=0.5x-1.0
    }
    else if ((qv >= PM) && (qv <= PB))
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
//  FuzzyPID(&AngleLoop_Fuzzy,Wheel.RealSpeedNow,280);
//}


