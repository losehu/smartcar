#ifndef _DEVICES_H_
#define _DEVICES_H_

#include "headfile.h"

//===============================================ALL======================================//

void Decices_Init(void);

//===============================================LED======================================//


#define    LED0_PIN     D14
#define    LED0_ON      gpio_set(LED0_PIN,0)
#define    LED0_OFF     gpio_set(LED0_PIN,1)
#define    LED0_TOGGLE  gpio_toggle(LED0_PIN)


#define    P9813_SDA_PIN   B10
#define    P9813_SCL_PIN   B11
#define    P9813_SCL(x)   gpio_set(P9813_SCL_PIN,x)
#define    P9813_SDA(x)   gpio_set(P9813_SDA_PIN,x)
#define    RGBLEDOFF    RGBLED(BLACK);


//常用颜色
#define    RGB24RED		    0x00ff0000				// 红色
#define    RGB24BLUE		0x000000ff				// 蓝色
#define    RGB24YELLOW		0x00ffff00				// 黄色
#define    RGB24GREEN		0x0000ff00				// 绿色
#define    RGB24WHITE		0x00ffffff				// 白色
#define    RGB24BLACK		0x00000000				// 黑色 
#define    RGB24GRAY		0X00d3d3d3				// 灰色
#define    RGB24BROWN		0X00a52a2a				// 棕色
#define    RGB24PURPLE		0X00a020f0				// 紫色
#define    RGB24PINK		0X00ffc0cb				// 粉色



//fucntion
void  LED_Test(void);
void  RGBLED_test(void);
void  RGBLED(uint32 COLOR32BIT);


//===============================================BEEP======================================//


#define BEEP_PIN       H2
#define BEEP_ON        gpio_set(BEEP_PIN,1)
#define BEEP_OFF       gpio_set(BEEP_PIN,0)
#define BEEP_TOGGLE    gpio_toggle(BEEP_PIN)
#define ON    1
#define OFF   0



//statement
typedef struct
{
    uint16 State;
    uint16 Num;
    uint16 Time;
    uint32 Cnt;
} BeepTypedef;

extern BeepTypedef Beep;


//function
void Beep_DiDi(uint16 n, uint16 time);
void BeepInt(void);


//===============================================ENCODER======================================//


//define
#define PAI             3.14159265358979f            //圆周率
#define BodyR           9.0f                        //轴向半径
#define Encoder1mCnt	45000
#define RealSpeedK 		0.2142
//编码器
#define ENCODER1_TIM		TIM_3
#define ENCODER1_A			TIM_3_ENC1_B04
#define ENCODER1_B			TIM_3_ENC2_B05
#define ENCODER1_DIR   		B5

#define ENCODER2_TIM		TIM_4
#define ENCODER2_A			TIM_4_ENC1_B06
#define ENCODER2_B			TIM_4_ENC2_B07
#define ENCODER2_DIR   		B7
//检测出来的编码器方向
#define    ENCODER_L_DIR    0
#define    ENCODER_R_DIR    0
//statement
typedef struct
{
    int32 encoder;
    float now;
    float last;
    float acc;
    float out;
} EncoderTypedef;


typedef struct
{
    float LSpeed;
    float RSpeed;
    float now;
    float last;
    float DistanceAll;
} WheelTypedef;


extern EncoderTypedef L_Wheel, R_Wheel;
extern WheelTypedef Wheel;


//function
void Encoder_Read(void);
uint8 Encoder_Show(void);


//===========================================MOTOR==========================================//


//define
#define PWM_TIM				TIM_5
#define PWM_L0				TIM_5_CH1_A00
#define PWM_L1				TIM_5_CH2_A01
#define PWM_R0				TIM_5_CH3_A02
#define PWM_R1				TIM_5_CH4_A03

#define SERVOPWM_TIM		TIM_2
#define PWM_SERVO			TIM_2_CH4_B11
//输出限幅 限制在1000以内
#define    pwm_limit(x)  		( (x)>0? ((x)>1000? ((x)=1000):(x)) : ((x)<-1000? ((x)=-1000):(x)) )

//statement
extern short pwm_l;
extern short pwm_r;

extern short Servo_mid;
extern short Servo_out;
extern short Servo_add;


//fucntion
void Mortordriver_Init(void);
void PWM_Write(void);
void Motor_Show(void);
void SERVO_Write(void);
void Servo_Test(void);


//===========================================KEY==========================================//
typedef enum
{
    nopress = 0,
    onepress,//短按一下
    alwayspress,//一直按着
} KeyStateEnum;


//define
#define    KEY_CENTER_PIN 	C5
#define    KEY_UP_PIN 	 	B1
#define    KEY_DOWN_PIN	    F11
#define    KEY_LEFT_PIN 	C4
#define    KEY_RIGHT_PIN 	B2
#define    PAGE_UP_PIN		F12
#define    PAGE_DOWN_PIN    B0
//center  E5
//up	 C13
//down	E4
//l    E6
//r		E3


#define    DIP_1_PIN 	    A9
#define    DIP_2_PIN 	    A10
#define    DIP_3_PIN        A11
#define    DIP_4_PIN  	    A12

#define    KEY_CENTER   gpio_get(KEY_CENTER_PIN)
#define    KEY_UP 		gpio_get(KEY_UP_PIN)
#define    KEY_DOWN 	gpio_get(KEY_DOWN_PIN)
#define    KEY_LEFT 	gpio_get(KEY_LEFT_PIN)
#define    KEY_RIGHT	gpio_get(KEY_RIGHT_PIN)
#define    KEY_PAGE_UP	gpio_get(PAGE_UP_PIN)
#define    KEY_PAGE_DOWN gpio_get(PAGE_DOWN_PIN)
//#define    DIP_1 	    gpio_get(DIP_1_PIN)
//#define    DIP_2 	    gpio_get(DIP_2_PIN)
//#define    DIP_3       gpio_get(DIP_3_PIN)
//#define    DIP_4	     gpio_get(DIP_4_PIN)

extern KeyStateEnum KeyCenter, KeyUp, KeyDown, KeyLeft, KeyRight;
extern KeyStateEnum KeyPageUp, KeyPageDown;

//fucntion
void Key_Init(void);
void DIP_Scan(void);
unsigned char Key_Scan(void);
void Key_Test(void);
void KeyScanInt(void);


#endif


