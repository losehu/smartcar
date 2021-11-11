//所有设备以及参数初始化
#include "All_Init.h"

//delay_us
void delay_us(unsigned short us)
{
    systick_delay_us(us);
}


//delay_ms
void delay_ms(unsigned short ms)
{
    for (; ms > 0; ms--)
    {
        delay_us(1000);
    }
}

//整型限幅保护
short range_protect(short x, short x_min, short x_max)
{
    if (x >= x_max) return x_max;
    else if (x <= x_min) return x_min;
    //none
    return x;
}

//浮点数限幅保护
float range_protect_float(float x, float x_min, float x_max)
{
    if (x >= x_max) return x_max;
    else if (x <= x_min) return x_min;
    //none
    return x;
}

//整型绝对值
int ABS(int data)
{
    if (data <= 0)
        data = -data;

    return data;
}

//浮点型绝对值
float ABS_FLOAT(float data)
{
    if (data <= 0.00000f)
        data = -data;

    return data;
}

//all init
uint8 Global_Init(void)
{
    int16 i = 0;
    //device
    systick_start();
    ips114_init();
    ips114_showstr(0, 0, "ips114_init :");
    ips114_showstr(200, 0, "OK");
    //  ips114_showstr(0,1,"ov7725_init :");
    //  /////////////////////////////////////////////ov7725_init();
    ips114_showstr(0, 1, "mt9v03x_init:");
    mt9v03x_init();
    ips114_showstr(200, 1, "OK");
    //param
    ips114_showstr(0, 2, "param_init :");
    ImageProcessingStatusInit();
    ELC_Status_Init();
    PID_Init();
    CarSystem_Init();
    ips114_showstr(200, 2, "OK");
    ips114_showstr(0, 3, "devices_init :");
    Decices_Init();
    ANO_DT_Init();
    ips114_showstr(200, 3, "OK");
    ips114_showstr(0, 4, "icm20602_init :");
    icm20602_init_spi();
    while (ICM20602_Offset() == false);
    ips114_showstr(200, 4, "OK");
    ips114_showstr(0, 5, "read flash params:");
    FlashParamsRead();
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_CNST, ov7725_threshold);//阈值
    ips114_showstr(200, 5, "OK");
    ips114_clear(YELLOW);
    delay_ms(200);
    RGBLED(0);
    i = 10;
    while (i)
    {
        if (mt9v03x_finish_flag == 1)
        {
            //电磁
            Inductor_Read();
            ELC_Normalized();
            ELC_Judge();
            ELC_Center_Calculate();
            CameraExtract0();
            ips114_displayimage032(&Bin_Image[0][0], 128, 60);
            Center_Cal();
            GetBorder();
            mt9v03x_finish_flag = 0;
            i --;
        }
    }
    Base.element_check = 'T';//元素判断 'T':开 'F':关
    point_center = 0;
    ips114_clear(YELLOW);
    IPS114_PENCOLOR = RED;
    IPS114_BGCOLOR = YELLOW;
    //按键S1发车
    ips114_showstr(0, 0, "Press Button S1 To Start Car!");
    //////////////////////////////////////////  ///////////////////////////BEEP_ON;
    delay_ms(300);
    ////////////////////////////////////////////    BEEP_OFF;
    //  while(gpio_get(PAGE_UP_PIN));
    ips114_clear(BLACK);
    //开始调度
    tim_interrupt_init_us(TIM_6, 1000, 0x02, 0x03);

    return 1;
}


