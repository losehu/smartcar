#include "task.h"

Task task[taskSum];
//----------------------------------------------------------------
//  @brief        对每个任务进行初始化
//  @param        void
//  @return       void
//  Sample usage: TaskInit(void); 
//----------------------------------------------------------------
void TaskInit()
{
    unsigned char i;
    for(i = 0; i < taskSum; i++)
    {
        task[i].Function = 0;
        task[i].status = sleep;
        task[i].period = 0;
        task[i].runTime = 0;
    }
}
//----------------------------------------------------------------
//  @brief        创建任务
//  @param        ID        任务名称，在TaskID枚举里面，添加任务需要在枚举里面补充，放在最后的taskSum之前
//  @param        function  函数名称，这个任务对应的函数
//  @param        Period    执行周期，定时器是1s，所以这个值是多少就是以多少为周期执行
//  @param        isOpen    用来确定是否执行这个任务，例如按键和屏幕等。
//  @return       void
//  Sample usage: CreatTask(motorTask, Cam_Motor_Control, 2, true); 
//----------------------------------------------------------------
void CreatTask(unsigned char ID, void (*Function)(void), void (*OpenTaskInterrupt)(void), void (*CloseTaskInterrupt)(void), unsigned short Period, bool isOpen)
{
    if(!task[ID].Function)  //判断任务是否被创建
    {
        task[ID].Function = Function;
		task[ID].OpenTaskInterrupt = OpenTaskInterrupt;
		task[ID].CloseTaskInterrupt = CloseTaskInterrupt;
        task[ID].status = sleep;
        task[ID].period = Period;
        task[ID].runTime = 0;
        task[ID].isOpen = isOpen;
    }
}
//----------------------------------------------------------------
//  @brief        开启任务,如果任务是关闭的，现在打开才执行回调函数
//  @param        ID        任务名称，需要关闭的任务名。
//  @return       void
//  Sample usage: OpenTask(lcdTask); 
//----------------------------------------------------------------
void OpenTask(TaskID ID)
{
	if(task[ID].isOpen == false)
	{
		task[ID].isOpen = true;
		//如果存在回调的话再去执行，避免空指针
		if(*(task[ID].OpenTaskInterrupt))
		{
			(*(task[ID].OpenTaskInterrupt))();
		}
		else
		{
		}
	}
}
//----------------------------------------------------------------
//  @brief        关闭任务，任务必须之前是开启的，才执行回调函数
//  @param        ID        任务名称，需要关闭的任务名。
//  @return       void
//  Sample usage: CloseTask(lcdTask); 
//----------------------------------------------------------------
void CloseTask(TaskID ID)
{
	if(task[ID].isOpen == true)
	{
		task[ID].isOpen = false;
		//如果存在回调的话再去执行，避免空指针
		if(*(task[ID].CloseTaskInterrupt))
		{
			(*(task[ID].CloseTaskInterrupt))();
		}
		else
		{
		}
	}
}
//----------------------------------------------------------------
//  @brief        定时器中,确定是run还是sleep
//  @param        void
//  @return       void
//  Sample usage: ClockInterrupt(void); 
//----------------------------------------------------------------
void ClockInterrupt(void)
{
    unsigned char i;
    for(i = 0; i < taskSum ; i++)
    {
        if(task[i].Function && task[i].isOpen)
        {
            task[i].runTime++;
            if(task[i].runTime >= task[i].period)
            {
                task[i].runTime = 0;
                task[i].status = run;
            }
        }
    }
}
