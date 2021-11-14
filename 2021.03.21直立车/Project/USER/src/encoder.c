#include "encoder.h"

int16 encoder_l = 0;
int16 encoder_r = 0;



void Encoder_Init(void)
{
    tim_encoder_init(TIM_1, TIM_1_ENC1_A08, TIM_1_ENC2_A01);
    tim_encoder_init(TIM_8, TIM_8_ENC1_C00, TIM_8_ENC2_C01);
}

void Encoder_Read(void)
{
    encoder_r = tim_encoder_get_count(TIM_1);
    encoder_l = tim_encoder_get_count(TIM_8);

    tim_encoder_rst(TIM_1);
    tim_encoder_rst(TIM_8);
}






