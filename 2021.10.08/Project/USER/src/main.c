/**
 * @Editor      : 赵延国
 * @Date        : 2021-05-12 08:44:04
 * @Version     : At MDK5.32
 * @LastEditTime: 2021-05-12 08:57:44
 * @Description : 主函数
 * @Attention   : 没有main.h
 */




#include "headfile.h"
#include "isr.h"
bool run_show = true;
#define    WAVE_SEND_TO_PC   0
uint8      HMI_START_KEY = 'F';     //按下按键开始显示HMI 否则不进HMI

int main(void)
{
    board_init(true);// 初始化 debug 输出串口
    //外部设备初始化
    while (!Global_Init());
    while (1)
    {
        if (HMI_START_KEY == 'F')
        {
            if (run_show)
            {
                IPS114_BGCOLOR = BLACK;
                ips114_clear(BLACK);
                ips114_showstr(16, 0, "runing...");
                IPS114_BGCOLOR = YELLOW;

                run_show = false;

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
        else
        {
            if (mt9v03x_finish_flag == 1 && Barn.state != BarnIn)
            {
                //电磁
                //   Inductor_Read();
                //    ELC_Normalized();
                //   ELC_Judge();
                // ELC_Center_Calculate();
                CameraExtract0();
                GetBorder();
                ShowExBorder();
                Center_Cal();
                mt9v03x_finish_flag = 0;
                //IMAGETOFLASH;
            }
        }
        if (WaveWriteOK == 1 && CarInfo.Protect == 'F' && MeetCar.MeetEnable == 'F')
        {
            Waveform_Write();
            WaveWriteOK = 0;
        }
        //////////        MeetCar_Control();
        //////////        Car_Protect();

        //      if(CarInfo.UpTime >= 5.0f){CarInfo.Protect = 'T';}
        //
        //
        //      if(Fork.FindFlag == 'T' && Fork.state == ForkIn)
        //      {
        //          ExSpeed = ForkFindOtherCarSpeed;
        //          CarInfo.ControlMode = ELCMODE;
        //      }
        //      else if(Fork.FindFlag == 'T' && Fork.state == ForkOutTurn)
        //      {
        //          ExSpeed = NormalSpeed;
        //          CarInfo.ControlMode = CAMMODE;
        //      }
        //      if(BalanceCarStart == 1)
        //      {
        //          //CarInfo.Protect = 'T';
        //          CarInfo.ClosedLoopMode =  DirectLoop;
        //          BalanceCarStart = 2;
        //      }
        //      if(BalanceCarStop == 1)
        //      {
        //          BalanceCarStart = 3;
        //          ExSpeed = NormalSpeed;
        //          CarInfo.ControlMode = CAMMODE;
        //          CarInfo.ClosedLoopMode =  AllLoop;
        //          ExSpeed = NormalSpeed;
        //          BalanceCarStop = 0;
        //      }
    }
}
