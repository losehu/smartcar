#define THREAD_PRIORITY 15
#define THREAD_STACK_SIZE 512
#define THREAD_TIMESLICE 5
#include "headfile.h"
#include "isr.h"
#include "LQ_CAMERA.h"
#include <rtthread.h>
#include <board.h>
#include "rtthread.h"

#include "headfile.h"

// *************************** ����˵�� ***************************
// 
// ������Ҫ׼����ɿƼ� MM32F3277 ���İ�һ��
// 
// ����������Ҫ׼����ɿƼ� CMSIS-DAP ���������� �� ARM ���������� һ��
// 
// ��������ʾʱ��Ƭ������ʽ
// 
// ���µĹ��̻��߹����ƶ���λ�����ִ�����²���
// ��һ�� �ر��������д򿪵��ļ�
// �ڶ��� project->clean  �ȴ��·�����������
// 
// *************************** ����˵�� ***************************

// **************************** �궨�� ****************************
// **************************** �궨�� ****************************

// **************************** �������� ****************************
static rt_uint32_t count = 0;
// **************************** �������� ****************************

// **************************** �������� ****************************
void thread_slice_10_entry (void *parameter);
void thread_slice_20_entry (void *parameter);
int time_slice_20_thread_example (void);
int time_slice_10_thread_example (void);

// ʹ��INIT_APP_EXPORT���Զ���ʼ����Ҳ����ͨ���������߳��ڵ���time_slice_10_thread_example�������г�ʼ��
INIT_APP_EXPORT(time_slice_10_thread_example);					// Ӧ�ó�ʼ��

// ʹ��INIT_APP_EXPORT���Զ���ʼ����Ҳ����ͨ���������߳��ڵ���time_slice_20_thread_example�������г�ʼ��
INIT_APP_EXPORT(time_slice_20_thread_example);					// Ӧ�ó�ʼ��

int main(void)
{
ips114_init();
	while(1)
	{
	ips114_clear(RED);
		rt_thread_mdelay(100);
			ips114_clear(YELLOW);

	
	}
}

//------------------------------------------------------------
// @brief		�߳�1��ں���
// @param		parameter	����
// @return		void
// Sample usage:
//------------------------------------------------------------
void thread_slice_10_entry(void *parameter)
{
	rt_thread_mdelay(10);										// ��ʱ�ȴ�finsh��ʼ�����

	while(1)
	{
		if(0 == (count % 5))
		{
			rt_enter_critical();
			// �����ٽ��� �����ӡ��ʱ�������̴߳��
			rt_kprintf("time slice 10 is running, count = %d\n", count);
			rt_exit_critical();
			if(200 <= count)
			{
				return;
			}
		}
		count++;
	}
}

//------------------------------------------------------------
// @brief		�߳�2��ں���
// @param		parameter	����
// @return		void
// Sample usage:
//------------------------------------------------------------
void thread_slice_20_entry(void *parameter)
{
	rt_thread_mdelay(10);										// ��ʱ�ȴ�finsh��ʼ�����

	while(1)
	{
		if(0 == (count % 5))
		{
			rt_enter_critical();
			// �����ٽ��� �����ӡ��ʱ�������̴߳��
			rt_kprintf("time slice 20 is running, count = %d\n", count);
			rt_exit_critical();
			if(200 <= count)
			{
				return;
			}
		}
		count++;
	}
}

//------------------------------------------------------------
// @brief		��̬�̴߳����Լ�����
// @param		void		��
// @return		void
// Sample usage:
//------------------------------------------------------------
int time_slice_10_thread_example(void)
{
	// �߳̿��ƿ�ָ��
	rt_thread_t tid1;

	// ������̬�߳�
	tid1 = rt_thread_create("thread_10",						// �߳�����
		thread_slice_10_entry,									// �߳���ں���
		RT_NULL,												// �̲߳���
		256,													// 265 ���ֽڵ�ջ�ռ�
		5,														// �߳����ȼ�Ϊ5����ֵԽС�����ȼ�Խ�ߣ�0Ϊ������ȼ���
																// ����ͨ���޸�rt_config.h�е�RT_THREAD_PRIORITY_MAX�궨��(Ĭ��ֵΪ8)���޸����֧�ֵ����ȼ�
		20);													// ʱ��ƬΪ20

	rt_kprintf("create time slice 10 thread...\n\n");
	if(tid1 != RT_NULL)											// �̴߳����ɹ�
	{
		rt_kprintf("time slice 10 thread create OK...\n\n");
		rt_thread_startup(tid1);								// ���и��߳�
	}
	else														// �̴߳���ʧ��
	{
		rt_kprintf("time slice 10 thread create ERROR...\n\n");
		return 1;
	}

	return 0;
}

//------------------------------------------------------------
// @brief		�̴߳����Լ�����
// @param		void
// @return		void
// Sample usage:
//------------------------------------------------------------
int time_slice_20_thread_example(void)
{
	// �߳̿��ƿ�ָ��
	rt_thread_t tid1;
	// ������̬�߳�
	tid1 = rt_thread_create("thread_20",						// �߳�����
		thread_slice_20_entry,									// �߳���ں���
		RT_NULL,												// �̲߳���
		256,													// 256 ���ֽڵ�ջ�ռ�
		5,														// �߳����ȼ�Ϊ5����ֵԽС�����ȼ�Խ�ߣ�0Ϊ������ȼ���
																// ����ͨ���޸�rt_config.h�е�RT_THREAD_PRIORITY_MAX�궨��(Ĭ��ֵΪ8)���޸����֧�ֵ����ȼ�
		10);													// ʱ��Ƭ

	rt_kprintf("create time slice 20 thread...\n\n");
	if(tid1 != RT_NULL)											// �̴߳����ɹ�
	{
		rt_kprintf("time slice 20 thread create OK...\n\n");
		rt_thread_startup(tid1);								// ���и��߳�
	}
	else														// �̴߳���ʧ��
	{
		rt_kprintf("time slice 20 thread create ERROR...\n\n");
		return 1;
	}

	return 0;
}
// **************************** �������� ****************************


//bool run_show = true;
//#define    WAVE_SEND_TO_PC   0
//uint8      HMI_START_KEY = 'F';     //���°�����ʼ��ʾHMI ���򲻽�HMI
//int extern Radius;//PIDƫ��




//int start_car()
//{
////rt_kprintf("start_car!\n");
//    while (1)
//    {
//        if (HMI_START_KEY == 'F')
//        {
//            if (run_show)
//            {
//                ips114_clear(BLACK);
//                run_show = false;
//            }
//            if (mt9v03x_finish_flag == 1 && Barn.state != BarnIn)
//            {
//                CameraExtract0();//��ֵ��
//                GetBorder();//ͼ����&�ж�&����
//                Center_Cal();//�������ڣ�point_center����ƫ��
//                mt9v03x_finish_flag = 0;
//            //    Radius = PlacePID_Control(&Turn_PID, Turn[2], point_center, 0);	// ��̬PID����ת��ǶȻ�
//                  Radius =- rubo_PID(&Turn_PID, rubo_Vel, point_center, 0);	// ��̬PID����ת��ǶȻ�
//               Radius=range_protect(Radius,-180,180);
//            }
//            if (KeyCenter == onepress)
//            {
//                HMI_START_KEY = 'T';
//                ips114_clear(YELLOW);
//                KeyCenter = nopress;
//            }
//        }
//        if (HMI_START_KEY == 'T')
//        {
//					int a=HMI_Display(0);
//            if (a==1)
//            {
//                HMI_START_KEY = 'F';
//                run_show = true;
//            }
//						else if(a==2)return 0;
//        }
////        if (WaveWriteOK == 1)
////        {
////            Waveform_Write();
////            WaveWriteOK = 0;
////        }
//	//rt_thread_delay(1);
//    }
//}
//MSH_CMD_EXPORT(start_car, start_car);
//int main(void)
//{
////start_car();
//		  while (!Global_Init());

////rt_thread_delete(main);
//}
