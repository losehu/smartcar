#include "pid.h"


//PID�����仯

#define     SPEEDLOOP_Kp        0
#define     SPEEDLOOP_Ki        1
#define     SPEEDLOOP_Kd        2
#define     SPEEDLOOP_MaxO      3
#define     SPEEDLOOP_MinO      4
//P  I  D    MAX_SUM_ERROR
float Angle[4]   = {1.3, 0.01, 0.0010, 1};      // �ǶȻ�PID{0.2, 0, 0.03, 1000};
float Ang_Vel[4] = {0.06, 0.003, 0.00000, 500};   // ���ٶȻ�PID {0.2, 0.007, 0, 500};
float Vel[4] = {6, 0.017, 0.001, 1000};   // �ٶȻ�PID {0.2, 0.007, 0, 500};
float rubo_Vel[4]= {0.52, 0.00150, 0.01, 100}; //ת��ǶȻ�
//float Direct[4]  = {5, 0.01, 0.000, 70};	// ת����ٶȻ�PID λ��{0.018, 0.00215, 0.023, 55};
float Direct[4]  = {4, 0.028, 0.000, 200};	// ת����ٶȻ�PID λ��{0.018, 0.00215, 0.023, 55};

//����ת�����PID
/*******************�ٶȻ�********************/
float SpeedPIDParam[][5] =
{
    /*   Kp       Ki       Kd     MaxO    MinO */
    {  3.000f, 0.300f, 0.100f, 0400.0f, -0400.0f  },    //0.120f , 0.150f , 1.600f , 0400.0f , -0400.0f
    {  2.000f, 0.600f, 0.100f, 0400.0f, -0400.0f  },
};

/**************�ٶȻ�(���������)*************/
//����
float LeftMotorSpeedPIDParam[][7] =
{
    /*   Kp       Ki       Kd     MaxO    MinO */
    {  3.000f, 6.00f, 0.000f, 300, -300, 1000.0f, -1000.0f }
};


//�ҵ��
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
    //��ǰ������Ϊ�Ĵ���������ֻ���������ͺ��ַ��ͱ�������������ٶ�
    int iError,	// ��ǰ���
        Realize;	// ���ó���ʵ������

    iError = Point - NowData;	// ���㵱ǰ���
    //	iError = range_protect(iError, -25000, 25000);
    sptr->SumError += PID[KI] * iError;	// ������
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
    sptr->PrevError = sptr->LastError;	// ����ǰ�����
    sptr->LastError = iError;		  	// �����ϴ����
    sptr->LastData  = NowData;			// �����ϴ�����

    return Realize;	// ����ʵ��ֵ
}
int PlacePID_Control(PID *sprt, float *PID, int NowPiont, int SetPoint)
{
    //����Ϊ�Ĵ���������ֻ���������ͺ��ַ��ͱ�������������ٶ�
    int iError,	//��ǰ���
        Actual;	//���ó���ʵ�����ֵ
    float Kp;		//��̬P
    iError = SetPoint - NowPiont;	//���㵱ǰ���
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
    Kp = 1.0 * (iError * iError) / PID[KP] + PID[KI];	//Pֵ���ֵ�ɶ��κ�����ϵ���˴�P��I����PID���������Ƕ�̬PID����
    Actual = Kp * iError
             + PID[KD] * ((0.8 * iError + 0.2 * sprt->LastError) - sprt->LastError); //ֻ��PD
    sprt->LastError = iError;		//�����ϴ����
    //	Actual += sprt->SumError*0.1;
    Actual = range_protect(Actual, -180, 180);
    return Actual;
}

//��ʼ��PID�Ĳ���
void PID_Init(void)
{
    //---------------ģ��PID------------//
    FuzzyPID_Init();
}
int Position2_PID(int Encoder, int Target)

{

    static float Bias, Pwm, Integral_bias, Last_Bias;

    Bias = Encoder - Target; //����ƫ��

    Integral_bias += Bias; //���ƫ��Ļ���

    Pwm = 0.65 * Bias + 0.0 * Integral_bias + 0 * (Bias - Last_Bias); //λ��ʽPID������

    Last_Bias = Bias; //������һ��ƫ��

    return Pwm; //�������

}
int32 PID_Realize3(PID *sptr, float *PID, int32 NowData, int32 Point)
{
    //��ǰ������Ϊ�Ĵ���������ֻ���������ͺ��ַ��ͱ�������������ٶ�
    int32 iError,   // ��ǰ���
          Realize;  // ���ó���ʵ������

    iError = Point - NowData;   // ���㵱ǰ���
    iError = range_protect(iError, -90, 90);
    sptr->SumError += PID[KI] * iError; // ������
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
    sptr->PrevError = sptr->LastError;  // ����ǰ�����
    sptr->LastError = iError;           // �����ϴ����
    sptr->LastData  = NowData;          // �����ϴ�����

    return Realize; // ����ʵ��ֵ
}
int rubo_PID(PID *sptr, float *PID, int NowData, int Point)
{
    //��ǰ������Ϊ�Ĵ���������ֻ���������ͺ��ַ��ͱ�������������ٶ�
    float iError,   Realize;    //  ��ǰ���, ���ó���ʵ������
    iError = Point - NowData;   // ���㵱ǰ���
    sptr->SumError += iError * PID[KI]; // ������
    if (sptr->SumError >= PID[KT])  sptr->SumError = PID[KT];
    else if (sptr->SumError <= -PID[KT])    sptr->SumError = -PID[KT];
    Realize = PID[KP]  * iError +  sptr->SumError + PID[KD] * (iError - sptr->LastError);
    sptr->PrevError = sptr->LastError;  // ����ǰ�����
    sptr->LastError = iError;           // �����ϴ����
    sptr->LastData  = NowData;          // �����ϴ�����
    //if(sptr->PrevError*sptr->LastError<0) sptr->SumError =0;
    return (int)Realize;    // ����ʵ��ֵ
}

//---------------ֱ��������/λ��PID-----------------//
int PID_Increase(PID *sptr, float *PID, int NowData, int Point)
{
    //��ǰ������Ϊ�Ĵ���������ֻ���������ͺ��ַ��ͱ�������������ٶ�
    int iError, //��ǰ���
        Increase;   //���ó���ʵ������

    iError = Point - NowData;   // ���㵱ǰ���

    Increase =  PID[KP] * (iError - sptr->LastError)
                + PID[KI] * iError
                + PID[KD] * (iError - 2 * sptr->LastError + sptr->PrevError);

    sptr->PrevError = sptr->LastError;  // ����ǰ�����
    sptr->LastError = iError;           // �����ϴ����
    sptr->LastData  = NowData;          // �����ϴ�����
    return Increase;    // ��������
}
int PID_Realize2(PID *sptr, float *PID, int NowData, int Point)
{
    //��ǰ������Ϊ�Ĵ���������ֻ���������ͺ��ַ��ͱ�������������ٶ�
    int iError, // ��ǰ���
        Realize;    // ���ó���ʵ������

    iError = Point - NowData;   // ���㵱ǰ���
    sptr->SumError += PID[KI] * iError; // ������
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
    sptr->PrevError = sptr->LastError;  // ����ǰ�����
    sptr->LastError = iError;           // �����ϴ����
    sptr->LastData  = NowData;          // �����ϴ�����
    //  if(sptr->PrevError*sptr->LastError<0) sptr->SumError =0;

    return Realize; // ����ʵ��ֵ
}
//------------------------��ͨ����ʽpid-------------------------//
SpeedPIDTypedef SpeedPID, SpeedPIDLeft, SpeedPIDRight;
int16 SimpleIncremental_PID(float *pid_param, SpeedPIDTypedef *pid, int16 _exval, int16 _nowval)
{
    //��ȡ����
    pid->kp = pid_param[SPEEDLOOP_Kp];
    pid->ki = pid_param[SPEEDLOOP_Ki];
    pid->kd = pid_param[SPEEDLOOP_Kd];
    pid->maxout = (int16)pid_param[SPEEDLOOP_MaxO];
    pid->minout = (int16)pid_param[SPEEDLOOP_MinO];

    //����
    pid->exval = _exval;
    pid->nowval = _nowval;
    //��ǰ���
    pid->nowError =  pid->exval - pid->nowval;
    //����
    pid->increase =
        pid->kp * (float)(pid->nowError - pid->lastError) +
        pid->ki * (float)pid->nowError +
        pid->kd * (float)(pid->nowError - 2 * pid->lastError + pid->beforeError);
    //�������
    pid->lastError = pid->nowError;
    pid->beforeError = pid->lastError;
    pid->out += (int16)pid->increase;
    //���Ʒ���
    pid->out = range_protect(pid->out, (int32)pid->minout, (int32)pid->maxout);
    //���
    return pid->out;
}
float Float_SimpleIncremental_PID(float *pid_param, TurnLoopCamTypedef *pid, float _exval, float _nowval)
{
    //��ȡ����
    pid->kp1 = pid_param[0];

    pid->ki = pid_param[1];
    pid->kd = pid_param[2];
    pid->sumErrormax = (int16)pid_param[3];
    pid->sumErrormin = (int16)pid_param[4];
    pid->maxout = (int16)pid_param[5];
    pid->minout = (int16) pid_param[6];

    //����
    pid->exval = _exval;
    pid->nowval = _nowval;
    //��ǰ���
    pid->nowError =  pid->exval - pid->nowval;
    //������
    pid->sumError += pid->nowError;
    //�������޷�
    pid->sumError = range_protect(pid->sumError, (int32)pid->sumErrormin, (int32)pid->sumErrormax);
    //����kp2
    pid->kp2Real = pid->kp2_K * pid->kp2;
    //����kp_sum
    pid->kp = pid->kp1; //+ pid->kp2*(1 - (2 / ( exp(pid->kp3 * pid->nowError) +exp(- pid->kp3 * pid->nowError) ) ) );
    //����Ud
    pid->Ud = pid->kd * (1.000f - pid->Ud_a) * (float)(pid->nowError - pid->lastError) +
              pid->Ud_a * pid->Ud_last;
    //�������
    pid->out = (int16)(
                   pid->kp * (float)pid->nowError + \
                   pid->ki * (float)pid->sumError + \
                   pid->Ud);
    //�������
    pid->lastError = pid->nowError;
    pid->beforeError = pid->lastError;
    //���Ʒ���
    pid->out = range_protect(pid->out, (int32)pid->minout, (int32)pid->maxout);
    //��¼Ud
    pid->Ud_last = pid->out;
    //���
    return pid->out;
}




//----------------------λ��ʽpid���� ����ͷ---------------------------//
//������


//------------------------------------ ģ��PID ---------------------------------------------//

/*����*/
#define     NB      -6
#define     NM      -4
#define     NS      -2
#define     ZO      0
#define     PS      2
#define     PM      4
#define     PB      6


/*Kp�����*/
int8 ruleKp[7][7] =
{

    /*  \ Ec */      /* NB ����   NM ����  NS ��С   ZO ��  PS ��С  PM ����  PB ���� */
    /* E \ */           /* -6        -4        -2      0        2        4         6*/
    /* NB ���� -6 */      PB,    PB,  PM,  PB,   PM,   ZO,   ZO,
    /* NM ���� -4 */      PB,    PB,  PM,  PM,   PS,   ZO,   NS,
    /* NS ��С -2 */      PM,    PM,  PS,  PS,   ZO,   NS,   NS,
    /* ZO ��    0 */      PM,    PM,  PS,  ZO,   NS,   NM,   NM,
    /* PS ��С  2 */      PS,    PS,  ZO,  NS,   NS,   NM,   NM,
    /* PM ����  4 */      PS,    ZO,  NS,  NM,   NM,   NB,   NB,
    /* PB ����  6 */      ZO,    ZO,  NM,  NB,   NM,   NB,   NB,

};

/*Ki�����*/
int8 ruleKi[7][7] =
{

    /*  \ Ec */      /* NB ����   NM ����  NS ��С   ZO ��  PS ��С  PM ����  PB ���� */
    /* E \ */           /* -6        -4        -2      0        2        4         6*/
    /* NB ���� -6 */      PS,    NS,  NB,  NB,   NB,   NM,   PS,
    /* NM ���� -4 */      PS,    NS,  NB,  NM,   NM,   NS,   PS,
    /* NS ��С -2 */      ZO,    NS,  NM,  NM,   NS,   NS,   ZO,
    /* ZO ��    0 */      ZO,    NS,  NS,  NS,   NS,   NS,   ZO,
    /* PS ��С  2 */      ZO,    ZO,  ZO,  ZO,   ZO,   ZO,   ZO,
    /* PM ����  4 */      PB,    NS,  PS,  PS,   PS,   PS,   PB,
    /* PB ����  6 */      PB,    PM,  PM,  PM,   PS,   PS,   PB,

};


/*Kd�����*/
int8 ruleKd[7][7] =
{

    /*  \ Ec */      /* NB ����   NM ����  NS ��С   ZO ��  PS ��С  PM ����  PB ���� */
    /* E \ */           /* -6        -4        -2      0        2        4         6*/
    /* NB ���� -6 */      PB,    PB,  PM,  PB,   PM,   ZO,   ZO,
    /* NM ���� -4 */      PB,    PB,  PM,  PM,   PS,   ZO,   NS,
    /* NS ��С -2 */      PM,    PM,  PS,  PS,   ZO,   NS,   NS,
    /* ZO ��    0 */      PM,    PM,  PS,  ZO,   NS,   NM,   NM,
    /* PS ��С  2 */      PS,    PS,  ZO,  NS,   NS,   NM,   NM,
    /* PM ����  4 */      PS,    ZO,  NS,  NM,   NM,   NB,   NB,
    /* PB ����  6 */      ZO,    ZO,  NM,  NB,   NM,   NB,   NB,

};


//statement
FuzzyPID_Typedef  AngleLoop_Fuzzy;


/*��ʼ��fuzzy pid*/
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


/*�����ȼ��㺯��*/
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
//ģ��PIDʵ��
float FuzzyPID(FuzzyPID_Typedef *pid, float _nowval, float _exval)
{
    //��ȡ��ֵ
    pid->exval = _exval;
    pid->nowval = _nowval;
    //����Լ���������޷�
    pid->nowError = range_protect(pid->nowError, pid->nowErrmin, pid->nowErrmax);
    pid->ErrorInc = range_protect(pid->ErrorInc, pid->ECmin, pid->ECmax);
    //���µ�ǰ���
    pid->nowError =  pid->exval - pid->nowval;
    pid->ErrorInc = pid->nowError - pid->lastError;
    /********  ģ����  *******/
    //���� ����{-6��-5��-4��-3��-2��-1��0��1��2��3��4��5��6}
    pid->E_Quantized  = 6.0 * pid->nowError / pid->nowErrmax;
    pid->EC_Quantized = 6.0 * pid->ErrorInc / pid->ECmax;
    //����������
    Calculate_Membership(pid->E_Quantized, pid->E_Index, pid->E_Membership);
    Calculate_Membership(pid->EC_Quantized, pid->EC_Index, pid->EC_Membership);
    /********  ��ģ��  *******/
    //����kx2��ϵ��
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
    //�����ܵ�kx
    pid->kp = pid->kp1 + pid->kp2 * (ABS_FLOAT(pid->kp2k) / 6.0);//��������ֵ �������kp�����ٷֱ�
    pid->kd = pid->kd1 + pid->kd2 * (ABS_FLOAT(pid->kd2k) / 6.0);//��������ֵ �������kp�����ٷֱ�
    //�������
    pid->out =
        pid->kp * (float)pid->nowError + \
        pid->kd * (float)(pid->nowError - pid->lastError);
    //�������
    pid->lastError = pid->nowError;
    pid->preError = pid->lastError;
    //����޷�
    pid->out = range_protect(pid->out, (int32)pid->outmin, (int32)pid->outmax);
    //���
    return pid->out;
}

//void Fuzzy_PID_Test(void)
//{
//  FuzzyPID(&AngleLoop_Fuzzy,Wheel.RealSpeedNow,280);
//}


