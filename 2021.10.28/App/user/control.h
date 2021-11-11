#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "headfile.h"


//速度给定
#define BarnOutStraightSpeed        200
#define NormalSpeed                 50//280
#define ForkInSpeed                 270
#define ForkFindOtherCarSpeed       140
#define ForkOutTurnSpeed            180
#define CircleFoundSpeed            250
#define BigCircleTurnInSpeed        230
#define CircleInSpeed               280
#define BiggestCircleInSpeed        300
#define RampSpeed                   230


typedef enum
{
    NOCONTROL = 0,
    ELCMODE = 1,
    CAMMODE = 2
} ControlModeEnum;

typedef enum
{
    NOLoop   = 0,
    SpeedLoop,
    DirectLoop,
    AllLoop,
    LR_SingleControl
} CloseLoopModeEnum;

typedef struct
{
    float RunTime;//s保留三位小数
    float UpTime;//s
    float RunDistance;//m
    uint8 Protect;
    ControlModeEnum ControlMode;
    CloseLoopModeEnum ClosedLoopMode;
    uint8 CAR_PROTECT_MODE;
    float StopTime;
    uint8 ReadFlashParams;
    uint8 Camera_FPS;
    uint8 RiseHeadLoopEnable;
} CarInfoTypedef;


//sattement
extern int32 point_center;
extern int32 point_center0;
extern int32 point_center1;
extern int32 point_center2;
extern int16 CarOutProtectCnt;
extern int16 AccAndRealSpeedProtect;
extern CarInfoTypedef   CarInfo;
extern int16 ExSpeed;
extern int16 L_ExSpeed;
extern int16 R_ExSpeed;
extern uint8 WaveWriteOK;
extern float Gyro_Z_out;
extern int16 DirectOut;
extern int16 RiseHeadOut;
extern int16 SpeedOut;

//fun
uint8 CarSystem_Init(void);
uint8  Center_Cal(void);
void  Timer_IT(void);
void  Car_Protect(void);
uint8 CarInfo_Show(void);
extern int map(float a, int big, int small);

#endif

