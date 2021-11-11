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

// *************************** 例程说明 ***************************
// 
// 测试需要准备逐飞科技 MM32F3277 核心板一块
// 
// 调试下载需要准备逐飞科技 CMSIS-DAP 调试下载器 或 ARM 调试下载器 一个
// 
// 本例程演示时间片工作方式
// 
// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project->clean  等待下方进度条走完
// 
// *************************** 例程说明 ***************************

// **************************** 宏定义 ****************************
// **************************** 宏定义 ****************************

// **************************** 变量定义 ****************************
static rt_uint32_t count = 0;
// **************************** 变量定义 ****************************

// **************************** 代码区域 ****************************
void thread_slice_10_entry (void *parameter);
void thread_slice_20_entry (void *parameter);
int time_slice_20_thread_example (void);
int time_slice_10_thread_example (void);

// 使用INIT_APP_EXPORT宏自动初始化，也可以通过在其他线程内调用time_slice_10_thread_example函数进行初始化
INIT_APP_EXPORT(time_slice_10_thread_example);					// 应用初始化

// 使用INIT_APP_EXPORT宏自动初始化，也可以通过在其他线程内调用time_slice_20_thread_example函数进行初始化
INIT_APP_EXPORT(time_slice_20_thread_example);					// 应用初始化

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
// @brief		线程1入口函数
// @param		parameter	参数
// @return		void
// Sample usage:
//------------------------------------------------------------
void thread_slice_10_entry(void *parameter)
{
	rt_thread_mdelay(10);										// 延时等待finsh初始化完毕

	while(1)
	{
		if(0 == (count % 5))
		{
			rt_enter_critical();
			// 进入临界区 避免打印的时候被其他线程打断
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
// @brief		线程2入口函数
// @param		parameter	参数
// @return		void
// Sample usage:
//------------------------------------------------------------
void thread_slice_20_entry(void *parameter)
{
	rt_thread_mdelay(10);										// 延时等待finsh初始化完毕

	while(1)
	{
		if(0 == (count % 5))
		{
			rt_enter_critical();
			// 进入临界区 避免打印的时候被其他线程打断
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
// @brief		动态线程创建以及启动
// @param		void		空
// @return		void
// Sample usage:
//------------------------------------------------------------
int time_slice_10_thread_example(void)
{
	// 线程控制块指针
	rt_thread_t tid1;

	// 创建动态线程
	tid1 = rt_thread_create("thread_10",						// 线程名称
		thread_slice_10_entry,									// 线程入口函数
		RT_NULL,												// 线程参数
		256,													// 265 个字节的栈空间
		5,														// 线程优先级为5，数值越小，优先级越高，0为最高优先级。
																// 可以通过修改rt_config.h中的RT_THREAD_PRIORITY_MAX宏定义(默认值为8)来修改最大支持的优先级
		20);													// 时间片为20

	rt_kprintf("create time slice 10 thread...\n\n");
	if(tid1 != RT_NULL)											// 线程创建成功
	{
		rt_kprintf("time slice 10 thread create OK...\n\n");
		rt_thread_startup(tid1);								// 运行该线程
	}
	else														// 线程创建失败
	{
		rt_kprintf("time slice 10 thread create ERROR...\n\n");
		return 1;
	}

	return 0;
}

//------------------------------------------------------------
// @brief		线程创建以及启动
// @param		void
// @return		void
// Sample usage:
//------------------------------------------------------------
int time_slice_20_thread_example(void)
{
	// 线程控制块指针
	rt_thread_t tid1;
	// 创建动态线程
	tid1 = rt_thread_create("thread_20",						// 线程名称
		thread_slice_20_entry,									// 线程入口函数
		RT_NULL,												// 线程参数
		256,													// 256 个字节的栈空间
		5,														// 线程优先级为5，数值越小，优先级越高，0为最高优先级。
																// 可以通过修改rt_config.h中的RT_THREAD_PRIORITY_MAX宏定义(默认值为8)来修改最大支持的优先级
		10);													// 时间片

	rt_kprintf("create time slice 20 thread...\n\n");
	if(tid1 != RT_NULL)											// 线程创建成功
	{
		rt_kprintf("time slice 20 thread create OK...\n\n");
		rt_thread_startup(tid1);								// 运行该线程
	}
	else														// 线程创建失败
	{
		rt_kprintf("time slice 20 thread create ERROR...\n\n");
		return 1;
	}

	return 0;
}
// **************************** 代码区域 ****************************


//bool run_show = true;
//#define    WAVE_SEND_TO_PC   0
//uint8      HMI_START_KEY = 'F';     //按下按键开始显示HMI 否则不进HMI
//int extern Radius;//PID偏差




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
//                CameraExtract0();//二值化
//                GetBorder();//图像处理&判断&补线
//                Center_Cal();//函数出口：point_center中线偏差
//                mt9v03x_finish_flag = 0;
//            //    Radius = PlacePID_Control(&Turn_PID, Turn[2], point_center, 0);	// 动态PID控制转向角度环
//                  Radius =- rubo_PID(&Turn_PID, rubo_Vel, point_center, 0);	// 动态PID控制转向角度环
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
