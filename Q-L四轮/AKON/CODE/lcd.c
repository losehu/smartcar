#include "lcd.h"
/*
ips显示函数  液晶显示字符   ips200_showchar(uint16 x,uint16 y,const int8 dat)   ips200_showchar(0,0,'x');//坐标0,0写一个字符x
               液晶显示字符串 ips200_showstr(uint16 x,uint16 y,const int8 dat[])  ips200_showstr(0,0,"seekfree");   
               液晶显示8位有符号 ips200_showint8(uint16 x,uint16 y,int8 dat)       ips200_showint8(0,0,x);//x为int8类型
               液晶显示浮点数(去除整数部分无效的0) ips200_showfloat(uint16 x,uint16 y,double dat,int8 num,int8 pointnum) ips_showfloat(0,0,x,2,3);//显示浮点数 
NOTION:颜色是大写的，eg：红色 RED
*/



Lcd lcd;
void MyLcdDouble(uint16 y,const int8 dat[], double num)
{
	ips200_showstr(10,y,dat);
	ips200_showfloat(130,y,num,4,2);
}
void MyLcdUint32(uint16 y,const int8 dat[], uint32 num)
{
	ips200_showstr(10,y,dat);
	ips200_showuint32(130,y,num,4);
}
void MyLcdInt32(uint16 y,const int8 dat[], int32 num)
{
	ips200_showstr(10,y,dat);
	ips200_showint32(130,y,num,4);
}
void MyLcdBool(uint16 y,const int8 dat[], bool boolnum)
{
	ips200_showstr(10,y,dat);
	if(boolnum == true)
		ips200_showstr(130,y,"True");
	else ips200_showstr(130,y,"False");
}
void MyLcd_aux(uint16 y, const int8 dat[], int32 typeval,...)
{
	switch(typeval)
	{
		case 1:
		{//double
			va_list v;
            va_start(v, typeval);

            float s = va_arg(v, double);

            va_end(v);

            MyLcdDouble(y,dat,s);
			
			break;
		}
		case 2:
		{//uint32
			va_list v;
            va_start(v, typeval);

            uint32 s = va_arg(v, uint32);

            va_end(v);

            MyLcdUint32(y,dat,s);
			
			break;
		}
		case 3:
		{//int32
			va_list v;
            va_start(v, typeval);

            int32 s = va_arg(v, int32);

            va_end(v);

            MyLcdInt32(y,dat,s);
			
			break;
		}
		case 4:
		{//float
			va_list v;
            va_start(v, typeval);

            float s = va_arg(v, double);

            va_end(v);

            MyLcdDouble(y,dat,s);
			
			break;
		}
		case 5:
		{//int16
			va_list v;
            va_start(v, typeval);

            int32 s = va_arg(v, int32);

            va_end(v);

            MyLcdInt32(y,dat,s);
			
			break;
		}
		case 6:
		{//uint16
			va_list v;
            va_start(v, typeval);

            uint32 s = va_arg(v, uint32);

            va_end(v);

            MyLcdUint32(y,dat,s);
			
			break;
		}
		case 7:
		{//int8
			va_list v;
            va_start(v, typeval);

            int32 s = va_arg(v, int32);

            va_end(v);

            MyLcdInt32(y,dat,s);
			
			break;
		}
		case 8:
		{//uint8
			va_list v;
            va_start(v, typeval);

            uint32 s = va_arg(v, uint32);

            va_end(v);

            MyLcdUint32(y,dat,s);
			
			break;
		}
		case 9:
		{//bool
			va_list v;
            va_start(v, typeval);

            bool s = va_arg(v, int);

            va_end(v);

            MyLcdBool(y,dat,s);
			
			break;
		}
		default:
		{
			BEEP_ON;
			while(1);
		}
	}
}
#define MyLcd(y, dat, type)\
    MyLcd_aux(y, dat,\
          __builtin_types_compatible_p(typeof(type), double) * 1\
        + __builtin_types_compatible_p(typeof(type), uint32) * 2\
		+ __builtin_types_compatible_p(typeof(type),  int32) * 3\
		+ __builtin_types_compatible_p(typeof(type), float)  * 4\
	    + __builtin_types_compatible_p(typeof(type),  int16) * 5\
		+ __builtin_types_compatible_p(typeof(type), uint16) * 6\
		+ __builtin_types_compatible_p(typeof(type),  int8)  * 7\
		+ __builtin_types_compatible_p(typeof(type), uint8)  * 8\
		+ __builtin_types_compatible_p(typeof(type), bool)   * 9\
        , type)

void LcdInit()
{
    ips200_init();			//初始化2.0寸IPS屏幕
    lcd.page = 0;
    lcd.cursor = 0;
}

uint8 GetKeyState(void)  //获取按键目前的状态
{
    if( key.mode == upDown || key.mode == downDown)
    {
        return 1;
    }
    else if (key.mode == upUp || key.mode == downUp)
    {
        return 0;
    }
    return 2;
}
//----------------------------------------------------------------
//  @brief        获取目前拨码开关状态
//  @param        void
//  @return       void
//  Sample usage: KeyState(); 
//----------------------------------------------------------------
//Statement
LcdStateEnum keyState;  //目前屏幕拨码开关状态
//Function
void KeyState()  
{
    if(GetKeyState() == 1 )  keyState = Select;
    else if(GetKeyState() == 0 )  keyState = Change;
}


void CursorMove(uint8 direction, int8 moveDistance) //光标移动
{
    if( direction == Up)
    {
        lcd.cursor -= moveDistance;
    }
    else if( direction == Down)
    {
        lcd.cursor += moveDistance;
    }
	lcd.cursor = range_protect(lcd.cursor, 0, 15);
}
//----------------------------------------------------------------
//  @brief        上下
//  @param        void
//  @return       void
//  Sample usage: UpAndDown(); 
//----------------------------------------------------------------
void UpAndDown()  
{
    if(key.up == onePress || key.up == alwaysPress)
    {
        key.up = noPress;
        CursorMove(Up, 1);
        ips200_clear(IPS200_BGCOLOR);
    }
    else if(key.down == onePress || key.down == alwaysPress)
    {
        key.down = noPress;
        CursorMove(Down, 1);
        ips200_clear(IPS200_BGCOLOR);
    }
    
}

void LcdPageZero()  //目录
{
    ips200_showstr(80, 0, "CATALOG");
    
    ips200_showstr(10, 1, "Page One   - Motor");
    ips200_showstr(10, 2, "Page Two   - Servo");
    ips200_showstr(10, 3, "Page Three - SwitchFunction");
    ips200_showstr(10, 4, "Page Four  - CamPicture");
    ips200_showstr(10, 5, "Page Five  - ELC");
    ips200_showstr(10, 6, "Page Six   - Gyroscope");
    ips200_showstr(10, 7, "Page Seven - Circle");
		
    
}
void LcdPageOne()   //第一页：电机PID和信息
{
    ips200_showstr(80, 0, "Page One");
    
	MyLcd(1,"_LeftKP",pid.leftMotor.kp);
	MyLcd(2,"_LeftKi",pid.leftMotor.ki);
	MyLcd(3,"_LeftKd",pid.leftMotor.kd);
    
	MyLcd(4,"_RightKp",pid.rightMotor.kp);
	MyLcd(5,"_RightKi",pid.rightMotor.ki);
	MyLcd(6,"_RightKd",pid.rightMotor.kd);
    
    MyLcd(7,"_NormalExpect",motor.expectSpeed.normal);
	MyLcd(8,"_CircleExpect",motor.expectSpeed.normal);
	MyLcd(9,"ExpectLeft",motor.left.expectSpeed);
	MyLcd(10,"ExpectRight",motor.right.expectSpeed);
	MyLcd(11,"RightSpeed",wheel.right.now);
	MyLcd(12,"LeftSpeed",wheel.left.now);

    MyLcd(13,"CamError",camServoError);

}
void LcdPageTwo()  //第二页：舵机
{
    ips200_showstr(80, 0, "Page Two");
	
	MyLcd(1,"_ServoKp1",pid.servo.cam.normal.kp1);
	MyLcd(2,"_ServoKp2",pid.servo.cam.normal.kp2);
	MyLcd(3,"_ServoKd",pid.servo.cam.normal.kd);
	
	MyLcd(4,"Error",camServoError);
	MyLcd(5,"ServoDutyAdd",servo.dutyAdd);
	MyLcd(6,"ServoDuty",servo.dutyAdd);
	MyLcd(7,"_Mid",servo.normalMid);
	MyLcd(8,"NowMid",servo.nowMid);

    
}
void LcdPageThree() //第三页：Function开关
{
	
    ips200_showstr(80, 0, "Page Three");
    
	MyLcd(1,"_ServoOpen", task[servoTask].isOpen);
	MyLcd(2,"_StopProtectOpen",debug.stopProtect);
	MyLcd(3,"_ANODTOpen",task[ANODTTask].isOpen);
}

extern int16 all_lose_count;
extern int16 left_lose_count;
extern int16 right_lose_count;
void LcdPageFour()  //第四页：Cam图像
{
	
    Display_Pitcure(0,0,1);
	
	MyLcd(5, "Threshold", Threshold);
	MyLcd(6, "Error", camServoError);
	MyLcd(7, "DutyAdd", servo.dutyAdd);
	
	MyLcd(8, "_ServoKp1", pid.servo.cam.normal.kp1);
	MyLcd(9, "_ServoKp2", pid.servo.cam.normal.kp2);
	MyLcd(10, "_ServoKd", pid.servo.cam.normal.kd);
	
	MyLcd(11, "_Mid", servo.nowMid);
	
	MyLcd(12, "servo_sum_flag", servo_sum_flag);
	MyLcd(13, "angel_count", angel_count);
	MyLcd(14, "left_lose_count", left_lose_count);
	MyLcd(15, "right_lose_count", right_lose_count);
	MyLcd(16, "all_lose_count", all_lose_count);

}
extern uint8 circleState;
extern uint8  circle_ready_flag;
void LcdPageFive()  //第五页显示内容 电感值
{
    ips200_showstr(80, 0, "Page Five");
    
	MyLcd(1, "L2", adc.l2.value);
	MyLcd(2, "L1", adc.l1.value);
	MyLcd(3, "R1", adc.r1.value);
	MyLcd(4, "R2", adc.r2.value);
	
	MyLcd(5, "Sum", adc.sum);
	MyLcd(6, "ELC Error", pid.servo.elc.error);
	MyLcd(7, "CircleState", circleState);
	MyLcd(8, "circle_ready_flag", circle_ready_flag);


	
    
}
void LcdPageSix()   //第六页陀螺仪
{
    ips200_showstr(80, 0, "Page Six");
    
    ips200_showstr(10, 1, "Gyro X");
    ips200_showint16(130, 1, icm.processed.gyro.x);
    ips200_showstr(10, 2, "Gyro Y");
    ips200_showint16(130, 2, icm.processed.gyro.y);
    ips200_showstr(10, 3, "Gyro Z");
    ips200_showint16(130, 3, icm.processed.gyro.z);
	ips200_showstr(10, 4, "ACC  X");
    ips200_showint16(130, 4, icm.processed.accdata.x);
    ips200_showstr(10, 5, "ACC  Y");       
    ips200_showint16(130, 5, icm.processed.accdata.y);
    ips200_showstr(10, 6, "ACC  Z");       
    ips200_showint16(130, 6, icm.processed.accdata.z);
	ips200_showstr(10, 7, "Gyro X RAW");
    ips200_showint16(130, 7, icm.raw.gyro.x);
    ips200_showstr(10, 8, "Gyro Y RAW");
    ips200_showint16(130, 8, icm.raw.gyro.y);
    ips200_showstr(10, 9, "Gyro Z RAW");
    ips200_showint16(130, 9, icm.raw.gyro.z);
    ips200_showstr(10, 10, "Circle_angle");
    ips200_showint16(130, 10, Circle_angle);
    ips200_showstr(10, 11, "Circle_angle0");
    ips200_showint32(130, 11, Circle_angle0, 5);
	
	ips200_showstr(10, 12, "CircleState");
    ips200_showuint8(130, 12, circleState);

	  
	 
}



void LcdPageSeven() //第七页：环岛
{
    ips200_showstr(80, 0, "Page Seven");
    if(hd_zuo == 1)   ips200_showstr(10, 1, "Circle is Left");
	else if(hd_you == 1) ips200_showstr(10, 1, "Circle is Right");

    ips200_showstr(10, 2, "ELC_Circle_flag");
    ips200_showuint8(130, 2, ELC_Circle_flag);
	ips200_showstr(10,   3, "ELC_Circle_turn");
    ips200_showuint8(130, 3, ELC_Circle_turn);
	ips200_showstr(10, 4, "angel_count");
    ips200_showint32(130, 4, angel_count, 4);
	
	ips200_showstr(10, 5, "left_lose_count");
    ips200_showuint16(130, 5, left_lose_count);
	ips200_showstr(10, 6, "right_lose_count");
    ips200_showuint16(130, 6, right_lose_count);
	ips200_showstr(10, 7, "all_lose_count");
    ips200_showuint16(130, 7, all_lose_count);
    
}

void LcdPageEight() //第八页
{
    ips200_showstr(80, 0, "Page Eight");
}
void KEYSTATESelect()   //每页内容显示
{
    switch(lcd.page)
    {
        case 0:
            LcdPageZero();  //目录
            break;
        case 1:
            LcdPageOne();   //电机PID
            break;
        case 2:
            LcdPageTwo();   //开关保护
            break;
        case 3:
            LcdPageThree(); //舵机PID
            break;
        case 4:
            LcdPageFour();  //摄像头图像
            break;
        case 5:
            LcdPageFive();  //电感值
            break;
        case 6:
            LcdPageSix();   //陀螺仪
            break;
        case 7:
            LcdPageSeven(); //环岛
            break;
        case 8:
            LcdPageEight();
            break;
				default:
						break;
    }
}

void ConfirmAndBack()   //确认键和返回键
{
    if(key.left == onePress || key.left == alwaysPress)
    {
        key.left = noPress;
        switch(lcd.page)
        {
            //在第一页的话是跳转到其他页面
            case 0:
                lcd.page = lcd.cursor;
				ips200_clear(IPS200_BGCOLOR); 
				KEYSTATESelect();
                break;
            case 1:
            case 2:
            case 3:
			case 4:
                FlashWrite();
                FlashRead();
                break;
			default:
                break;
                
        }
        
    }
    else if((key.right == onePress || key.right == alwaysPress) && lcd.page != 0)
    {
        key.right = noPress;
        lcd.page = 0;
        ips200_clear(IPS200_BGCOLOR);
        KEYSTATESelect();
    }
}
void ShowArrow(uint16 x, uint16 y, uint16 color, uint8 direction) //画一个箭头，x，y和其他函数一样，起始位置
{
    int i;
    y = y*16;
    for(i = 0; i < ARROWLENGTH; i++)  //画箭头的横线
    {
        ips200_drawpoint(x+i, y+ARROWWIDTH/2, color);
    }
    switch(direction)
    {
        case Right:
            for(i = 0; i < ARROWWIDTH/2; i++)//画箭头的两条斜线
            { 
                ips200_drawpoint(x+i+ARROWWIDTH/2, y+i, color);
                ips200_drawpoint(x+i+ARROWWIDTH/2, y-i+ARROWWIDTH, color);
            }
            break;
        case Left:
            for(i = 0; i < ARROWWIDTH/2; i++)//画箭头的两条斜线
            { 
                ips200_drawpoint(x+i, y-i+ARROWWIDTH/2, color);
                ips200_drawpoint(x+i, y+i+ARROWWIDTH/2, color);
            }
            break;
        default:
            BEEP_ON;
						while(1)
						{
							PRINTF("switch(direction) in ShowArrow(uint16 x, uint16 y, uint16 color, uint8 direction) Function Error");
						}
            break;
    }
}
void ChangeParamInt32(int32 *Param) //之后的只需要使用ChangeParam（）；就好--用于修改模式中的修改int32型
{
	if(Param)
	{
		if(key.up == onePress || key.up == alwaysPress)
		{
			key.up = noPress;
			*Param = *Param + 1;
			ips200_clear(IPS200_BGCOLOR);
			KEYSTATESelect();
		}
		else if(key.down == onePress || key.down == alwaysPress)
		{
			key.down = noPress;
			*Param = *Param - 1;
			ips200_clear(IPS200_BGCOLOR);
			KEYSTATESelect();
		}
	}
	else
	{
		BEEP_ON;
		while(1)
		{
			PRINTF("Param in ChangeParamInt16(int16 *Param) Function Error");
		}
	}
}
void ChangeParamUint32(uint32 *Param) //用于修改模式中的修改uint32型
{
	if(Param)
	{
		if(key.up == onePress || key.up == alwaysPress)
		{
			key.up = noPress;
			*Param = *Param + 1;
			ips200_clear(IPS200_BGCOLOR);
			KEYSTATESelect();
		}
		else if(key.down == onePress || key.down == alwaysPress)
		{
			key.down = noPress;
			*Param = *Param - 1;
			ips200_clear(IPS200_BGCOLOR);
			KEYSTATESelect();
		}
	}
	else
	{
		BEEP_ON;
		while(1)
		{
			PRINTF("Param in ChangeParamInt16(int16 *Param) Function Error");
		}
	}
}
void ChangeParamInt16(int16 *Param) //用于修改模式中的修改int16型
{
	if(Param)
	{
		if(key.up == onePress || key.up == alwaysPress)
		{
			key.up = noPress;
			*Param = *Param + 1;
			ips200_clear(IPS200_BGCOLOR);
			KEYSTATESelect();
		}
		else if(key.down == onePress || key.down == alwaysPress)
		{
			key.down = noPress;
			*Param = *Param - 1;
			ips200_clear(IPS200_BGCOLOR);
			KEYSTATESelect();
		}
	}
	else
	{
		BEEP_ON;
		while(1)
		{
			PRINTF("Param in ChangeParamInt16(int16 *Param) Function Error");
		}
	}
}

void ChangeParamUint16(uint16 *Param) //用于修改模式中的修改uint16型
{
	if(Param)
	{
    if(key.up == onePress || key.up == alwaysPress)
    {
        key.up = noPress;
        *Param = *Param + 1;
        ips200_clear(IPS200_BGCOLOR);
        KEYSTATESelect();
    }
    else if(key.down == onePress || key.down == alwaysPress)
    {
        key.down = noPress;
        *Param = *Param - 1;
        ips200_clear(IPS200_BGCOLOR);
        KEYSTATESelect();
    }
	}
	else
	{
		BEEP_ON;
		while(1)
		{
			PRINTF("Param in ChangeParamUint16(uint16 *Param) Function Error");
		}
	}
}
void ChangeParamInt8(int8 *Param) //用于修改模式中的修改uint16型
{
	if(Param)
	{
    if(key.up == onePress || key.up == alwaysPress)
    {
        key.up = noPress;
        *Param = *Param + 1;
        ips200_clear(IPS200_BGCOLOR);
        KEYSTATESelect();
    }
    else if(key.down == onePress || key.down == alwaysPress)
    {
        key.down = noPress;
        *Param = *Param - 1;
        ips200_clear(IPS200_BGCOLOR);
        KEYSTATESelect();
    }
	}
	else
	{
		BEEP_ON;
		while(1)
		{
			PRINTF("Param in ChangeParamInt8(int8 *Param) Function Error");
		}
	}
}
void ChangeParamUint8(uint8 *Param) //用于修改模式中的修改uint16型
{
	if(Param)
	{
    if(key.up == onePress || key.up == alwaysPress)
    {
        key.up = noPress;
        *Param = *Param + 1;
        ips200_clear(IPS200_BGCOLOR);
        KEYSTATESelect();
    }
    else if(key.down == onePress || key.down == alwaysPress)
    {
        key.down = noPress;
        *Param = *Param - 1;
        ips200_clear(IPS200_BGCOLOR);
        KEYSTATESelect();
    }
	}
	else
	{
		BEEP_ON;
		while(1)
		{
			PRINTF("Param in ChangeParamUint8(uint8 *Param) Function Error");
		}
	}
}
void ChangeParamInt(int *Param) //用于修改模式中的修改int型
{
	if(Param)	//指针不为空
	{
		//正常执行代码
    if(key.up == onePress || key.up == alwaysPress)
    {
        key.up = noPress;
        *Param = *Param + 1;
        ips200_clear(IPS200_BGCOLOR);
        KEYSTATESelect();
    }
    else if(key.down == onePress || key.down == alwaysPress)
    {
        key.down = noPress;
        *Param = *Param - 1;
        ips200_clear(IPS200_BGCOLOR);
        KEYSTATESelect();
    }
	}
	else
	{
		//错误执行代码
		BEEP_ON;
		while(1)
		{
			PRINTF("Param in ChangeParamInt(int *Param) Function Error");
		}
	}
}
void ChangeParamFloat(float *Param)//用于修改模式中的修改float型
{
	if(Param)
	{
    if(key.up == onePress || key.up == alwaysPress)
    {
        key.up = noPress;
        *Param = *Param + 1;
        ips200_clear(IPS200_BGCOLOR);
        KEYSTATESelect();
    }
    else if(key.down == onePress || key.down == alwaysPress)
    {
        key.down = noPress;
        *Param = *Param - 1;
        ips200_clear(IPS200_BGCOLOR);
        KEYSTATESelect();
    }
	}
	else
	{
		BEEP_ON;
		while(1)
		{
			PRINTF("Param in ChangeParamFloat(float *Param) Function Error");
		}
	}
}
void ChangeParamDouble(double *Param)//用于修改模式中的修改double型
{
	if(Param)
	{
    if(key.up == onePress || key.up == alwaysPress)
    {
        key.up = noPress;
        *Param = *Param + 1;
        ips200_clear(IPS200_BGCOLOR);
        KEYSTATESelect();
    }
    else if(key.down == onePress || key.down == alwaysPress)
    {
        key.down = noPress;
        *Param = *Param - 1;
        ips200_clear(IPS200_BGCOLOR);
        KEYSTATESelect();
    }
	}
	else
	{
		BEEP_ON;
		while(1)
		{
			PRINTF("Param in ChangeParamDouble(double *Param) Function Error");
		}
	}
}
void ChangeParamBool(bool *Param)//用于修改模式中的修改bool型
{
	if(Param)
	{
    if(key.up == onePress || key.up == alwaysPress || key.down == onePress || key.down == alwaysPress)
    {
        key.up = noPress;
        key.down = noPress;
        if(*Param) *Param = false;
        else *Param = true;
        ips200_clear(IPS200_BGCOLOR);
        KEYSTATESelect();
    }
	}
	else
	{
		BEEP_ON;
		while(1)
		{
			PRINTF("Param in ChangeParamBool(bool *Param) Function Error");
		}
	}
}

void ChangeParam_aux(int typeval, ...)//重载函数！！！
{
	switch(typeval)
	{
		case 1:
		{
			va_list v;
            va_start(v, typeval);

            int32 *type = va_arg(v, int32 *);

            va_end(v);

            ChangeParamInt32(type);
			break;
		}
		case 2:
		{
			va_list v;
            va_start(v, typeval);

            uint32 *type = va_arg(v, uint32 *);

            va_end(v);

            ChangeParamUint32(type);
			break;
		}
		case 3:
		{
			va_list v;
            va_start(v, typeval);

            int16 *type = va_arg(v, int16 *);

            va_end(v);

            ChangeParamInt16(type);
			break;
		}
		case 4:
		{
			va_list v;
            va_start(v, typeval);

            uint16 *type = va_arg(v, uint16 *);

            va_end(v);

            ChangeParamUint16(type);
			break;
		}
		case 5:
		{
			va_list v;
            va_start(v, typeval);

            int8 *type = va_arg(v, int8 *);

            va_end(v);

            ChangeParamInt8(type);
			break;
		}
		case 6:
		{
			va_list v;
            va_start(v, typeval);

            uint8 *type = va_arg(v, uint8 *);

            va_end(v);

            ChangeParamUint8(type);
			break;
		}
		case 7:
		{
			va_list v;
            va_start(v, typeval);

            int *type = va_arg(v, int *);

            va_end(v);

            ChangeParamInt(type);
			break;
		}
		case 8:
		{
			va_list v;
            va_start(v, typeval);

            float *type = va_arg(v, float *);

            va_end(v);

            ChangeParamFloat(type);
			break;
		}
		case 9:
		{
			va_list v;
            va_start(v, typeval);

            double *type = va_arg(v, double *);

            va_end(v);

            ChangeParamDouble(type);
			break;
		}
		case 10:
		{
			va_list v;
            va_start(v, typeval);

            bool *type = va_arg(v, bool *);

            va_end(v);

            ChangeParamBool(type);
			break;
		}
	}
}

#define ChangeParam(type)\
		ChangeParam_aux(\
          __builtin_types_compatible_p(typeof(type),  int32 *) * 1\
        + __builtin_types_compatible_p(typeof(type),  uint32*) * 2\
		+ __builtin_types_compatible_p(typeof(type),  int16 *) * 3\
        + __builtin_types_compatible_p(typeof(type),  uint16*) * 4\
		+ __builtin_types_compatible_p(typeof(type),  int8  *) * 5\
        + __builtin_types_compatible_p(typeof(type),  uint8 *) * 6\
		+ __builtin_types_compatible_p(typeof(type),  int   *) * 7\
        + __builtin_types_compatible_p(typeof(type),  float *) * 8\
		+ __builtin_types_compatible_p(typeof(type),  double*) * 9\
		+ __builtin_types_compatible_p(typeof(type),  bool  *) * 10\
        , type)
//----------------------------------------------------------------
//  @brief        修改参数。需要和屏幕显示的参数保持一致
//  @param        void
//  @return       void
//  Sample usage: ChangeParam(); 
//----------------------------------------------------------------
void SelectParam()  
{
    switch(lcd.page)
    {
        case 1:
            switch(lcd.cursor)
            {
                case 1:
                    ChangeParam(&pid.leftMotor.kp);
                    break;     
                case 2:        
                    ChangeParam(&pid.leftMotor.ki);
                    break;     
                case 3:        
                    ChangeParam(&pid.leftMotor.kd);
                    break;     
                case 4:        
                    ChangeParam(&pid.rightMotor.kp);
                    break;     
                case 5:        
                    ChangeParam(&pid.rightMotor.ki);
                    break;
                case 6:
                    ChangeParam(&pid.rightMotor.kd);
                    break;
                case 7:
                    ChangeParam(&motor.expectSpeed.normal);
                    break;
				case 8:
					ChangeParam(&motor.expectSpeed.circle);
					break;
				case 9:
					break;
				case 10:
					break;
				case 11:
                    break;
                case 12:
                    ChangeParam(&motor.left.differential);
                    break;
                case 13:
                    ChangeParam(&motor.right.differential);
                    break;
								
				default:
					break;
            }
            break;
         case 2:
             switch(lcd.cursor)
             {
                case 1:
                    ChangeParam(&pid.servo.cam.normal.kp1);
                    break;
                case 2:
                    ChangeParam(&pid.servo.cam.normal.kp2);
                    break;
                case 3:
					ChangeParam(&pid.servo.cam.normal.kd);
                    break;
                case 4:
                    break;
				case 5:
                    break;
				case 6:
					ChangeParam(&servo.nowMid);
                    break;
								
				default:
					break;
             }
             break;
          case 3:
             switch(lcd.cursor)
             {
                 case 1:
				   ChangeParam(&task[servoTask].isOpen);
                     break;
                 case 2:
				     ChangeParam(&debug.stopProtect);
                     break;
                 case 3:
					 ChangeParam(&task[ANODTTask].isOpen);
                     break;
								 
				 default:
					 break;
             }
             break;
		  case 4:
			switch(lcd.cursor)
			{
				case 1:
                     break;
                 case 2:
                     break;
                 case 3:
                     break;
			     case 4:
                     break;
                 case 5:
                     break;
                 case 6:
                     break;
				 case 7:
                     break;
                 case 8:
					 ChangeParam(&pid.servo.cam.normal.kp1);
                     break;
                 case 9:
					 ChangeParam(&pid.servo.cam.normal.kp2);
                     break;	
			     case 10:
					 ChangeParam(&pid.servo.cam.normal.kd);
					 break;
			}
						 
	   	  default:
			 break;
    }
}
//----------------------------------------------------------------
//  @brief        屏幕关闭的回调函数，无须主动调用，在关闭任务的时候自动调用
//  @param        void
//  @return       void
//  Sample usage: LcdCloseInterrupt(); 
//----------------------------------------------------------------
void LcdCloseInterrupt()
{
	BeepDiDi(1, 50);
	ips200_clear(IPS200_BGCOLOR);
}
//----------------------------------------------------------------
//  @brief        屏幕总体函数，包含修改flash
//  @param        void
//  @return       void
//  Sample usage: LcdDisplay(); 
//  Notion        ChangeParam()摆放一定要和KEYSTATESelect()里面一样
//----------------------------------------------------------------
void LcdDisplay()
{
    KeyState();
    ConfirmAndBack();
    switch(keyState)
    {
        case Select:
            UpAndDown();
            KEYSTATESelect();
            break;
        case Change:
            SelectParam();
            break;
		default:
		{
			 BEEP_ON;
			 while(1)
	 		{
				PRINTF("switch(keyState) in LcdDisplay() Function  Error");
			}
	    	break;
		}
    }

}