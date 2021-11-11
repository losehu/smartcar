#ifndef _ELC_H_
#define _ELC_H_

#include "headfile.h"


#define L1 2
#define L2 0
#define L3 3
#define L4 1

#define ELC_NUM     4       //电感总数
#define ELC_GROUP   3       //采集组数
#define ELC_TIME    3       //每组的采集次数
#define ELC_NEED    1       //排序完后的所需值


extern uint16   ELC[ELC_GROUP][ELC_TIME][ELC_NUM];
extern int32    LNow[ELC_NUM], LMax[ELC_NUM], LMin[ELC_NUM];
extern float    LNor[ELC_NUM];//归一化的数值
extern uint16   LNORMAX[ELC_NUM];
extern uint8    MaxNorIndex;
extern uint8    MinNorIndex;
extern uint16   ELC_Sum_14;
extern uint16   ELC_Sum_23;
extern uint16   ELC_Sum_1234;
extern int16    ELC_Center_14;
extern int16    ELC_Center_23;
extern uint8    ELC_PROTECT_FLAG;
extern uint16   KFP_ELC_Sum_14;
extern uint16   KFP_ELC_Sum_23;
extern float  ELC_Center_14_100;
//function
void    InductorParam_Init(void);
uint8   Inductor_Read(void);
void    ELC_Normalized(void);
void    ELC_Center_Calculate(void);
uint8   ELC_Judge(void);
uint8   ELC_Show(void);
void    ELC_Status_Init(void);



//---------------------------------------------------环岛---------------------------------------//


typedef struct
{
    uint8 FindFlag;//发现标志
    uint8 SUM_MAX;//没有削减前的最大值
    uint8 NowValLessThanMaxCnt;//削减计数标志
    uint8 CircleInTurnFlag;
} ELC_CircleTypedef;


//statement
extern ELC_CircleTypedef ELC_Circle;


//fun
uint8   ELC_Circle_Judge(void);

#endif
