#ifndef ADC_H
#define ADC_H
#include "headfile.h"

#define FILTERNUM 10

//kalman�˲�
//1. �ṹ�����Ͷ���
typedef struct 
{
    float LastP;//�ϴι���Э���� ��ʼ��ֵΪ0.02
    float Now_P;//��ǰ����Э���� ��ʼ��ֵΪ0
    float out;//�������˲������ ��ʼ��ֵΪ0
    float Kg;//���������� ��ʼ��ֵΪ0
    float Q;//��������Э���� ��ʼ��ֵΪ0.001
    float R;//�۲�����Э���� ��ʼ��ֵΪ0.543
}KFP_Typedef;//Kalman Filter parameter

//statement
extern KFP_Typedef KFP_Speed;
extern KFP_Typedef KFP_ELC14;
extern KFP_Typedef KFP_ELC23;
//function

float KalmanFilter(KFP_Typedef *kfp,float input);

extern void ADC_init();
extern void AdcParamInit();
extern void ADC_calc();
extern void AdcControl();

extern void ELC_Flag_Judge(void);
extern uint16 ELC_circle_count;//��������
extern uint8  ELC_Circle_turn;//��������ת����0����δ��
extern uint8  ELC_Circle_flag;//�ݶ��õ���жϻ�����־��������ͷ�ж����һ���
extern uint8  circle_ready_flag;//����ǰ�Ƿ���Ԥ��־λ
extern uint8  ELC_Circle_at;//���ڵ�Ż��������еı�־λ

typedef struct
{
    uint8  raw[FILTERNUM];
    uint8  value;
    uint8  max;
    uint8  min;
    uint16 sum;
}SpecificAdc;

typedef struct
{
    SpecificAdc l2;
    SpecificAdc l1;
    SpecificAdc r1;
    SpecificAdc r2;
    uint16 sum;
}Adc;
extern Adc adc;
#endif
