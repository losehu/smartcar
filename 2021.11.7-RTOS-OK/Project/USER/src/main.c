
#include "headfile.h"
#include "isr.h"
#include "LQ_CAMERA.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"        //消息队列
#include "semphr.h"       //信号量、互斥信号量
#include "event_groups.h" //事件
#include "timers.h"       //软件定时器
bool run_show = true;
#define    WAVE_SEND_TO_PC   0
uint8      HMI_START_KEY = 'F';     //按下按键开始显示HMI 否则不进HMI
int extern Radius;//PID偏差

//任务参数--------------------------
//优先级 堆栈大小 任务句柄 任务函数
#define START_TASK_PRIO		1
#define START_STK_SIZE 		128
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);
int a=0;int b=0;
#define TASK1_TASK_PRIO		3
#define TASK1_STK_SIZE 		128
TaskHandle_t Task1Task_Handler;
void task1_task(void *pvParameters);
#define TASK2_TASK_PRIO		2
#define TASK2_STK_SIZE 		128
TaskHandle_t Task2Task_Handler;
void task2_task(void *pvParameters);

int main(void)
{
    board_init(true);
    Global_Init();

    //设置系统中断优先级分组4(FreeRTOS中的默认方式！)

    //初始化LED端口

    //创建开始任务

    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄
    //开启任务调度
    vTaskStartScheduler();
}



//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区

    //创建TASK1任务
    xTaskCreate((TaskFunction_t )task1_task,
                (const char*    )"task1_task",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_TASK_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )task2_task,
                (const char*    )"task2_task",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
								
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//task1任务函数
void task1_task(void *pvParameters)
{

    while(1)
    {
			vTaskDelay(5);
              ips114_clear(GREEN);
    }
}
void task2_task(void *pvParameters)
{
    while(1)
    {
//			delay_ms(5);
//			ips114_clear(YELLOW);

        if (HMI_START_KEY == 'F')
        {
            if (run_show)
            {
                ips114_clear(BLACK);
                run_show = false;
            }
            if (mt9v03x_finish_flag == 1 && Barn.state != BarnIn)
            {
                CameraExtract0();//二值化
                GetBorder();//图像处理&判断&补线
                Center_Cal();//函数出口：point_center中线偏差
                mt9v03x_finish_flag = 0;
                //    Radius = PlacePID_Control(&Turn_PID, Turn[2], point_center, 0);	// 动态PID控制转向角度环
                Radius =- rubo_PID(&Turn_PID, rubo_Vel, point_center, 0);	// 动态PID控制转向角度环
                Radius=range_protect(Radius,-180,180);
            }

            if (KeyCenter == onepress)
            {
                HMI_START_KEY = 'T';
                ips114_clear(YELLOW);
                KeyCenter = nopress;
            }
        }
        if (HMI_START_KEY == 'T')
        {

            if (HMI_Display(0))
            {
                HMI_START_KEY = 'F';
                run_show = true;
            }

        }
        if (WaveWriteOK == 1)
        {
            Waveform_Write();
            WaveWriteOK = 0;
        }
    }
}




//int main(void)
//{
//    board_init(true);
//    while (!Global_Init());
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
//            if (HMI_Display(0))
//            {
//                HMI_START_KEY = 'F';
//                run_show = true;
//            }
//        }
//        if (WaveWriteOK == 1)
//        {
//            Waveform_Write();
//            WaveWriteOK = 0;
//        }
//    }
//}
