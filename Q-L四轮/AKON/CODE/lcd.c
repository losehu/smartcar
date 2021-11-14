#include "lcd.h"
/*
ips��ʾ����  Һ����ʾ�ַ�   ips200_showchar(uint16 x,uint16 y,const int8 dat)   ips200_showchar(0,0,'x');//����0,0дһ���ַ�x
               Һ����ʾ�ַ��� ips200_showstr(uint16 x,uint16 y,const int8 dat[])  ips200_showstr(0,0,"seekfree");   
               Һ����ʾ8λ�з��� ips200_showint8(uint16 x,uint16 y,int8 dat)       ips200_showint8(0,0,x);//xΪint8����
               Һ����ʾ������(ȥ������������Ч��0) ips200_showfloat(uint16 x,uint16 y,double dat,int8 num,int8 pointnum) ips_showfloat(0,0,x,2,3);//��ʾ������ 
NOTION:��ɫ�Ǵ�д�ģ�eg����ɫ RED
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
    ips200_init();			//��ʼ��2.0��IPS��Ļ
    lcd.page = 0;
    lcd.cursor = 0;
}

uint8 GetKeyState(void)  //��ȡ����Ŀǰ��״̬
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
//  @brief        ��ȡĿǰ���뿪��״̬
//  @param        void
//  @return       void
//  Sample usage: KeyState(); 
//----------------------------------------------------------------
//Statement
LcdStateEnum keyState;  //Ŀǰ��Ļ���뿪��״̬
//Function
void KeyState()  
{
    if(GetKeyState() == 1 )  keyState = Select;
    else if(GetKeyState() == 0 )  keyState = Change;
}


void CursorMove(uint8 direction, int8 moveDistance) //����ƶ�
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
//  @brief        ����
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

void LcdPageZero()  //Ŀ¼
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
void LcdPageOne()   //��һҳ�����PID����Ϣ
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
void LcdPageTwo()  //�ڶ�ҳ�����
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
void LcdPageThree() //����ҳ��Function����
{
	
    ips200_showstr(80, 0, "Page Three");
    
	MyLcd(1,"_ServoOpen", task[servoTask].isOpen);
	MyLcd(2,"_StopProtectOpen",debug.stopProtect);
	MyLcd(3,"_ANODTOpen",task[ANODTTask].isOpen);
}

extern int16 all_lose_count;
extern int16 left_lose_count;
extern int16 right_lose_count;
void LcdPageFour()  //����ҳ��Camͼ��
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
void LcdPageFive()  //����ҳ��ʾ���� ���ֵ
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
void LcdPageSix()   //����ҳ������
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



void LcdPageSeven() //����ҳ������
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

void LcdPageEight() //�ڰ�ҳ
{
    ips200_showstr(80, 0, "Page Eight");
}
void KEYSTATESelect()   //ÿҳ������ʾ
{
    switch(lcd.page)
    {
        case 0:
            LcdPageZero();  //Ŀ¼
            break;
        case 1:
            LcdPageOne();   //���PID
            break;
        case 2:
            LcdPageTwo();   //���ر���
            break;
        case 3:
            LcdPageThree(); //���PID
            break;
        case 4:
            LcdPageFour();  //����ͷͼ��
            break;
        case 5:
            LcdPageFive();  //���ֵ
            break;
        case 6:
            LcdPageSix();   //������
            break;
        case 7:
            LcdPageSeven(); //����
            break;
        case 8:
            LcdPageEight();
            break;
				default:
						break;
    }
}

void ConfirmAndBack()   //ȷ�ϼ��ͷ��ؼ�
{
    if(key.left == onePress || key.left == alwaysPress)
    {
        key.left = noPress;
        switch(lcd.page)
        {
            //�ڵ�һҳ�Ļ�����ת������ҳ��
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
void ShowArrow(uint16 x, uint16 y, uint16 color, uint8 direction) //��һ����ͷ��x��y����������һ������ʼλ��
{
    int i;
    y = y*16;
    for(i = 0; i < ARROWLENGTH; i++)  //����ͷ�ĺ���
    {
        ips200_drawpoint(x+i, y+ARROWWIDTH/2, color);
    }
    switch(direction)
    {
        case Right:
            for(i = 0; i < ARROWWIDTH/2; i++)//����ͷ������б��
            { 
                ips200_drawpoint(x+i+ARROWWIDTH/2, y+i, color);
                ips200_drawpoint(x+i+ARROWWIDTH/2, y-i+ARROWWIDTH, color);
            }
            break;
        case Left:
            for(i = 0; i < ARROWWIDTH/2; i++)//����ͷ������б��
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
void ChangeParamInt32(int32 *Param) //֮���ֻ��Ҫʹ��ChangeParam�������ͺ�--�����޸�ģʽ�е��޸�int32��
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
void ChangeParamUint32(uint32 *Param) //�����޸�ģʽ�е��޸�uint32��
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
void ChangeParamInt16(int16 *Param) //�����޸�ģʽ�е��޸�int16��
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

void ChangeParamUint16(uint16 *Param) //�����޸�ģʽ�е��޸�uint16��
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
void ChangeParamInt8(int8 *Param) //�����޸�ģʽ�е��޸�uint16��
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
void ChangeParamUint8(uint8 *Param) //�����޸�ģʽ�е��޸�uint16��
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
void ChangeParamInt(int *Param) //�����޸�ģʽ�е��޸�int��
{
	if(Param)	//ָ�벻Ϊ��
	{
		//����ִ�д���
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
		//����ִ�д���
		BEEP_ON;
		while(1)
		{
			PRINTF("Param in ChangeParamInt(int *Param) Function Error");
		}
	}
}
void ChangeParamFloat(float *Param)//�����޸�ģʽ�е��޸�float��
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
void ChangeParamDouble(double *Param)//�����޸�ģʽ�е��޸�double��
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
void ChangeParamBool(bool *Param)//�����޸�ģʽ�е��޸�bool��
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

void ChangeParam_aux(int typeval, ...)//���غ���������
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
//  @brief        �޸Ĳ�������Ҫ����Ļ��ʾ�Ĳ�������һ��
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
//  @brief        ��Ļ�رյĻص������������������ã��ڹر������ʱ���Զ�����
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
//  @brief        ��Ļ���庯���������޸�flash
//  @param        void
//  @return       void
//  Sample usage: LcdDisplay(); 
//  Notion        ChangeParam()�ڷ�һ��Ҫ��KEYSTATESelect()����һ��
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