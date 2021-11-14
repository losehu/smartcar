#ifndef _devices_h_
#define _devices_h_

#include "headfile.h"


//===============================================BEEP======================================//


#define BEEP_PIN       B11
#define BEEP_ON        gpio_set(BEEP_PIN,1)
#define BEEP_OFF       gpio_set(BEEP_PIN,0)
#define BEEP_TOGGLE    gpio_toggle(BEEP_PIN)
#define ON    1
#define OFF   0



//statement
typedef struct
{
	uint16 state;
	uint16 num;
	uint16 time;
	uint32 cnt;
    void (*function)(void);
}Beep;

extern Beep beep; 

//function
extern void BeepInt(void); //这个extern就应该在这，因为beep.function执行是这个函数
extern void BeepDiDi(uint16 n,uint16 time);

//===========================================KEY==========================================//

//define    板子上从上到下：上下左右
#define    KEY_UP_PIN 	    C31
#define    KEY_DOWN_PIN 	C27	
#define    KEY_LEFT_PIN 	C26
#define    KEY_RIGHT_PIN    C4 

//#define    DIP_1_PIN 	    A9
//#define    DIP_2_PIN 	    A10
//#define    DIP_3_PIN        A11
//#define    DIP_4_PIN  	    A12

//#define    KEY_CENTER   gpio_get(KEY_CENTER_PIN)
#define    KEY_UP 		gpio_get(KEY_UP_PIN)
#define    KEY_DOWN 	gpio_get(KEY_DOWN_PIN)
#define    KEY_LEFT 	gpio_get(KEY_LEFT_PIN)
#define    KEY_RIGHT	gpio_get(KEY_RIGHT_PIN)
//#define    KEY_PAGE_UP	gpio_get(PAGE_UP_PIN)
//#define    KEY_PAGE_DOWN gpio_get(PAGE_DOWN_PIN)
//#define    DIP_1 	    gpio_get(DIP_1_PIN)
//#define    DIP_2 	    gpio_get(DIP_2_PIN)
//#define    DIP_3       gpio_get(DIP_3_PIN)
//#define    DIP_4	     gpio_get(DIP_4_PIN)

#define DIALSWITCH1 D27
#define DIALSWITCH2 D4

typedef enum
{
	noPress = 0,
	onePress,//短按一下
	alwaysPress,//一直按着
} KeyStateEnum;
typedef enum
{
	upUp,
	upDown,
	downUp,
	downDown,
}DialSwicth;
typedef struct
{
    KeyStateEnum up;
    KeyStateEnum down;
    KeyStateEnum left;
    KeyStateEnum right; 
	DialSwicth mode;
    void (*function)(void);
}Key;
//extern KeyStateEnum KeyPageUp,KeyPageDown;
extern Key key;
//fucntion
extern void DialSwitch();
extern void KeyScanInt(void);

//===========================================MOTOR==========================================//


//define 
#define PWM_TIM				TIM_5
#define PWM_L0				TIM_5_CH1_A00
#define PWM_L1				TIM_5_CH2_A01
#define PWM_R0				TIM_5_CH3_A02
#define PWM_R1				TIM_5_CH4_A03
//输出限幅 限制在50000以内
#define    pwm_limit(x)  		( (x)>0? ((x)>50000? ((x)=50000):(x)) : ((x)<-50000? ((x)=-50000):(x)) )

//statement
typedef struct
{
    int16 fix;
	int16 normal;
	int16 circle;
	int16 use;
}ExpectSpeed;
typedef struct
{
    uint16 differential;
    int16  expectSpeed;
	int16  speed;
    int32  pwm;
}Speed;
typedef struct
{
    ExpectSpeed expectSpeed;
    Speed  left;
    Speed  right;
    void (*function)(void);
}Motor;
extern Motor motor;
//fucntion
void PWM_Write(void);
//===========================================SERVO==============================================================================================================================//

typedef struct
{
    uint16 duty;
    int32  dutyAdd;
    int    expectPosition;
	int32  normalMid;
    int32  nowMid;
	uint16 rangeProtect;
    void (*function)(void);
}Servo;
extern Servo servo;

//===============================================ENCODER======================================//

//检测出来的编码器方向
typedef enum
{
    positive,
    negative,
}EncoderDIREnum;

//statement
typedef struct
{
	int32 encoder;
	int16 now;
	int16 last;
	int16 acc;	
	int16 out;
    EncoderDIREnum dir;
}Encoder;


typedef struct
{
	int16 leftSpeed;
	int16 rightSpeed;
	int16 speed;
	int16 lastSpeed;
	int16 distanceAll;
	int16 leftDistance;
	int16 rightDistance;
    Encoder left;
    Encoder right;
    void (*function)(void);
}Wheel;

extern Wheel wheel;

//function
extern void EncoderRead(void);

//===============================================Debug======================================// 

typedef struct
{
	bool lcdIsOpen;
	bool keyIsOpen;
	bool ANODTIsOpen;
}DebugTask;
typedef struct
{
    bool open;
    bool stopProtect;
	DebugTask debugTask; 
    void (*function)(void);
}Debug;
extern Debug debug;
extern void DebugOpenInterrupt();
extern void DebugCloseInterrupt();
extern void DebugOpenScan();
//===============================================Gyroscope======================================// 

#define AcceRatio   4096.0f
#define GyroRatio   16.4f
#define ACC_FILTER_NUM  2        // 加速度计滤波深度
#define GYRO_FILTER_NUM 2       // 陀螺仪滤波深度

extern int64 Circle_angle0;
extern int16 Circle_angle;

extern void ICM20602_Offset(void);  //置零偏
extern void GetGyroscope();



typedef struct
{
	int16 x;
	int16 y;
	int16 z;
}ICM20602xyz;


typedef struct
{
		ICM20602xyz gyro;
		ICM20602xyz accdata;
}ICM20602Data;

typedef struct
{
		ICM20602Data offset;
		ICM20602Data raw;
		ICM20602Data processed;
}ICM20602;

extern ICM20602 icm;

//===============================================LED======================================// 



#define REDPIN    C3
#define BLUEPIN   C25
#define GREENPIN  C5


typedef enum
{
	pwm = 0,
	io,
}ColorControlEnum;
typedef enum
{
	red = 0,
	green,
	blue,
    white,
}ColorEnum;

typedef struct
{
	ColorControlEnum control;
	ColorEnum colorColor;
}Color;


extern void LedInit(void);
extern void LedColor(ColorEnum Color);
#endif
