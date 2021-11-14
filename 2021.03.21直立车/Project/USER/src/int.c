#include "int.h"


//uint8 System_OK = 0;
uint8 Run_Flag = 0;

float Attitude_Angel_P;        //��ǰpitch
float Target_Angel_P = 300;          //Ŀ��pitch


void System_Init()
{
    /************************ ���� ��ʼ��  *********************************/
    initdata();
    /************************ ��� ��ʼ��  *********************************/
    //	adc_init(ADC_1, ADC1_CH00_A00, ADC_12BIT);//��ʼ��B27ΪADC���� �ֱ���Ϊ8λ
    adc_init(ADC_1, ADC1_CH02_A02, ADC_12BIT);//��ʼ��B27ΪADC���� �ֱ���Ϊ8λ
    adc_init(ADC_1, ADC1_CH03_A03, ADC_12BIT);//��ʼ��B27ΪADC���� �ֱ���Ϊ8λ

    //	adc_init(ADC_1, ADC1_CH11_B10, ADC_12BIT);//��ʼ��B27ΪADC���� �ֱ���Ϊ8λ

    adc_init(ADC_1, ADC1_CH05_A05, ADC_12BIT);//��ʼ��B27ΪADC���� �ֱ���Ϊ8λ
    //	adc_init(ADC_1, ADC1_CH06_A06, ADC_12BIT);//��ʼ��B27ΪADC���� �ֱ���Ϊ8λ ��

    adc_init(ADC_1, ADC1_CH07_A07, ADC_12BIT);//��ʼ��B27ΪADC���� �ֱ���Ϊ8λ
    //	adc_init(ADC_2, ADC2_CH03_B13, ADC_12BIT);//��ʼ��B27ΪADC���� �ֱ���Ϊ8λ

    /************************ PWM��ʼ��************************/
    //	pwm_init(TIM_2, TIM_2_CH3_C09, 17000, 0);
    //	pwm_init(TIM_2, TIM_2_CH4_B11, 17000, 0);
    //	pwm_init(TIM_2, TIM_2_CH1_A11, 17000, 0);
    //	pwm_init(TIM_2, TIM_2_CH2_A12, 17000, 0);
    gpio_init(B11, GPO, 1, GPO_PUSH_PULL);//��
    pwm_init(TIM_2, TIM_2_CH2_A12, 17000, 0);
    pwm_init(TIM_2, TIM_2_CH3_C09, 17000, 0);//��
    gpio_init(A11, GPO, 1, GPO_PUSH_PULL);
    /************************ TFT��Ļ��ʼ�� *****************************/
    //	oled_init();
    lcd_init();
    /************************ ������ ��ʼ��  *********************************/
    Encoder_Init();
    /************************ ���ߴ��ڳ�ʼ�� *****************************/
    seekfree_wireless_init();
    CommunicateInit();
    /************************ GPIO��ʼ�� *****************************/
    gpio_init(A15, GPO, 0, GPO_PUSH_PULL);
    /************************ �����ǳ�ʼ�� *****************************/
    uint32 i, j;
    icm20602_init_spi();
    ICM20602_Offset();

    for (i = 0; i < 200; i++)
    {
        for (j = 0; j < 5; j++)
        {
            get_icm20602_gyro_spi();
            get_icm20602_accdata_spi();
            Data_Filter();					// ��ԭʼ���ݻ�����
        }
        Get_Attitude();	// ��̬����
    }

    /************************ �ж� ��ʼ��  *********************************/
    tim_interrupt_init(TIM_14, 1000, 0);

}


void initdata(void)
{
    /******************����·��*********************/
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

    /******************Բ��*********************/
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

    /******************�µ�*********************/
    rampway.In_flag = 0;
}

