#include "Pid.h"

float Angle[4]   = {0.22, 0, 0.03, 500};		// �ǶȻ�PID{0.2, 0, 0.03, 1000};
float Ang_Vel[4] = {0.2, 0.005, 0, 1000};	// ���ٶȻ�PID {0.2, 0.007, 0, 500};
float Direct[4]  = {0.025, 0.001, 0.02, 70};	// ת��PID λ��{0.018, 0.00215, 0.023, 55};
float MOTOR[4]   = {50, 0, 5, 1000};		// �ٶȻ�PID	���һ��Ϊ�����޷�{50, 0, 5, 1000};
float Turn[5][4] = {{100, 2, 1, 100},   //��ת�����PID
    {100, 3, 2, 100},   //����ת�����PID
    {100, 4, 3, 100},   //����ת�����PID
    {100, 5, 4, 100},   //����ת�����PID
    {200, 10, 8, 100}
};  //����ת�����PID
//float Angle[4]   = {0.3, 0, 0.04, 1000};		// �ǶȻ�PID{0.25, 0, 0.06, 1000}{0.21, 0, 0.042, 1000};
//float Ang_Vel[4] = {0.23, 0.015, 0, 800};	// ���ٶȻ�PID {0.4, 0.03, 0, 1000}   {0.6, 0.023, 0, 999} {0.175, 0.0028, 0, 1000};
//float Direct[4]  = {0.018, 0.00215, 0.023, 55};	// ת��PID λ��{0.019, 0.002, 0.023, 60}; {0.085, 0.00154, 0, 70}
//float MOTOR[4]   = {50, 0, 5, 1000};		// �ٶȻ�PID	���һ��Ϊ�����޷�{50, 0, 5, 1000};
//float Turn[5][4] = {{100, 2, 1, 100},   //��ת�����PID
//		    {100, 3, 2, 100},   //����ת�����PID
//		    {100, 4, 3, 100},   //����ת�����PID
//		    {100, 5, 4, 100},   //����ת�����PID
//		    {200, 10, 8, 100}};  //����ת�����PID

float temp = 0;

PID Angle_PID, Ang_Vel_PID, Direct_PID, MOTOR_PID, Turn_PID;

// PID������ʼ��
void PID_Parameter_Init(PID *sptr)
{
    sptr->SumError  = 0;
    sptr->LastError = 0;	//Error[-1]
    sptr->PrevError = 0;	//Error[-2]
    sptr->LastData  = 0;
}

// λ��ʽPID����
int32 PID_Realize1(PID *sptr, float *PID, int32 NowData, int32 Point)
{
    //��ǰ������Ϊ�Ĵ���������ֻ���������ͺ��ַ��ͱ�������������ٶ�
    int32 iError,	// ��ǰ���
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

int32 PID_Realize2(PID *sptr, float *PID, int32 NowData, int32 Point)
{
    //��ǰ������Ϊ�Ĵ���������ֻ���������ͺ��ַ��ͱ�������������ٶ�
    int32 iError,	// ��ǰ���
          Realize;	// ���ó���ʵ������

    iError = Point - NowData;	// ���㵱ǰ���
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

int32 PID_Realize3(PID *sptr, float *PID, int32 NowData, int32 Point)
{
    //��ǰ������Ϊ�Ĵ���������ֻ���������ͺ��ַ��ͱ�������������ٶ�
    int32 iError,	// ��ǰ���
          Realize;	// ���ó���ʵ������

    iError = Point - NowData;	// ���㵱ǰ���
    iError = range_protect(iError, -90, 90);
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

// ����ʽPID�������
int32 PID_Increase(PID *sptr, float *PID, int32 NowData, int32 Point)
{
    //��ǰ������Ϊ�Ĵ���������ֻ���������ͺ��ַ��ͱ�������������ٶ�
    int32 iError,	//��ǰ���
          Increase;	//���ó���ʵ������

    iError = Point - NowData;	// ���㵱ǰ���

    Increase =  PID[KP] * (iError - sptr->LastError)
                + PID[KI] * iError
                + PID[KD] * (iError - 2 * sptr->LastError + sptr->PrevError);

    sptr->PrevError = sptr->LastError;	// ����ǰ�����
    sptr->LastError = iError;		  	// �����ϴ����
    sptr->LastData  = NowData;			// �����ϴ�����

    return Increase;	// ��������
}


// λ��ʽ��̬PID����
int32 PlacePID_Control(PID *sprt, float *PID, int NowPiont, int SetPoint)
{
    //����Ϊ�Ĵ���������ֻ���������ͺ��ַ��ͱ�������������ٶ�
    int32 iError,	//��ǰ���
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