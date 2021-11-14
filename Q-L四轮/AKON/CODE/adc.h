#ifndef ADC_H
#define ADC_H
#include "headfile.h"

#define FILTERNUM 10

//kalman滤波
//1. 结构体类型定义
typedef struct 
{
    float LastP;//上次估算协方差 初始化值为0.02
    float Now_P;//当前估算协方差 初始化值为0
    float out;//卡尔曼滤波器输出 初始化值为0
    float Kg;//卡尔曼增益 初始化值为0
    float Q;//过程噪声协方差 初始化值为0.001
    float R;//观测噪声协方差 初始化值为0.543
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
extern uint16 ELC_circle_count;//环岛计数
extern uint8  ELC_Circle_turn;//进环出环转换，0代表未进
extern uint8  ELC_Circle_flag;//暂定用电磁判断环岛标志，用摄像头判断左右环岛
extern uint8  circle_ready_flag;//进环前是否有预标志位
extern uint8  ELC_Circle_at;//处于电磁环岛环境中的标志位

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
