#include "ANO_DT.h"

uint8 DataToSend[100];        //	数据缓冲位大小，应该大于发送数据的大小, 单字节发送

void ANODT_SendF1(int16 a, int16 b, int16 c, int16 d, int32 e, int32 f)
{
	int16 cnt = 0;										
	
	DataToSend[cnt++] = 0xAA;					//帧头
	DataToSend[cnt++] = 0xFF;         //目标地址     0xFF为广播模式，一般不需要更改
	DataToSend[cnt++] = 0xF1;					//功能码
	DataToSend[cnt++] = 16;            //数据长度     按字节算，例：int16为两字节
	
	
	//填充数据
	DataToSend[cnt++] = BYTE0(a);						
	DataToSend[cnt++] = BYTE1(a);						
//	DataToSend[cnt++] = BYTE2(a);
//	DataToSend[cnt++] = BYTE3(a);
	
	DataToSend[cnt++] = BYTE0(b);
	DataToSend[cnt++] = BYTE1(b);
	
	DataToSend[cnt++] = BYTE0(c);
	DataToSend[cnt++] = BYTE1(c);
	
	DataToSend[cnt++] = BYTE0(d);
	DataToSend[cnt++] = BYTE1(d);
	
	
	DataToSend[cnt++] = BYTE0(e);
	DataToSend[cnt++] = BYTE1(e);
	DataToSend[cnt++] = BYTE2(e);
	DataToSend[cnt++] = BYTE3(e);
    
    DataToSend[cnt++] = BYTE0(f);
	DataToSend[cnt++] = BYTE1(f);
	DataToSend[cnt++] = BYTE2(f);
	DataToSend[cnt++] = BYTE3(f);
//	
	//添加校验
	uint8 sc = 0;                     //和校验
	uint8 ac = 0;											//附加校验
	for(uint8 i = 0; i<DataToSend[3] + 4; i++)
	{
		sc += DataToSend[i];
		ac += sc;
	}
	
	DataToSend[cnt++] = sc;
	DataToSend[cnt++] = ac;
	//数据填充结束
	
  //发送
//	uart_putbuff(USART_1, DataToSend, cnt); //发送uart_send 
	seekfree_wireless_send_buff(DataToSend,sizeof(DataToSend)-1);
}

void ANODTSend()
{
    ANODT_SendF1(wheel.right.now, wheel.left.now, motor.right.expectSpeed, motor.left.expectSpeed, motor.left.pwm, motor.right.pwm);  //无线串口  
}