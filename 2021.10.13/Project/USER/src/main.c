
#include "headfile.h"
#include "isr.h"
#include "LQ_CAMERA.h"
bool run_show = true;
#define    WAVE_SEND_TO_PC   0
uint8      HMI_START_KEY = 'F';     //按下按键开始显示HMI 否则不进HMI

int main(void)
{
    board_init(true);
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
                //电磁
                //  不需要 Inductor_Read();
                //  不需要     ELC_Normalized();
                //  不需要     ELC_Judge();
                //  不需要      ELC_Center_Calculate();
                CameraExtract0();
                //      ips114_displayimage032(&Bin_Image[0][0],128,60 );
                GetBorder();
                //       ShowExBorder();
                //                ShowLeftAndRightBorder0();
                Center_Cal();
                mt9v03x_finish_flag = 0;
                //IMAGETOFLASH;
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
        if (WaveWriteOK == 1 && CarInfo.Protect == 'F' && MeetCar.MeetEnable == 'F')
        {
            Waveform_Write();
            WaveWriteOK = 0;
        }

    }
}
