#ifndef _APP_FILTER_H_
#define _APP_FILTER_H_

#include "headfile.h"


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























#endif


