#ifndef _MEETING_H_
#define _MEETING_H_


#include "headfile.h"

/*************** ͨ�� *************/


#define  INFO_HEAD  				'D'
#define  INFO_TAIL  				'L'

#define  BALANCECAR_START		 	0x01
#define  BALANCECAR_STOP			0x02


void  Send_BalanceCar_StartFlag(void);
uint8 ConfirmBalanceCarState(uint8 dat);
void  Send_BalanceCar_StopFlag(void);


/*************** �ᳵ���� *************/

typedef struct
{
    uint8 MeetEnable;
    uint8 MeetCarFlag;
    short ForkNum;//�ڼ�������ᳵ
    float ReduceSpeedDis;//���پ���
    short ReduceSpeed;
    float MeetForwardDis;
    uint8 BalanceCarStart;
    uint8 BalanceCarStop;
} MeetCarTypedef;

extern MeetCarTypedef MeetCar;

void MeetCar_Init(void);
uint8 MeetCar_Control(void);
uint8 MeetCar_Show(void);

#endif
