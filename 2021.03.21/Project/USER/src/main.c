#include "headfile.h"

// **************************** �궨�� ****************************
// **************************** �궨�� ****************************

// **************************** �������� ****************************

// **************************** �������� ****************************

// **************************** �������� ****************************
int main(void)
{
    board_init(false);																// ��ʼ�� debug �������
    System_Init();
    In_Roundabout_Flag3 = 1;


    systick_delay_ms(500);
    gpio_set(A15, 1);
    systick_delay_ms(500);
    gpio_set(A15, 0);
    while(1)
    {


        if(thressload.Stop_Flag)
        {
            systick_delay_ms(100);
            thressload.Stop_time++;
        }
        //	Communication();
        if( (Tracking_Sensor_data_1_LCD > 120) && (thressload.Stop_time < 20) ) //&& Communication_G_Flag )
        {
            Run_Flag = 1;
        }
        else
        {
            Run_Flag = 0;
        }
        Tracking_Sensor_Detect();
        Radius = PlacePID_Control(&Turn_PID, Turn[Fres], point_center, 0);	// ��̬PID����ת�� Difference
        //		Waveform_Write();
    }
}
// **************************** �������� ****************************
