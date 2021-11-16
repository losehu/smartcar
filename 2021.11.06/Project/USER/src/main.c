
#include "headfile.h"
#include "isr.h"
#include "LQ_CAMERA.h"
bool run_show = true;
#define    WAVE_SEND_TO_PC   0
uint8      HMI_START_KEY = 'F';     //���°�����ʼ��ʾHMI ���򲻽�HMI
int extern Radius;//PIDƫ��


uint32 use_time;

	   // systick_start();

 //   CRM123();//����ͷCAME->����CRM
	// use_time = systick_getval_us();
//		    systick_start();


int main(void)
{
    while (!Global_Init());

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
  							 systick_start();
                CameraExtract0();//��ֵ��
                GetBorder();//ͼ����&�ж�&����
                Center_Cal();//�������ڣ�point_center����ƫ��
								 use_time = systick_getval_us();

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
