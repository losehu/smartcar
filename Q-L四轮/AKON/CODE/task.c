#include "task.h"

Task task[taskSum];
//----------------------------------------------------------------
//  @brief        ��ÿ��������г�ʼ��
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
//  @brief        ��������
//  @param        ID        �������ƣ���TaskIDö�����棬���������Ҫ��ö�����油�䣬��������taskSum֮ǰ
//  @param        function  �������ƣ���������Ӧ�ĺ���
//  @param        Period    ִ�����ڣ���ʱ����1s���������ֵ�Ƕ��پ����Զ���Ϊ����ִ��
//  @param        isOpen    ����ȷ���Ƿ�ִ������������簴������Ļ�ȡ�
//  @return       void
//  Sample usage: CreatTask(motorTask, Cam_Motor_Control, 2, true); 
//----------------------------------------------------------------
void CreatTask(unsigned char ID, void (*Function)(void), void (*OpenTaskInterrupt)(void), void (*CloseTaskInterrupt)(void), unsigned short Period, bool isOpen)
{
    if(!task[ID].Function)  //�ж������Ƿ񱻴���
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
//  @brief        ��������,��������ǹرյģ����ڴ򿪲�ִ�лص�����
//  @param        ID        �������ƣ���Ҫ�رյ���������
//  @return       void
//  Sample usage: OpenTask(lcdTask); 
//----------------------------------------------------------------
void OpenTask(TaskID ID)
{
	if(task[ID].isOpen == false)
	{
		task[ID].isOpen = true;
		//������ڻص��Ļ���ȥִ�У������ָ��
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
//  @brief        �ر������������֮ǰ�ǿ����ģ���ִ�лص�����
//  @param        ID        �������ƣ���Ҫ�رյ���������
//  @return       void
//  Sample usage: CloseTask(lcdTask); 
//----------------------------------------------------------------
void CloseTask(TaskID ID)
{
	if(task[ID].isOpen == true)
	{
		task[ID].isOpen = false;
		//������ڻص��Ļ���ȥִ�У������ָ��
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
//  @brief        ��ʱ����,ȷ����run����sleep
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
