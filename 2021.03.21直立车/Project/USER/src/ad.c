#include "AD.h"
float Tracking_Sensor_data_1_LCD = 0;                     //��������ʾ ����۲�
float Tracking_Sensor_data_2_LCD = 0;
float Tracking_Sensor_data_3_LCD = 0;
float Tracking_Sensor_data_4_LCD = 0;
float Tracking_Sensor_data_5_LCD = 0;

float Tracking_ADC_Val_Sum = 0;                         //�ڲ����ҵ�к�
float Tracking_ADC_Val_Sub = 0;                         //�ڲ����ҵ�в�
float point_center = 0;
float last_point_center = 0 ;

float Tracking_Sensor_data_1_LCD_last = 0;
float Tracking_Sensor_data_2_LCD_last = 0;
float Tracking_Sensor_data_3_LCD_last = 0;
float Tracking_Sensor_data_4_LCD_last = 0;
float Tracking_Sensor_data_5_LCD_last = 0;

Thressloadtypedef thressload;
Roundabouttypedef roundabout;
Rampwaytypedef rampway;

/*====================================================================================================*/
/*====================================================================================================*
** ��    �� : LPF_1st
** ��    �� : һ�׵�ͨ�˲�
** ��    �� :
** ݔ    �� : None
** ��    ע : None
** ���÷�ʽ : LPF_1st(oldData, newData, 0.386f);
**====================================================================================================*/
/*====================================================================================================*/
float LPF_1st(float oldData, float newData, float lpf_factor)
{
    lpf_factor = 0.8;
    return oldData * (1 - lpf_factor) + newData * lpf_factor;
}

/***********************************************************
�������ƣ�getNormalizedADValue()
�������ܣ���������ѹ��һ������
��ڲ�����ADinput:                            ��������ֵ
          Min_val��Max_val:                   ����������С�����ֵ
          Normalize_MIN��Normalize_MAX:       ��һ�����С�����ֵ
���ڲ�����
�� ע��
***********************************************************/
float getNormalizedADValue(float ADinput, float Min_val, float Max_val, float Normalize_MIN, float Normalize_MAX)
{
    float tmp;

    tmp = ADinput;
    tmp = tmp < Min_val ? Min_val : tmp; //��С��ȡ
    //tmp = tmp>Max_val?Max_val:tmp; //����ȡ
    tmp = (Normalize_MAX - Normalize_MIN) * ((tmp - Min_val) / (Max_val - Min_val)) + Normalize_MIN; //��һֵ
    return tmp;
}



void  Tracking_Sensor_Detect(void)
{
    uint8 i, j;
#define FILTER_N 5
    float filter_temp, filter_sum_L = 0, filter_sum_LC = 0, filter_sum_M = 0, filter_sum_R = 0, filter_sum_RC = 0;
    float filter_buf_L[FILTER_N] = {0};
    float filter_buf_LC[FILTER_N] = {0};
    float filter_buf_M[FILTER_N] = {0};
    float filter_buf_R[FILTER_N] = {0};
    float filter_buf_RC[FILTER_N] = {0};
    for(i = 0; i < FILTER_N; i++)
    {
        //		filter_buf_L[i]  = adc_convert(ADC_1,ADC1_CH00_A00);
        //		filter_buf_LC[i] = adc_convert(ADC_1,ADC1_CH03_A03);
        //		filter_buf_R[i]  = adc_convert(ADC_1,ADC1_CH02_A02);
        //		filter_buf_RC[i] = adc_convert(ADC_1,ADC1_CH11_B10);
        //		filter_buf_M[i]  = adc_convert(ADC_1,ADC1_CH02_A02);
        //	adc_init(ADC_1, ADC1_CH00_A00, ADC_12BIT);//��ʼ��B27ΪADC���� �ֱ���Ϊ8λ

        filter_buf_L[i]  = adc_convert(ADC_1, ADC1_CH07_A07);
        filter_buf_LC[i] = adc_convert(ADC_1, ADC1_CH05_A05);
        filter_buf_M[i]  = adc_convert(ADC_1, ADC1_CH03_A03);
        //		filter_buf_RC[i] = adc_convert(ADC_1, ADC1_CH06_A06);
        filter_buf_R[i]  = adc_convert(ADC_1, ADC1_CH02_A02);
    }

    // ����ֵ��С�������У�ð�ݷ���
    for(j = 0; j < FILTER_N - 1; j++)
    {
        for(i = 0; i < FILTER_N - 1 - j; i++)
        {
            if(filter_buf_L[i] > filter_buf_L[i + 1])
            {
                filter_temp = filter_buf_L[i];
                filter_buf_L[i] = filter_buf_L[i + 1];
                filter_buf_L[i + 1] = filter_temp;
            }
        }
    }
    for(j = 0; j < FILTER_N - 1; j++)
    {
        for(i = 0; i < FILTER_N - 1 - j; i++)
        {
            if(filter_buf_LC[i] > filter_buf_LC[i + 1])
            {
                filter_temp = filter_buf_LC[i];
                filter_buf_LC[i] = filter_buf_LC[i + 1];
                filter_buf_LC[i + 1] = filter_temp;
            }
        }
    }
    for(j = 0; j < FILTER_N - 1; j++)
    {
        for(i = 0; i < FILTER_N - 1 - j; i++)
        {
            if(filter_buf_R[i] > filter_buf_R[i + 1])
            {
                filter_temp = filter_buf_R[i];
                filter_buf_R[i] = filter_buf_R[i + 1];
                filter_buf_R[i + 1] = filter_temp;
            }
        }
    }
    for(j = 0; j < FILTER_N - 1; j++)
    {
        for(i = 0; i < FILTER_N - 1 - j; i++)
        {
            if(filter_buf_RC[i] > filter_buf_RC[i + 1])
            {
                filter_temp = filter_buf_RC[i];
                filter_buf_RC[i] = filter_buf_RC[i + 1];
                filter_buf_RC[i + 1] = filter_temp;
            }
        }
    }
    for(j = 0; j < FILTER_N - 1; j++)
    {
        for(i = 0; i < FILTER_N - 1 - j; i++)
        {
            if(filter_buf_M[i] > filter_buf_M[i + 1])
            {
                filter_temp = filter_buf_M[i];
                filter_buf_M[i] = filter_buf_M[i + 1];
                filter_buf_M[i + 1] = filter_temp;
            }
        }
    }
    // ȥ�������С��ֵ����ƽ��
    for(i = 1; i < FILTER_N - 1; i++)
    {
        filter_sum_L += filter_buf_L[i];
        filter_sum_LC += filter_buf_LC[i];
        filter_sum_R += filter_buf_R[i];
        filter_sum_RC += filter_buf_RC[i];
        filter_sum_M += filter_buf_M[i];
    }
    Tracking_Sensor_data_1_LCD = filter_sum_L / (FILTER_N - 2);
    Tracking_Sensor_data_2_LCD = filter_sum_LC / (FILTER_N - 2);
    Tracking_Sensor_data_3_LCD = filter_sum_M / (FILTER_N - 2);
    Tracking_Sensor_data_4_LCD = filter_sum_RC / (FILTER_N - 2);
    Tracking_Sensor_data_5_LCD = filter_sum_R / (FILTER_N - 2) + 100;

    //    Tracking_Sensor_data_1_LCD =  (int32)LPF_1st(Tracking_Sensor_data_1_LCD_last,Tracking_Sensor_data_1_LCD,0.386f) ;   //һ�׵�ͨ�˲�
    //    Tracking_Sensor_data_2_LCD =  (int32)LPF_1st(Tracking_Sensor_data_2_LCD_last,Tracking_Sensor_data_2_LCD,0.386f) ;
    //    Tracking_Sensor_data_3_LCD =  (int32)LPF_1st(Tracking_Sensor_data_3_LCD_last,Tracking_Sensor_data_3_LCD,0.386f) ;   //һ�׵�ͨ�˲�
    //    Tracking_Sensor_data_4_LCD =  (int32)LPF_1st(Tracking_Sensor_data_4_LCD_last,Tracking_Sensor_data_4_LCD,0.386f) ;
    //    Tracking_Sensor_data_5_LCD =  (int32)LPF_1st(Tracking_Sensor_data_5_LCD_last,Tracking_Sensor_data_5_LCD,0.386f) ;
    //
    //  Tracking_Sensor_data_1_LCD_last= Tracking_Sensor_data_1_LCD;
    //  Tracking_Sensor_data_2_LCD_last= Tracking_Sensor_data_2_LCD;
    //  Tracking_Sensor_data_3_LCD_last= Tracking_Sensor_data_3_LCD;
    //  Tracking_Sensor_data_4_LCD_last= Tracking_Sensor_data_4_LCD;
    //  Tracking_Sensor_data_5_LCD_last= Tracking_Sensor_data_5_LCD;

    //*------------------�ɼ���ɺ����ƫ�����-------------------------*//
    if( (!roundabout.In_process) && (!roundabout.Out_flag) && (!thressload.In_flag) )//�������Ĺ����С���������������·�ڲ���
    {
        if(Tracking_Sensor_data_1_LCD > 0 || Tracking_Sensor_data_2_LCD > 0)
        {
            Tracking_ADC_Val_Sum = Tracking_Sensor_data_2_LCD + Tracking_Sensor_data_1_LCD;
            Tracking_ADC_Val_Sub = Tracking_Sensor_data_2_LCD - Tracking_Sensor_data_1_LCD;
            //			  point_center=110*(Tracking_ADC_Val_Sub/Tracking_ADC_Val_Sum); //��ͱȵõ�ƫ��
        }
        else
        {
            point_center = last_point_center;
        }
        if(Attitude_Angel_P > -5)
        {
            point_center = 10 * (Tracking_ADC_Val_Sub / Tracking_ADC_Val_Sum); //��ͱȵõ�ƫ��
        }
        if(Attitude_Angel_P > -15)
        {
            point_center = 80 * (Tracking_ADC_Val_Sub / Tracking_ADC_Val_Sum); //��ͱȵõ�ƫ��
        }
        else
        {
            point_center = 100 * (Tracking_ADC_Val_Sub / Tracking_ADC_Val_Sum); //��ͱȵõ�ƫ��
        }
    }
    //    if(Tracking_Sensor_data_3_LCD > 0 || Tracking_Sensor_data_4_LCD > 0)
    //    {
    //          Tracking_ADC_Val_Sum = Tracking_Sensor_data_3_LCD + Tracking_Sensor_data_4_LCD;
    //          Tracking_ADC_Val_Sub = Tracking_Sensor_data_3_LCD - Tracking_Sensor_data_4_LCD;
    //          point_center = 180*(3850*Tracking_ADC_Val_Sub/(Tracking_Sensor_data_1_LCD*(Tracking_ADC_Val_Sum + Tracking_Sensor_data_1_LCD))); //��ͱȵõ�ƫ��
    //    }
    //    else
    //    {
    //          point_center = last_point_center;
    //    }


}

/******************����***************************/
void Huandao(void)
{
    if( (Tracking_Sensor_data_1_LCD > 2700) && (Tracking_Sensor_data_2_LCD > 2700) && \
            (Tracking_Sensor_data_3_LCD > 600) && \
            (Tracking_Sensor_data_5_LCD > 600) && (!roundabout.Out_flag) && (Attitude_Angel_P < -10) ) //&& \
        ( ( (Tracking_Sensor_data_2_LCD - Tracking_Sensor_data_4_LCD) > 750 ) || \
          ( (Tracking_Sensor_data_2_LCD - Tracking_Sensor_data_4_LCD) < -750 ) ) )//������
    {

        roundabout.cnt++;

    }
    if(roundabout.cnt > 3)
{
    roundabout.In_flag = 1;
    roundabout.In_dis_flag = 1;

    roundabout.Speed = 1;
}
if(roundabout.In_dis_I > 300)
{
    roundabout.In_process = 1;
    gpio_set(A15, 1);
    }
    //	else if( (In_Out_Roundabout_I2 > 10) && (In_Out_Roundabout_I2 < 500) )
    //	{
    //		gpio_set(A15,1);
    //	}
    if( (roundabout.In_turn_I < 40) && (roundabout.In_turn_I > -40) && (roundabout.In_process) && \
                (roundabout.In_flag) )//&& (roundabout.gyro_I < 1) )
    {

        roundabout.cnt = 0;
        roundabout.In_dis_flag = 0;
        roundabout.In_dis_I = 0;
        point_center = 150 * ((Tracking_Sensor_data_5_LCD - Tracking_Sensor_data_3_LCD)\
                              / (Tracking_Sensor_data_3_LCD + Tracking_Sensor_data_5_LCD));
        Fres = 4;
    }
    if( ( ( (roundabout.In_turn_I > 40) && (roundabout.In_turn_I < 70) ) || \
            ( (roundabout.In_turn_I < -40) && (roundabout.In_turn_I > -70) ) ) && (roundabout.In_flag) )
    {
        roundabout.In_process = 0;
        gpio_set(A15, 0);
        roundabout.Speed = 0;
    }
    if( (roundabout.In_turn_I < -250) || (roundabout.In_turn_I > 250) )
    {
        roundabout.Out_flag = 1;
        //		roundabout.Speed = 1;
    }
    if(roundabout.Out_flag)
{
    roundabout.In_flag = 0;
    roundabout.In_I = 0;
    roundabout.In_turn_I = 0;
    gpio_set(A15, 1);
        Tracking_ADC_Val_Sum = Tracking_Sensor_data_2_LCD + Tracking_Sensor_data_1_LCD;
        Tracking_ADC_Val_Sub = Tracking_Sensor_data_2_LCD - Tracking_Sensor_data_1_LCD;
        point_center = 80 * (Tracking_ADC_Val_Sub / Tracking_ADC_Val_Sum); //��ͱȵõ�ƫ��
        //		point_center = 100*(((Tracking_Sensor_data_5_LCD-Tracking_Sensor_data_4_LCD*0.8)-\
        //		 (Tracking_Sensor_data_1_LCD-Tracking_Sensor_data_2_LCD*0.8))/(Tracking_Sensor_data_1_LCD+\
        //		+Tracking_Sensor_data_5_LCD));
    }
    if(roundabout.Out_I > 2500)
{
    gpio_set(A15, 0);
        //		roundabout.Speed = 0;
        roundabout.Out_flag = 0;
        roundabout.Out_I = 0;
        roundabout.frequency--;
    }
}


/******************����·��***************************/
void Thressload(void)
{
    if( (Tracking_Sensor_data_1_LCD < 550) && (Tracking_Sensor_data_2_LCD < 600) && \
            (Tracking_Sensor_data_3_LCD < 700) && (!thressload.time) )
    {
        thressload.time = 1;
    }

    //	if( (Tracking_Sensor_data_1_LCD < 1000) && (Tracking_Sensor_data_2_LCD < 1150) && \
    //		(Tracking_Sensor_data_3_LCD < 1400) &&	(Tracking_Sensor_data_4_LCD < 1150) && \
    //	(Tracking_Sensor_data_5_LCD < 1000) && (Out_Thressload_Flag) && (Tracking_Sensor_data_3_LCD > 500) && \
    //	(!In_Roundabout_Flag) && (!Out_Roundabout_I) && (!Thressload_error) && (distance_I > 42000)\
    //	&& (distance_I < 46000))
    if( (Tracking_Sensor_data_3_LCD < 1200) && (thressload.Out_flag) && (Tracking_Sensor_data_3_LCD > 500) && \
            (!roundabout.In_flag) && (!roundabout.Out_flag) && (!thressload.time) && (distance_I > 45000)\
            && (distance_I < 55000))
    {
        thressload.cnt++;
    }
    if(thressload.cnt > 3)
    {
        thressload.In_flag = 1;
    }
    if(thressload.In_flag)
    {
        gpio_set(A15, 0);
        thressload.Out_flag = 0;
        //		point_center = -50;//��ת
        point_center = 50;//��ת
        //		Tracking_ADC_Val_Sum = Tracking_Sensor_data_5_LCD + Tracking_Sensor_data_2_LCD;
        //		Tracking_ADC_Val_Sub = Tracking_Sensor_data_5_LCD - Tracking_Sensor_data_2_LCD-800;
        //		point_center=100*(Tracking_ADC_Val_Sub/Tracking_ADC_Val_Sum); //��ͱȵõ�ƫ�� ��� (��)
        //		Tracking_ADC_Val_Sum = Tracking_Sensor_data_3_LCD + Tracking_Sensor_data_1_LCD;
        //		Tracking_ADC_Val_Sub = Tracking_Sensor_data_3_LCD + 350 - Tracking_Sensor_data_1_LCD;
        //		point_center=100*(Tracking_ADC_Val_Sub/Tracking_ADC_Val_Sum); //��ͱȵõ�ƫ��  �ҹ�
    }

    if( ( (thressload.In_turn_I > 30) || (thressload.In_turn_I < -30) ) && (!thressload.Out_flag) )
    {
        thressload.cnt = 0;
        thressload.In_flag = 0;
    }
    if( (thressload.Out_I > 1500) && (thressload.Out_I < 2000) )
    {
        thressload.In_I = 0;
        thressload.In_turn_I = 0;
    }
    if(thressload.Out_I > 1500)
    {
        //		Communication_S_Flag = 1;
        thressload.Target_flag = 1;
        thressload.Out_I = 0;
        thressload.Out_flag = 1;
        thressload.Stop_Flag = 1;
    }
}

/******************�µ�***************************/
void Rampway(void)
{
    if( (Tracking_Sensor_data_3_LCD > 3900) && (Speed_Now > 1) && \
            ( ( (Tracking_Sensor_data_2_LCD - Tracking_Sensor_data_4_LCD) < 400) || \
              ( (Tracking_Sensor_data_2_LCD - Tracking_Sensor_data_4_LCD) > -400) ) )
    {
        rampway.In_flag = 1;
        gpio_set(A15, 1);
    }
    if( (rampway.In_I > 1100) && (rampway.In_I < 2000) && (Tracking_Sensor_data_3_LCD < 1000) && (Speed_Now < 120) )
    {
        //		gpio_set(A15,1);
    }
    else
    {
        rampway.In_flag = 0;
        rampway.In_I = 0;
    }
    if(rampway.In_I > 3000)
    {
        rampway.In_flag = 0;
        rampway.In_I = 0;
    }
}

