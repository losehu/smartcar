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
extern long long cnt;
bool run_show = true;
#define    WAVE_SEND_TO_PC   0
uint8      HMI_START_KEY = 'F';     //按下按键开始显示HMI 否则不进HMI
int extern Radius;//PID偏差

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
int main(void)
{
		  while (!Global_Init());

while(1)
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
				rt_thread_mdelay(1);
}

}
//MSH_CMD_EXPORT(setting_pid, start_car); //设置PID
