#include "Communicate.h"


//串口初始化在ANO_DT中进行
//发送直立车启动数据
uint8 SendBuff0[3] = {INFO_HEAD,BALANCECAR_START,INFO_TAIL};
void Send_BalanceCar_StartFlag(void)
{
	uart_putbuff(UART_3,&SendBuff0[0],3);
	uart_putbuff(UART_3,&SendBuff0[0],3);
	uart_putbuff(UART_3,&SendBuff0[0],3);
}


//确认直立车启动
uint8 ReceiveBuff0[3];
uint8 ConfirmBalanceCarState(uint8 dat)
{
	uint8 ValidData[3] = {0,0,0};
	static uint8 ndatnum = 0;
	uint8 i = 0,j = 0;
	ReceiveBuff0[ndatnum] = dat;
	ndatnum ++;
	if(ndatnum == 3)
		ndatnum = 0;
	for(i = 0;i < 3;i ++)
	{
		if(ReceiveBuff0[i] == INFO_HEAD)
			break;
		if(i == 2)
			return 0;
	}
	for(j = 0;j < 3;j ++)
	{
		ValidData[j] = ReceiveBuff0[i];
		i ++;
		if(i == 3) 
			i = 0;
	}
	if(ValidData[0] == INFO_HEAD && \
		ValidData[1] == BALANCECAR_START && \
		ValidData[2] == INFO_TAIL )
		MeetCar.BalanceCarStart = 1;
	if(ValidData[0] == INFO_HEAD && \
		ValidData[1] == BALANCECAR_STOP && \
		ValidData[2] == INFO_TAIL )
		MeetCar.BalanceCarStop = 1;
	return 1;
}


uint8 SendBuff1[3] = {INFO_HEAD,BALANCECAR_STOP,INFO_TAIL};
//发送直立车停车数据
void Send_BalanceCar_StopFlag(void)
{
	uart_putbuff(UART_3,&SendBuff1[0],3);
	uart_putbuff(UART_3,&SendBuff1[0],3);
	uart_putbuff(UART_3,&SendBuff1[0],3);
}

/***************************************交接控制******************************************/

MeetCarTypedef MeetCar;

void MeetCar_Init(void)
{
	MeetCar.MeetEnable = 'F';
	MeetCar.MeetCarFlag = 'F';
	MeetCar.BalanceCarStart = 0;
	MeetCar.BalanceCarStop = 0;
	MeetCar.ForkNum = 1;
	MeetCar.ReduceSpeed = 140;
	MeetCar.ReduceSpeedDis = 0.3;
	MeetCar.MeetForwardDis = 0.5;
}



//会车控制
uint8 MeetCar_Control(void)
{
	//三岔计数或者交接控制关闭不进行交接
	if(MeetCar.MeetEnable == 'F')
		return 0;
	if(Fork.ForkNum != MeetCar.ForkNum)
		return 0;
	//交接控制
	if(Wheel.DistanceAll >= (Fork.InDis + MeetCar.ReduceSpeedDis) && \
		Wheel.DistanceAll <= (Fork.InDis + MeetCar.ReduceSpeedDis + 2.0) && \
		Fork.FindFlag == 'T' && Fork.state == ForkIn && MeetCar.MeetCarFlag == 'F')
	{
		ExSpeed = MeetCar.ReduceSpeed;
		MeetCar.MeetCarFlag = 'T';
		CarInfo.ControlMode = ELCMODE;
	}
	//没有车直接换摄像头跑
	if(Wheel.DistanceAll >= (Fork.InDis + MeetCar.ReduceSpeedDis + 2.0) && \
		Fork.FindFlag == 'T' && Fork.state == ForkIn && MeetCar.MeetCarFlag == 'T')
	{
		MeetCar.MeetCarFlag = 'F';
		ExSpeed = Fork.InSpeed;
		CarInfo.ControlMode = CAMMODE;
		CarInfo.ClosedLoopMode =  AllLoop;
	}
	//交接控制
	if(MeetCar.MeetCarFlag  == 'T' && MeetCar.BalanceCarStart == 1)
	{
		CarInfo.ClosedLoopMode =  DirectLoop;
		MeetCar.BalanceCarStart = 2;
	}
	if(MeetCar.MeetCarFlag  == 'T' && MeetCar.BalanceCarStop == 1)
	{
		MeetCar.MeetCarFlag = 'F';
		ExSpeed = Fork.InSpeed;
		CarInfo.ControlMode = CAMMODE;
		CarInfo.ClosedLoopMode =  AllLoop;

		MeetCar.BalanceCarStop = 2;
	}
	return 1;
}




const uint8 MEETItem0[] = "Meet_EN:";
const uint8 MEETItem1[] = "Fork_Num:";
const uint8 MEETItem2[] = "ReduceDis:";
const uint8 MEETItem3[] = "ReduceSpeed:";
const uint8 MEETItem4[] = "ForwardDis";

#define MEETING_MENU_NUM 4
//会车参数设置
uint8 MeetCar_Show()
{
	while(KeyPageUp != onepress)
	{
		static int8 NowMeetItem = 0;
		Menu_Choose(&NowMeetItem,MEETING_MENU_NUM);
		//显示条目
		IPS114_PENCOLOR = RED;
		ips114_showstr(16,0,MEETItem0);
		ips114_showstr(16,1,MEETItem1);
		ips114_showstr(16,2,MEETItem2);
		ips114_showstr(16,3,MEETItem3);
		ips114_showstr(16,4,MEETItem4);
		//显示参数
		IPS114_PENCOLOR = BLUE;
		if(MeetCar.MeetEnable == 'T')
			ips114_showstr(128,0,(const uint8*)("T"));
		else
			ips114_showstr(128,0,(const uint8*)("F"));
		ips114_showint16(120,1,MeetCar.ForkNum);
		ips114_showfloat(120,2,MeetCar.ReduceSpeedDis,3,3);
		ips114_showint16(120,3,MeetCar.ReduceSpeed);
		ips114_showfloat(120,4,MeetCar.MeetForwardDis,3,3);
		//调参确认
		if(KeyPageDown == onepress)
		{
			KeyPageDown = nopress;
			switch(NowMeetItem)
			{
				case(0):
				{
					StateAdjust(&MeetCar.MeetEnable,128,0);
					break;
				}
				case(1):
				{
					ParamsAdjustShort((unsigned short*)(&MeetCar.ForkNum),120,1,1);
					break;
				}
				case(2):
				{
					ParamsAdjustFloat(&MeetCar.ReduceSpeedDis,120,2,0.1);
					break;
				}
				case(3):
				{
					ParamsAdjustShort((unsigned short*)(&MeetCar.ReduceSpeed),120,3,1);
					break;
				}
				case(4):
				{
					ParamsAdjustFloat(&MeetCar.MeetForwardDis,120,4,0.1);
					break;
				}
			}
		}
		delay_ms(10);
	}
	KeyPageUp = nopress;
	//
	return 1; 
}













