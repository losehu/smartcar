
#include "headfile.h"
#include "isr.h"
#include "LQ_CAMERA.h"
bool run_show = true;
#define    WAVE_SEND_TO_PC   0
uint8      HMI_START_KEY = 'F';     //按下按键开始显示HMI 否则不进HMI
int extern Radius;//PID偏差
#include <rthw.h>
#include "rtthread.h"
 int a=0;
/* 定义线程控制块 */
static rt_thread_t led1_thread = RT_NULL;
 
/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void led1_thread_entry(void* parameter);
 
 
int main( void )
{
//  board_init(true);
//	UART3_GPIO_Init();
 //   while (!Global_Init());
	    printf("\n \\ | /\n");

   //  ips114_init();
		unsigned char ss[100]="203";
while(1)
{
	    rt_kprintf("\n \\ | /\n");
	  //    DMA_NVIC_Send_Config(DMA1_Channel2, (u32)&UART3->TDR, (u32)&ss, 3);delay_ms(100);
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

}
 
}
/*
*************************************************************************
*                             线程定义
*************************************************************************
*/
 
static void led1_thread_entry(void* parameter)
{	
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
//                CameraExtract0();//二值化
//                GetBorder();//图像处理&判断&补线
//                Center_Cal();//函数出口：point_center中线偏差
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
 


