#include "elc.h"


//statement
#define PROTECR_INDUCTOR_VAL  50

//ԭʼֵ
int32	LNow[ELC_NUM] = {0, 0, 0, 0},
                        LMax[ELC_NUM] = {0, 0, 0, 0},
                                        LMin[ELC_NUM] = {0, 0, 0, 0};
uint16  ELC[ELC_GROUP][ELC_TIME][ELC_NUM];
float	LNor[ELC_NUM];
uint16  LNORMAX[ELC_NUM] = {4095, 4095, 4095, 4095};
uint8	MaxNorIndex = 0;
uint8	MinNorIndex = 0;
uint16  ELC_Sum_14 = 0;
uint16  ELC_Sum_23 = 0;
uint16  ELC_Sum_1234 = 0;
int16   ELC_Center_14 = 0;
int16   ELC_Center_23 = 0;
uint8   ELC_PROTECT_FLAG;
uint16  KFP_ELC_Sum_14 = 0;
uint16  KFP_ELC_Sum_23 = 0;
float  ELC_Center_14_100;

//��ȡ���ֵ
uint8 Inductor_Read(void)
{
    uint16 L_sum[ELC_NUM] = {0, 0, 0, 0};
    int8 i = 0, j = 0, k = 0;
    uint16  L_max = 0, L_max_index = 0, len = 0;
    //��·ADC
    for(i = 0; i < ELC_GROUP; i ++)
    {
        for(j = 0; j < ELC_TIME; j ++)
        {
            ELC[i][j][3] = adc_convert(ADC_1, ADC1_CH04_A04);
            ELC[i][j][2] = adc_convert(ADC_1, ADC1_CH05_A05);
            ELC[i][j][1] = adc_convert(ADC_1, ADC1_CH06_A06);
            ELC[i][j][0] = adc_convert(ADC_1, ADC1_CH07_A07);
        }
    }
    //ѡ������
    //��·ADC
    for(i = 0; i < ELC_GROUP; i ++) //���е��鶼��Ҫ����
    {
        for(k = 0; k < ELC_NUM; k ++) //ÿ����к���������
        {
            len = ELC_TIME;
            while(len > 1)//����ֻʣ��һ��Ԫ�غ��ٽ�������
            {
                L_max = 0;
                L_max_index = 0;
                for(j = 0; j < len; j ++)
                {
                    if(ELC[i][j][k] > L_max)
                    {
                        L_max = ELC[i][j][k];//��¼���ֵ
                        L_max_index = j;//��¼���ֵ�±�
                    }
                }
                ELC[i][L_max_index][k] = ELC[i][len - 1][k];//��������
                ELC[i][len - 1][k] = L_max;//���ֵ����
                len --;
            }
            L_sum[k] += ELC[i][ELC_NEED][k];
            if(LMax[k] < ELC[i][ELC_TIME - 1][k])
                LMax[k] = ELC[i][ELC_TIME - 1][k];
            if(LMin[k] > ELC[i][0][k])
                LMin[k] = ELC[i][0][k];
        }
    }
    //��ֵ
    for(i = 0; i < ELC_NUM; i ++)
    {
        LNow[i] = L_sum[i] / ELC_GROUP;
    }
    return 0;
}


//��һ��
void ELC_Normalized(void)
{
    uint8 i = 0;
    uint8 max_nor = 0;
    uint8 min_nor = 100;
    //��һ�ι�һ��
    for(i = 0; i < ELC_NUM; i ++)
    {
        LNor[i] = LNow[i];
        LNor[i] = LNor[i] * 100 / LNORMAX[i];
    }
    //��ȡ����һ������±� ����ͣ������
    for(i = 0; i < ELC_NUM; i ++)
    {
        if(max_nor < LNor[i])
        {
            max_nor = LNor[i];
            MaxNorIndex = i;
        }
    }
    //��ȡ��С��һ������±� ����Ԫ�ر���
    for(i = 0; i < ELC_NUM; i ++)
    {
        if(min_nor > LNor[i])
        {
            min_nor = LNor[i];
            MinNorIndex = i;
        }
    }
    if(MAX(LNow[L1], LNow[L4]) <= PROTECR_INDUCTOR_VAL)
        ELC_PROTECT_FLAG = 'T';
    else
        ELC_PROTECT_FLAG = 'F';

    //��ȡ135���֮�� ������һ������
    ELC_Sum_14 = LNor[L1] + LNor[L4];
    ELC_Sum_23 = LNor[L2] + LNor[L3];
    ELC_Sum_1234 =  LNor[L1] + LNor[L2] + LNor[L3] + LNor[L4];
}


//���������� ʹ�õ�һ�ι�һ������ֵ
//ʹ�ò�Ⱥ��㷨
#define ELC_MAX_CENTER_ERROR 100
void ELC_Center_Calculate(void)
{
    uint8 LNorm[4] = {0, 0, 0};
    int32 NearLError[2] = {0, 0};
    int32 NearLSum[2] = {0, 0};
    //����ԭ��һ��ֵ
    LNorm[0] = LNor[L1] + 1;
    LNorm[1] = LNor[L2] + 1;
    LNorm[2] = LNor[L3] + 1;
    LNorm[3] = LNor[L4] + 1;
    //14 23�������
    NearLError[0] = LNorm[3] - LNorm[0];//����ֵ��
    NearLError[1] = LNorm[2] - LNorm[1];//б���ֵ��
    //14 23�������
    NearLSum[0] = LNorm[0] + LNorm[3];//����ֵ��
    NearLSum[1] = LNorm[1] + LNorm[2];//б���ֵ��
    //��Ⱥͼ�����������
    ELC_Center_14_100 = (LNorm[3] - LNorm[0]) * 10000 / NearLSum[0];
    ELC_Center_14 = NearLError[0] * 100 / NearLSum[0];//����
    ELC_Center_23 = NearLError[1] * 100 / NearLSum[1];//б���
    //�����޷� 200
    ELC_Center_14 = range_protect(ELC_Center_14, -ELC_MAX_CENTER_ERROR, ELC_MAX_CENTER_ERROR);
    ELC_Center_23 = range_protect(ELC_Center_23, -ELC_MAX_CENTER_ERROR, ELC_MAX_CENTER_ERROR);
}


//��ű�־����
void ELC_Status_Init(void)
{
    //Circle
    ELC_Circle.FindFlag = 'F';
    ELC_Circle.NowValLessThanMaxCnt = 0;
    ELC_Circle.SUM_MAX = 0;
    ELC_Circle.CircleInTurnFlag = 'F';
}


//��м��Ԫ�� ���� ʮ��
uint8 ELC_Judge(void)
{
    //�Ե�Ž����˲�
    KFP_ELC_Sum_14 = KalmanFilter(&KFP_ELC14, ELC_Sum_14);
    KFP_ELC_Sum_23 = KalmanFilter(&KFP_ELC23, ELC_Sum_23);
    //	if(ELC_Sum_14 >= 80 && ELC_Sum_23 >= 70)
    //		Beep_DiDi(1,500);
    //	ELC_Circle_Judge();
    return 1;
}


//---------------------------------------------CIRCLE-----------------------------------------//
ELC_CircleTypedef ELC_Circle;


//�����������ͷ�����жϻ����յ�
uint8 ELC_Circle_Judge(void)
{
    //	if(ABS(ELC_Center_23) < 20 && ELC_Sum_14 >= 125)
    //	{
    //		Beep_DiDi(1,100);
    //	}
    //	//���ֱ�־
    //	if(ELC_Sum_14 > 50 && ELC_Circle.FindFlag == 'F' && Circle.FindFlag == 'T')
    //	{
    //		ELC_Circle.FindFlag = 'T';
    //	}
    //	//�������ֵ
    //	if(ELC_Sum_14 > ELC_Circle.SUM_MAX && ELC_Circle.FindFlag == 'T')
    //	{
    //		ELC_Circle.SUM_MAX = ELC_Sum_14;
    //		ELC_Circle.NowValLessThanMaxCnt = 0;
    //	}
    //	if(ELC_Sum_14 < (ELC_Circle.SUM_MAX - 3) && ELC_Circle.FindFlag == 'T')
    //	{
    //		ELC_Circle.NowValLessThanMaxCnt ++;
    //	}
    //	if(ELC_Circle.NowValLessThanMaxCnt >= 10 && ELC_Circle.CircleInTurnFlag == 'F')
    //	{
    //		ELC_Circle.CircleInTurnFlag = 'T';
    //		Beep_DiDi(1,100);
    //	}
    //	if(ELC_Circle.CircleInTurnFlag == 'T' && Circle.FindFlag == 'F')
    //	{
    //		ELC_Circle.FindFlag = 'F';
    //		ELC_Circle.CircleInTurnFlag = 'F';
    //		ELC_Circle.NowValLessThanMaxCnt = 0;
    //		ELC_Circle.SUM_MAX = 0;
    //	}
    return 1;
}



//LCD��ʾ ��5ҳ
uint8 ELC_Page(void)
{
    while(KeyPageUp != onepress)
    {
        Inductor_Read();
        ELC_Normalized();
        ELC_Center_Calculate();
        ELC_Show();

    }
    KeyPageUp = nopress;
    return 0;
}


//��е���ʾ����
uint8 ELC_Show(void)
{
    while(KeyPageUp != onepress)
    {
        Inductor_Read();
        ELC_Normalized();
        ELC_Center_Calculate();
        IPS114_PENCOLOR = RED;
        ips114_showstr(0, 0, "    L1    L2    L3   L4 ");
        IPS114_PENCOLOR = BLUE;
        //ԭʼֵ
        ips114_showuint16(6  , 1, LNow[L1]);
        ips114_showuint16(52 , 1, LNow[L2]);
        ips114_showuint16(98 , 1, LNow[L3]);
        ips114_showuint16(144, 1, LNow[L4]);
        //��һ��ֵ
        IPS114_PENCOLOR = RED;
        ips114_showstr(0, 2, "    L1    L2   L3    L4  ");
        IPS114_PENCOLOR = BLUE;
        ips114_showuint8(6  , 3, LNor[L1]);
        ips114_showuint8(52 , 3, LNor[L2]);
        ips114_showuint8(98 , 3, LNor[L3]);
        ips114_showuint8(144, 3, LNor[L4]);
        //14�� 23��
        IPS114_PENCOLOR = RED;
        ips114_showstr(0, 4, "  LSUM_14  LSUM_23 LSUM_1234");
        IPS114_PENCOLOR = BLUE;
        ips114_showint16(0 , 5, ELC_Sum_14);
        ips114_showint16(80 , 5, ELC_Sum_23);
        ips114_showint16(160 , 5, ELC_Sum_1234);
        //�������ߵ�ֵ
        IPS114_PENCOLOR = RED;
        ips114_showstr(0, 6, "CENTER_14   CENTER_23");
        IPS114_PENCOLOR = BLUE;
        ips114_showint16(16 , 7, ELC_Center_14);
        ips114_showint16(88 , 7, ELC_Center_23);
        //return
        IPS114_PENCOLOR = RED;
    }
    KeyPageUp = nopress;
    return 1;
}
