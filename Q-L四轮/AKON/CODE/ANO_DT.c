#include "ANO_DT.h"

uint8 DataToSend[100];        //	���ݻ���λ��С��Ӧ�ô��ڷ������ݵĴ�С, ���ֽڷ���

void ANODT_SendF1(int16 a, int16 b, int16 c, int16 d, int32 e, int32 f)
{
	int16 cnt = 0;										
	
	DataToSend[cnt++] = 0xAA;					//֡ͷ
	DataToSend[cnt++] = 0xFF;         //Ŀ���ַ     0xFFΪ�㲥ģʽ��һ�㲻��Ҫ����
	DataToSend[cnt++] = 0xF1;					//������
	DataToSend[cnt++] = 16;            //���ݳ���     ���ֽ��㣬����int16Ϊ���ֽ�
	
	
	//�������
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
	//���У��
	uint8 sc = 0;                     //��У��
	uint8 ac = 0;											//����У��
	for(uint8 i = 0; i<DataToSend[3] + 4; i++)
	{
		sc += DataToSend[i];
		ac += sc;
	}
	
	DataToSend[cnt++] = sc;
	DataToSend[cnt++] = ac;
	//����������
	
  //����
//	uart_putbuff(USART_1, DataToSend, cnt); //����uart_send 
	seekfree_wireless_send_buff(DataToSend,sizeof(DataToSend)-1);
}

void ANODTSend()
{
    ANODT_SendF1(wheel.right.now, wheel.left.now, motor.right.expectSpeed, motor.left.expectSpeed, motor.left.pwm, motor.right.pwm);  //���ߴ���  
}