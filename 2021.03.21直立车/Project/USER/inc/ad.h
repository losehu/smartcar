#ifndef __AD_H__
#define __AD_H__

#include "headfile.h"

typedef struct
{
    char time;
    char In_flag;
    char Out_flag;
    char Target_flag;
    char Stop_Flag;

    short cnt;
    short Stop_time;

    int32 time_I;
    int32 In_I;
    int32 In_turn_I;
    int32 Out_I;
} Thressloadtypedef;


typedef struct
{
    char Out_flag;
    char In_flag;
    char In_dis_flag;
    char Speed;
    char In_process;
    char frequency;

    short cnt;

    int32 In_dis_I;
    int32 In_I;
    int32 In_turn_I;
    int32 Out_I;
} Roundabouttypedef;

typedef struct
{
    char In_flag;

    int32 In_I;
} Rampwaytypedef;

extern Thressloadtypedef thressload;
extern Roundabouttypedef roundabout;
extern Rampwaytypedef rampway;

extern float Tracking_Sensor_data_1_LCD;
extern float Tracking_Sensor_data_2_LCD;
extern float Tracking_Sensor_data_3_LCD;
extern float Tracking_Sensor_data_4_LCD;
extern float Tracking_Sensor_data_5_LCD;

extern float point_center;
extern float last_point_center;

void  Tracking_Sensor_Detect();
void Thressload(void);
void Huandao(void);
void Rampway(void);

#endif