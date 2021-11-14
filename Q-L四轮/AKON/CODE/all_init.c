
#include "all_init.h"
extern void DevicesInit();
extern void DebugParamInit();
extern void StateParamInit();
uint8 GlobalInit()
{
    
	LcdInit();
	ips200_showstr(0,0,"Lcd Init");
    ips200_showstr(200,0,"OK");
    ips200_showstr(0,1,"mt9v03x init :");
	mt9v03x_csi_init();	//��ʼ������ͷ ʹ��CSI�ӿ�
	ips200_showstr(200,1,"OK");
	ips200_showstr(0,2,"Devices Init");
	DevicesInit();
	ips200_showstr(200,2,"OK");
    pit_init();                     //��ʼ��pit����
    pit_interrupt_ms(PIT_CH0,1);  //��ʼ��pitͨ��0 ����
    NVIC_SetPriority(PIT_IRQn,10);  //�����ж����ȼ� ��Χ0-15 ԽС���ȼ�Խ�� ��·PIT����һ��PIT�жϺ���
	ADC_init();
	AdcParamInit();
    seekfree_wireless_init();
    DebugParamInit();
    PidParamInit();
	FlashInit();
	StateParamInit();
    BEEP_ON;
    delay_ms(300);
    BEEP_OFF;
    ips200_clear(IPS200_BGCOLOR);
	StartRunAndStopRun();
    TaskInit();
    
    return 1;
}    
