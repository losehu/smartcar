#include "int.h"


//uint8 System_OK = 0;
uint8 Run_Flag = 0;

float Attitude_Angel_P;        //当前pitch
float Target_Angel_P = 300;          //目标pitch


void System_Init()
{
    /************************ 数据 初始化  *********************************/
    initdata();
    /************************ 电感 初始化  *********************************/
    //	adc_init(ADC_1, ADC1_CH00_A00, ADC_12BIT);//初始化B27为ADC功能 分辨率为8位
    adc_init(ADC_1, ADC1_CH02_A02, ADC_12BIT);//初始化B27为ADC功能 分辨率为8位
    adc_init(ADC_1, ADC1_CH03_A03, ADC_12BIT);//初始化B27为ADC功能 分辨率为8位

    //	adc_init(ADC_1, ADC1_CH11_B10, ADC_12BIT);//初始化B27为ADC功能 分辨率为8位

    adc_init(ADC_1, ADC1_CH05_A05, ADC_12BIT);//初始化B27为ADC功能 分辨率为8位
    //	adc_init(ADC_1, ADC1_CH06_A06, ADC_12BIT);//初始化B27为ADC功能 分辨率为8位 、

    adc_init(ADC_1, ADC1_CH07_A07, ADC_12BIT);//初始化B27为ADC功能 分辨率为8位
    //	adc_init(ADC_2, ADC2_CH03_B13, ADC_12BIT);//初始化B27为ADC功能 分辨率为8位

    /************************ PWM初始化************************/
    //	pwm_init(TIM_2, TIM_2_CH3_C09, 17000, 0);
    //	pwm_init(TIM_2, TIM_2_CH4_B11, 17000, 0);
    //	pwm_init(TIM_2, TIM_2_CH1_A11, 17000, 0);
    //	pwm_init(TIM_2, TIM_2_CH2_A12, 17000, 0);
    gpio_init(B11, GPO, 1, GPO_PUSH_PULL);//右
    pwm_init(TIM_2, TIM_2_CH2_A12, 17000, 0);
    pwm_init(TIM_2, TIM_2_CH3_C09, 17000, 0);//左
    gpio_init(A11, GPO, 1, GPO_PUSH_PULL);
    /************************ TFT屏幕初始化 *****************************/
    //	oled_init();
    lcd_init();
    /************************ 编码器 初始化  *********************************/
    Encoder_Init();
    /************************ 无线串口初始化 *****************************/
    seekfree_wireless_init();
    CommunicateInit();
    /************************ GPIO初始化 *****************************/
    gpio_init(A15, GPO, 0, GPO_PUSH_PULL);
    /************************ 陀螺仪初始化 *****************************/
    uint32 i, j;
    icm20602_init_spi();
    ICM20602_Offset();

    for (i = 0; i < 200; i++)
    {
        for (j = 0; j < 5; j++)
        {
            get_icm20602_gyro_spi();
            get_icm20602_accdata_spi();
            Data_Filter();					// 对原始数据滑动滤
        }
        Get_Attitude();	// 姿态解算
    }

    /************************ 中断 初始化  *********************************/
    tim_interrupt_init(TIM_14, 1000, 0);

}


void initdata(void)
{
    /******************三叉路口*********************/
    thressload.time = 1;
    thressload.time_I = 0;
    thressload.In_flag = 0;
    thressload.Out_flag = 1;
    thressload.cnt = 0;
    thressload.In_I = 0;
    thressload.In_turn_I = 0;
    thressload.Out_I = 0;
    thressload.Target_flag = 0;
    thressload.Stop_Flag = 0;
    thressload.Stop_time = 0;

    /******************圆环*********************/
    roundabout.Out_flag = 0;
    roundabout.cnt = 0;
    roundabout.In_flag = 0;
    roundabout.In_dis_flag = 0;
    roundabout.In_dis_I = 0;
    roundabout.Speed = 0;
    roundabout.In_process = 0;
    roundabout.In_I = 0;
    roundabout.In_turn_I = 0;
    roundabout.Out_I = 0;
    roundabout.frequency = 1;

    /******************坡道*********************/
    rampway.In_flag = 0;
}

