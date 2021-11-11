
#include "headfile.h"
#include "isr.h"
#include "LQ_CAMERA.h"
#include <rtthread.h>
#include <board.h>
#include "rtthread.h"
bool run_show = true;
#define    WAVE_SEND_TO_PC   0
uint8      HMI_START_KEY = 'F';     //���°�����ʼ��ʾHMI ���򲻽�HMI
int extern Radius;//PIDƫ��




int start_car()
{
//rt_kprintf("start_car!\n");
    while (1)
    {
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
					int a=HMI_Display(0);
            if (a==1)
            {
                HMI_START_KEY = 'F';
                run_show = true;
            }
						else if(a==2)return 0;
        }
//        if (WaveWriteOK == 1)
//        {
//            Waveform_Write();
//            WaveWriteOK = 0;
//        }
	//rt_thread_delay(1);
    }
}
MSH_CMD_EXPORT(start_car, start_car);
int main(void)
{
//start_car();
		  while (!Global_Init());

//rt_thread_delete(main);
}
