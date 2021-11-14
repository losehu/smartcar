#include "Pid.h"

float Angle[4]   = {0.22, 0, 0.03, 500};		// 角度环PID{0.2, 0, 0.03, 1000};
float Ang_Vel[4] = {0.2, 0.005, 0, 1000};	// 角速度环PID {0.2, 0.007, 0, 500};
float Direct[4]  = {0.025, 0.001, 0.02, 70};	// 转向环PID 位置{0.018, 0.00215, 0.023, 55};
float MOTOR[4]   = {50, 0, 5, 1000};		// 速度环PID	最后一项为积分限幅{50, 0, 5, 1000};
float Turn[5][4] = {{100, 2, 1, 100},   //起步转向参数PID
    {100, 3, 2, 100},   //低速转向参数PID
    {100, 4, 3, 100},   //中速转向参数PID
    {100, 5, 4, 100},   //高速转向参数PID
    {200, 10, 8, 100}
};  //环岛转向参数PID
//float Angle[4]   = {0.3, 0, 0.04, 1000};		// 角度环PID{0.25, 0, 0.06, 1000}{0.21, 0, 0.042, 1000};
//float Ang_Vel[4] = {0.23, 0.015, 0, 800};	// 角速度环PID {0.4, 0.03, 0, 1000}   {0.6, 0.023, 0, 999} {0.175, 0.0028, 0, 1000};
//float Direct[4]  = {0.018, 0.00215, 0.023, 55};	// 转向环PID 位置{0.019, 0.002, 0.023, 60}; {0.085, 0.00154, 0, 70}
//float MOTOR[4]   = {50, 0, 5, 1000};		// 速度环PID	最后一项为积分限幅{50, 0, 5, 1000};
//float Turn[5][4] = {{100, 2, 1, 100},   //起步转向参数PID
//		    {100, 3, 2, 100},   //低速转向参数PID
//		    {100, 4, 3, 100},   //中速转向参数PID
//		    {100, 5, 4, 100},   //高速转向参数PID
//		    {200, 10, 8, 100}};  //环岛转向参数PID

float temp = 0;

PID Angle_PID, Ang_Vel_PID, Direct_PID, MOTOR_PID, Turn_PID;

// PID参数初始化
void PID_Parameter_Init(PID *sptr)
{
    sptr->SumError  = 0;
    sptr->LastError = 0;	//Error[-1]
    sptr->PrevError = 0;	//Error[-2]
    sptr->LastData  = 0;
}

// 位置式PID控制
int32 PID_Realize1(PID *sptr, float *PID, int32 NowData, int32 Point)
{
    //当前误差，定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
    int32 iError,	// 当前误差
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

int32 PID_Realize2(PID *sptr, float *PID, int32 NowData, int32 Point)
{
    //当前误差，定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
    int32 iError,	// 当前误差
          Realize;	// 最后得出的实际增量

    iError = Point - NowData;	// 计算当前误差
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

int32 PID_Realize3(PID *sptr, float *PID, int32 NowData, int32 Point)
{
    //当前误差，定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
    int32 iError,	// 当前误差
          Realize;	// 最后得出的实际增量

    iError = Point - NowData;	// 计算当前误差
    iError = range_protect(iError, -90, 90);
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

// 增量式PID电机控制
int32 PID_Increase(PID *sptr, float *PID, int32 NowData, int32 Point)
{
    //当前误差，定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
    int32 iError,	//当前误差
          Increase;	//最后得出的实际增量

    iError = Point - NowData;	// 计算当前误差

    Increase =  PID[KP] * (iError - sptr->LastError)
                + PID[KI] * iError
                + PID[KD] * (iError - 2 * sptr->LastError + sptr->PrevError);

    sptr->PrevError = sptr->LastError;	// 更新前次误差
    sptr->LastError = iError;		  	// 更新上次误差
    sptr->LastData  = NowData;			// 更新上次数据

    return Increase;	// 返回增量
}


// 位置式动态PID控制
int32 PlacePID_Control(PID *sprt, float *PID, int NowPiont, int SetPoint)
{
    //定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
    int32 iError,	//当前误差
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