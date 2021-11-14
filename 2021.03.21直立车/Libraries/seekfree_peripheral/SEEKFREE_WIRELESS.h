/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,��ɿƼ�
* All rights reserved.
* ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897
*
* �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
* ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
*
* @file				SEEKFREE_WIRELESS
* @company			�ɶ���ɿƼ����޹�˾
* @author			��ɿƼ�(QQ3184284598)
* @version			�鿴doc��version�ļ� �汾˵��
* @Software			IAR 8.3 or MDK 5.24
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
* @note
* 					���߶��壺
* 					------------------------------------
* 					ģ��ܽ�				��Ƭ���ܽ�
* 					RX						�鿴SEEKFREE_WIRELESS.h�ļ��е�WIRELESS_UART_TX�궨��
* 					TX						�鿴SEEKFREE_WIRELESS.h�ļ��е�WIRELESS_UART_RX�궨��
* 					RTS						�鿴SEEKFREE_WIRELESS.h�ļ��е�RTS_PIN�궨��
* 					CMD						���ջ�������
* 					------------------------------------
********************************************************************************************************************/

#ifndef _SEEKFREE_WIRELESS
#define _SEEKFREE_WIRELESS

#include "common.h"

#define WIRELESS_UART			UART_1												// ����ת����ģ�� ��ʹ�õ��Ĵ���
#define WIRELESS_UART_TX		UART1_TX_A09
#define WIRELESS_UART_RX		UART1_RX_A10
#define WIRELESS_UART_BAUD		115200

#define RTS_PIN					B12													// ��������λ����  ָʾ��ǰģ���Ƿ���Խ�������  0���Լ�������  1�����Լ�������

#define WIRELESS_BUFFER_SIZE	5

extern uint8 wireless_rx_buffer[WIRELESS_BUFFER_SIZE];
extern uint16 wireless_rx_index;

void		wireless_uart_callback		(void);
void		seekfree_wireless_init		(void);
uint32		seekfree_wireless_send_buff	(uint8 *buff, uint32 len);


//-------------------------user-----------------------------------//
//֡����
#define 	COMMUSIZE  			5
#define 	Head0     			0  //֡ͷ0
#define 	Head1     			1  //֡ͷ1 
#define     CarID               2  //ID
#define 	DATA0        		3  //���־
#define 	SUMVERIFY 			4  //У��
//��ʶ
#define     PedicabID          0x01
#define 	BalanceID          0x02

typedef struct
{
    uint8 InfoState;
    int16 ID;
    int16 ConmmuState;
} CommunicateType;

extern CommunicateType CommuInfo;
extern uint8 CommuTxBuff[COMMUSIZE],
       CommuRxBuff[COMMUSIZE];

void CommunicateInit(void);
uint8 InformationReslove(uint8 *data, CommunicateType *Commu);
uint8 InformationSend(uint8 *data, CommunicateType *Commu);

#endif
