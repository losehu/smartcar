
#include "headfile.h"
#include "isr.h"
#include "LQ_CAMERA.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"        //��Ϣ����
#include "semphr.h"       //�ź����������ź���
#include "event_groups.h" //�¼�
#include "timers.h"       //�����ʱ��
bool run_show = true;
#define    WAVE_SEND_TO_PC   0
uint8      HMI_START_KEY = 'F';     //���°�����ʼ��ʾHMI ���򲻽�HMI
int extern Radius;//PIDƫ��

//�������--------------------------
//���ȼ� ��ջ��С ������ ������
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

    //����ϵͳ�ж����ȼ�����4(FreeRTOS�е�Ĭ�Ϸ�ʽ��)

    //��ʼ��LED�˿�

    //������ʼ����

    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������
    //�����������
    vTaskStartScheduler();
}



//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���

    //����TASK1����
    xTaskCreate((TaskFunction_t )task1_task,
                (const char*    )"task1_task",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_TASK_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    //����TASK2����
    xTaskCreate((TaskFunction_t )task2_task,
                (const char*    )"task2_task",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
								
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//task1������
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
                CameraExtract0();//��ֵ��
                GetBorder();//ͼ����&�ж�&����
                Center_Cal();//�������ڣ�point_center����ƫ��
                mt9v03x_finish_flag = 0;
                //    Radius = PlacePID_Control(&Turn_PID, Turn[2], point_center, 0);	// ��̬PID����ת��ǶȻ�
                Radius =- rubo_PID(&Turn_PID, rubo_Vel, point_center, 0);	// ��̬PID����ת��ǶȻ�
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
