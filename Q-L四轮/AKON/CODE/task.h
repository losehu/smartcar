#ifndef _task_h_
#define _task_h_

#include "headfile.h"

typedef enum
{
	keyTask,
	runControlTask,
    motorTask,
    servoTask,
    lcdTask,
    beepTask,
    gyroscopeTask,
    adcTask,
    debugTask,
    ANODTTask,
	dialTask,
	timeCountTask,
    judgetask,
	circleCountTask,
    taskSum,  //��������
}TaskID;

typedef enum
{
    sleep,  //����
    run,    //����
}TaskStatusEnum;

typedef struct
{
    void (*Function)(void);         //������ָ��
	void (*OpenTaskInterrupt)(void);		//��������ص�����
	void (*CloseTaskInterrupt)(void);	//�ر�����ص����������ر������ʱ��ִ��һ�Ρ�
    TaskStatusEnum status;  //����״̬
    unsigned short period;  //�ܵ��ж�ʱ��Ϊ1ms�����趨���پ��ж�ʱ��Ϊ����
    unsigned short runTime;
    bool isOpen;
}Task;
extern Task task[taskSum];
extern void OpenTask(TaskID ID);
extern void CloseTask(TaskID ID);
extern void ClockInterrupt(void);
extern void TaskInit();
extern void CreatTask(unsigned char ID, void (*Function)(void), void (*OpenInterrupt)(void), void (*CloseInterrupt)(void), unsigned short Period, bool isOpen);
#endif
