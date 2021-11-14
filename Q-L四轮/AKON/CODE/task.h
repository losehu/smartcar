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
    taskSum,  //任务总数
}TaskID;

typedef enum
{
    sleep,  //休眠
    run,    //运行
}TaskStatusEnum;

typedef struct
{
    void (*Function)(void);         //任务函数指针
	void (*OpenTaskInterrupt)(void);		//开启任务回调函数
	void (*CloseTaskInterrupt)(void);	//关闭任务回调函数（当关闭任务的时候执行一次。
    TaskStatusEnum status;  //运行状态
    unsigned short period;  //总的中断时间为1ms，故设定多少就中断时间为多少
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
