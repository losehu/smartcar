//�����豸�Լ�������ʼ��
#include "devices.h"


//all device
void Decices_Init(void)
{
    //LED
    //  gpio_init(LED0_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);//LED0
    //  gpio_init(P9813_SDA_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);//P9813�ӿ�
    //  gpio_init(P9813_SCL_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);//P9813�ӿ�
    //  RGBLED(RGB24WHITE);
    //beep
    //gpio_init(BEEP_PIN,GPO,0,GPO_PUSH_PULL);
    //encoder
    tim_counter_init(ENCODER1_TIM, ENCODER1_A);//ʹ�÷��������
    tim_counter_init(ENCODER2_TIM, ENCODER2_A);
    gpio_init(ENCODER1_DIR, GPI, 0, GPI_FLOATING_IN);
    gpio_init(ENCODER2_DIR, GPI, 0, GPI_FLOATING_IN);
    //  tim_encoder_init(ENCODER1_TIM, ENCODER1_A, ENCODER1_B);//ʹ������������
    //  tim_encoder_init(ENCODER2_TIM, ENCODER2_A, ENCODER2_B);

    //motor
    pwm_init(PWM_TIM, PWM_L0, 17000, 0);                                                // PWM ͨ��2 ��ʼ��Ƶ��10KHz ռ�ձȳ�ʼΪ0
    pwm_init(PWM_TIM, PWM_L1, 17000, 0);
    pwm_init(PWM_TIM, PWM_R0, 17000, 0);
    pwm_init(PWM_TIM, PWM_R1, 17000, 0);
    pwm_l = 0;
    pwm_r = 0;
    //servo
    pwm_init(SERVOPWM_TIM, PWM_SERVO, 50, Servo_mid);
    Servo_add = 0;
    //adc inductor
    adc_init(ADC_1, ADC1_CH04_A04, ADC_12BIT);
    adc_init(ADC_1, ADC1_CH07_A07, ADC_12BIT);
    adc_init(ADC_1, ADC1_CH05_A05, ADC_12BIT);
    adc_init(ADC_1, ADC1_CH06_A06, ADC_12BIT);
    //key
    gpio_init(KEY_CENTER_PIN, GPI, GPIO_HIGH, GPI_PULL_UP);
    gpio_init(KEY_UP_PIN, GPI, GPIO_HIGH, GPI_PULL_UP);
    gpio_init(KEY_DOWN_PIN, GPI, GPIO_HIGH, GPI_PULL_UP);
    gpio_init(KEY_LEFT_PIN, GPI, GPIO_HIGH, GPI_PULL_UP);
    gpio_init(KEY_RIGHT_PIN, GPI, GPIO_HIGH, GPI_PULL_UP);
    gpio_init(PAGE_UP_PIN, GPI, GPIO_HIGH, GPI_PULL_UP);
    gpio_init(PAGE_DOWN_PIN, GPI, GPIO_HIGH, GPI_PULL_UP);

}


//===============================================LED======================================//


void LED_Test(void)
{
    LED0_ON;
    delay_ms(100);
    LED0_OFF;
    delay_ms(100);
}


//P9813ȫ��ELD����  ����ǰ32λ'0'��ʼ֡
void RGBLED_Send32Zero(void)
{
    uint8 i;
    P9813_SDA(0);
    for (i = 0; i < 32; i++)
    {
        P9813_SCL(0);
        delay_us(200);
        P9813_SCL(1);
        delay_us(200);
    }
}

//��תǰ��λ�ĻҶ�ֵ
uint8 RGBLED_TakeAntiCode(uint8 dat)
{
    uint8 tmp = 0;

    tmp = ((~dat) & 0xC0) >> 6;
    return tmp;
}

//���ͻҶ�����
void RGBLED_DatSend(uint32 dx)
{
    uint8 i;
    for (i = 0; i < 32; i++)
    {
        if ((dx & 0x80000000) != 0)
            P9813_SDA(1);
        else
            P9813_SDA(0);
        dx <<= 1;
        P9813_SCL(0);
        delay_us(200);
        P9813_SCL(1);
        delay_us(200);
    }
}


//���ݴ����뷢��
//r;��ɫֵ 0~255
//g:��ɫֵ 0~255
//b:��ɫֵ 0~255
void RGBLED_DataDealWithAndSend(uint8 r, uint8 g, uint8 b)
{
    uint32 dx = 0;

    dx |= (uint32)0x03 << 30;  //ǰ��λ'1'λ��־λ
    dx |= (uint32)RGBLED_TakeAntiCode(b) << 28;
    dx |= (uint32)RGBLED_TakeAntiCode(g) << 26;
    dx |= (uint32)RGBLED_TakeAntiCode(r) << 24;

    dx |= (uint32)b << 16;
    dx |= (uint32)g << 8;
    dx |= r;
    RGBLED_DatSend(dx);//��������
}


//RGB�ƿ��ƺ���
//r;��ɫֵ 0~255
//g:��ɫֵ 0~255
//b:��ɫֵ 0~255
void RGBLED_Show(uint8 r, uint8 g, uint8 b)
{
    RGBLED_Send32Zero();//����ǰ32λ'0'��ʼ֡
    RGBLED_DataDealWithAndSend(r, g, b);//����32bit�Ҷ�����
    RGBLED_DataDealWithAndSend(r, g, b);
}


//RGBLED ��ɫ
void RGBLED(uint32  COLOR32BIT)
{
    RGBLED_Show((uint8)((COLOR32BIT >> 16) & 0xff), \
                (uint8)((COLOR32BIT >> 8) & 0xff), (uint8)(COLOR32BIT & 0xff));
    //  P9813_SCL(1);
    //  P9813_SDA(1);
}


void RGBLED_test(void)
{
    uint8 r = 0, g = 0, b = 0;
    while (1)
    {
        r += 1;
        g += 3;
        b += 5;
        RGBLED_Show(r, g, b);
        delay_ms(20);
    }
}

//==============================================BEEP======================================//

//statement
BeepTypedef Beep;
//����೤ʱ�� �켸�� ���ж���
void Beep_DiDi(uint16 n, uint16 time)
{
    if (Beep.State == OFF)
    {
        Beep.State = ON;
        Beep.Time  = time;
        Beep.Num   = n;
        if (Beep.Num == 0) Beep.Num = 1;
        Beep.Cnt = 1;
        BEEP_OFF;
    }
}


//���ж��м���
void BeepInt(void)
{
    if (Beep.State == ON)
    {
        Beep.Cnt ++;
        if (Beep.Cnt % Beep.Time == 0) BEEP_TOGGLE;
        if ((Beep.Cnt % Beep.Time == 0) && ((Beep.Cnt / Beep.Time) == (Beep.Num * 2)))
            Beep.State = OFF;
    }
}


//===============================================ENCODER======================================//
//statement
EncoderTypedef L_Wheel, R_Wheel;
WheelTypedef  Wheel;
/**
 * @name:void Encoder_Read(void)
 * @function:��������ֵ ע��ÿ�ζ�ֵ�ж�Ϊ10ms
 * @param:none
 * @return:none
 */
void Encoder_Read(void)
{
#if(ENCODER_L_DIR == 0)
    L_Wheel.encoder = tim_encoder_get_count(ENCODER2_TIM);
#endif
#if(ENCODER_L_DIR == 1)
    L_Wheel.encoder = - tim_encoder_get_count(ENCODER2_TIM);
#endif
#if(ENCODER_R_DIR == 0)
    R_Wheel.encoder = tim_encoder_get_count(ENCODER1_TIM);
#endif
#if(ENCODER_R_DIR == 1)
    R_Wheel.encoder = - tim_encoder_get_count(ENCODER1_TIM);
#endif
    tim_counter_rst(ENCODER1_TIM);
    tim_counter_rst(ENCODER2_TIM);
    if (gpio_get(ENCODER1_DIR) == 1)R_Wheel.encoder = -R_Wheel.encoder;
    if (gpio_get(ENCODER2_DIR) == 0)L_Wheel.encoder = -L_Wheel.encoder;
    /**********�������ֲ���**********/
    //��ʵ�ٶ�
    L_Wheel.now = (float)L_Wheel.encoder * 0.1542;
    L_Wheel.acc = L_Wheel.now - L_Wheel.last;
    //�˲�
    if (ABS_FLOAT(L_Wheel.acc) > 80)
        L_Wheel.now = L_Wheel.now * 0.5 + L_Wheel.last * 0.5;
    else
        L_Wheel.now = L_Wheel.now * 0.9 + L_Wheel.last * 0.1;
    //�޷�
    L_Wheel.out = range_protect_float(L_Wheel.now, -500, 500);
    //��¼
    L_Wheel.last = L_Wheel.now;
    /**********�������ֲ���**********/
    //��ʵ�ٶ�
    R_Wheel.now = 1.0 * (float)R_Wheel.encoder * 0.2142;
    R_Wheel.acc = R_Wheel.now - R_Wheel.last;
    //�˲�
    if (ABS_FLOAT(R_Wheel.acc) > 80)
        R_Wheel.now = R_Wheel.now * 0.5 + R_Wheel.last * 0.5;
    else
        R_Wheel.now = R_Wheel.now * 0.9 + R_Wheel.last * 0.1;
    //�޷����
    R_Wheel.out = range_protect_float(R_Wheel.now, -500, 500);
    //��¼
    R_Wheel.last = R_Wheel.now;
    /**********��ʵ�ٶ��Լ�·��**********/
    //�������
    Wheel.DistanceAll += (float)((R_Wheel.encoder + L_Wheel.encoder) / 2) / 450 / 100;
    //��ʵ�ٶ� PAI * BodyR /180 = 0.17
    Wheel.LSpeed =  L_Wheel.out/* - ICM_Treated.gyro.z * 0.17*/;
    Wheel.RSpeed =  R_Wheel.out/* + ICM_Treated.gyro.z * 0.17*/;
    //ѡ���ٶ�
    if (ICM_Treated.gyro.z < -50) //��ת����
    {
        Wheel.now = (L_Wheel.out + R_Wheel.out) / 2 +ABS( ICM_Treated.gyro.z) * 0.03;
    }
    else if (ICM_Treated.gyro.z > 50) //��תȡ��
    {
        Wheel.now = (L_Wheel.out + R_Wheel.out) / 2 + ABS(ICM_Treated.gyro.z) * 0.03;
    }
    else
    {
        Wheel.now = (L_Wheel.out + R_Wheel.out) / 2;
    }
    //�˲�
       // Wheel.now = (L_Wheel.out*(1+ICM_Treated.gyro.z/2000) + R_Wheel.out*(1-ICM_Treated.gyro.z/2000)) / 2;
//Wheel.now = (L_Wheel.out + R_Wheel.out) / 2;
    Wheel.now = Wheel.now * 0.9 + Wheel.last * 0.1;
    Wheel.last = Wheel.now;
}

//1m��������������
uint8 Encoder_Show(void)
{
    while (KeyPageUp != onepress)
    {
        //Encoder_Read();
        IPS114_PENCOLOR = RED;
        ips114_showstr(0, 0, "Lw.encoder:");
        ips114_showstr(0, 1, "Lw.now:");
        ips114_showstr(0, 2, "Rw.encoder:");
        ips114_showstr(0, 3, "Rw.now:");
        ips114_showstr(0, 4, "Wheel.now:");
        IPS114_PENCOLOR = BLUE;
        ips114_showint16(120, 0, L_Wheel.encoder);
        ips114_showfloat(120, 1, L_Wheel.now, 5, 2);
        ips114_showint16(120, 2, R_Wheel.encoder);
        ips114_showfloat(120, 3, R_Wheel.now, 5, 2);
        ips114_showfloat(120, 4, Wheel.now, 5, 2);
        IPS114_PENCOLOR = RED;
        delay_ms(10);
    }
    KeyPageUp = nopress;
    return 1;
}

//===========================================MOTOR==========================================//


//statement
short pwm_l = 0;// 0 ~ 1000
short pwm_r = 0;

/**
 * @name:void PWM_Write(void)
 * @function:PWMд��
 * @param:none
 * @return:none
 */
void PWM_Write(void)
{
    //�޷�
    pwm_limit(pwm_l);
    pwm_limit(pwm_r);
    if (pwm_l < 0)
    {
        pwm_duty_updata(PWM_TIM, PWM_L0, 0);
        pwm_duty_updata(PWM_TIM, PWM_L1, -pwm_l);
    }
    else
    {
        pwm_duty_updata(PWM_TIM, PWM_L0, pwm_l);
        pwm_duty_updata(PWM_TIM, PWM_L1, 0);
    }
    //�ұߵ��
    if (pwm_r < 0)
    {
        pwm_duty_updata(PWM_TIM, PWM_R0, -pwm_r);
        pwm_duty_updata(PWM_TIM, PWM_R1,  0);
    }
    else
    {
        pwm_duty_updata(PWM_TIM, PWM_R0, 0);
        pwm_duty_updata(PWM_TIM, PWM_R1,  pwm_r);
    }
    //��ߵ��
}
const uint8 PWMItem0[] = "PWM_L";
const uint8 PWMItem1[] = "PWM_R";

#define MOTOR_MENU_NUM  1

//motor test
void Motor_Show(void)
{
    CarInfo.ClosedLoopMode = NOLoop;
    //  CarInfo.Protect = 'F';
    CarInfo.CAR_PROTECT_MODE = 'F';
    int16 pwm_l0 = 0, pwm_r0 = 0;
    while (KeyPageUp != onepress)
    {
        static int8 NowPWMItem = 0;
        Menu_Choose(&NowPWMItem, MOTOR_MENU_NUM);
        //��ʾ��Ŀ
        ips114_showstr(16, 0, PWMItem0);
        ips114_showstr(16, 1, PWMItem1);
        if (KeyPageDown == onepress)
        {
            KeyPageDown = nopress;
            switch (NowPWMItem)
            {
            case (0):
            {
                ParamsAdjustShort(&pwm_l0, 120, 0, 10);
                break;
            }
            case (1):
            {
                ParamsAdjustShort(&pwm_r0, 120, 1, 10);
                break;
            }
            }
            KeyPageDown = nopress;
        }
        IPS114_PENCOLOR = BLUE;
        ips114_showint16(120, 0, pwm_l0);
        ips114_showint16(120, 1, pwm_r0);
        IPS114_PENCOLOR = RED;
        pwm_l = pwm_l0;
        pwm_r = pwm_r0;
        PWM_Write();
        delay_ms(10);
    }
    CarInfo.ClosedLoopMode = AllLoop;
    //  CarInfo.Protect = 'F';
    CarInfo.CAR_PROTECT_MODE = 'T';
    KeyPageUp = nopress;

}


//===========================================SERVO==========================================//
short Servo_mid = 6200;//S3010�����ֵ����5000  //6200:7000-5400
short Servo_out = 0;
short Servo_add = 0;//-500~500

void SERVO_Write(void)
{
    range_protect(Servo_add, -800, 800); //�޷�
    Servo_out = (int32)(Servo_mid + Servo_add);
    pwm_duty_updata(SERVOPWM_TIM, PWM_SERVO, Servo_out);
}

void Servo_Test(void)
{
    IPS114_PENCOLOR = RED;
    ips114_showstr(0, 0, "Sevro Test:");
    Servo_add = 100;
    SERVO_Write();
    delay_ms(500);
    Servo_add = 200;
    SERVO_Write();
    delay_ms(500);
    Servo_add = 300;
    SERVO_Write();
    delay_ms(500);
    Servo_add = 200;
    SERVO_Write();
    delay_ms(500);
    Servo_add = 100;
    SERVO_Write();
    delay_ms(500);
    Servo_add = 0;
    SERVO_Write();
    delay_ms(500);
    Servo_add = -100;
    SERVO_Write();
    delay_ms(500);
    Servo_add = -200;
    SERVO_Write();
    delay_ms(500);
    Servo_add = -300;
    SERVO_Write();
    delay_ms(500);
    Servo_add = -200;
    SERVO_Write();
    delay_ms(500);
    Servo_add = -100;
    SERVO_Write();
    delay_ms(500);
    Servo_add = 0;
    SERVO_Write();
}

//===========================================KEY==========================================//
KeyStateEnum KeyCenter, KeyUp, KeyDown, KeyLeft, KeyRight;
KeyStateEnum KeyPageUp, KeyPageDown;


//���ж���ɨ�����
void KeyScanInt(void)
{
    static uint8 KeyPressNum = 0;
    static uint16 KeyPressTime = 0;
    static uint8 SomeKeyPressFlag = 0;//0 �ɿ� 1 �а���  2:������ 3:����
#define AlwaysPressTime 1600//һֱ�����ʱ��
#define debouncing  5
    if (SomeKeyPressFlag == 0 && (KEY_UP == 0 || KEY_DOWN == 0 || KEY_CENTER == 0 || \
                                  KEY_LEFT == 0 || KEY_RIGHT == 0 || KEY_PAGE_UP == 0 || KEY_PAGE_DOWN == 0))
    {
        SomeKeyPressFlag = 1;
    }
    if (SomeKeyPressFlag > 0)
    {
        KeyPressTime ++;
        //��ʱ5ms����
        if (SomeKeyPressFlag == 1 && KeyPressTime >= debouncing)
        {
            SomeKeyPressFlag = 2;
            if (KEY_UP == 0)
                KeyPressNum = 1;//��ⰴ������
            else if (KEY_DOWN == 0)
                KeyPressNum = 2;
            else if (KEY_LEFT == 0)
                KeyPressNum = 3;
            else if (KEY_RIGHT == 0)
                KeyPressNum = 4;
            else if (KEY_CENTER == 0)
                KeyPressNum = 5;
            else if (KEY_PAGE_UP == 0)
                KeyPressNum = 6;
            else if (KEY_PAGE_DOWN == 0)
                KeyPressNum = 7;
        }
        //��һ�¾��ɿ���״̬
        if ((KEY_UP == 1 && KEY_DOWN == 1 && KEY_LEFT == 1 && KEY_RIGHT == 1  && KEY_PAGE_UP == 1 && \
                KEY_PAGE_DOWN == 1 && KEY_CENTER == 1) && KeyPressTime < AlwaysPressTime && SomeKeyPressFlag == 2)
        {
            SomeKeyPressFlag = 0;//�����ɿ���
            if (KeyPressNum == 1)
                KeyUp = onepress;
            else if (KeyPressNum == 2)
                KeyDown = onepress;
            else if (KeyPressNum == 3)
                KeyLeft = onepress;
            else if (KeyPressNum == 4)
                KeyRight = onepress;
            else if (KeyPressNum == 5)
                KeyCenter = onepress;
            else if (KeyPressNum == 6)
                KeyPageUp = onepress;
            else if (KeyPressNum == 7)
                KeyPageDown = onepress;
            KeyPressTime = 0;
            SomeKeyPressFlag = 0;
            Beep_DiDi(1, 50);
        }
        //�������ɿ���״̬
        if (KeyPressTime >= AlwaysPressTime && SomeKeyPressFlag == 2)
        {
            if (KeyPressNum == 1)      KeyUp = alwayspress;
            else if (KeyPressNum == 2) KeyDown = alwayspress;
            else if (KeyPressNum == 3) KeyLeft = alwayspress;
            else if (KeyPressNum == 4) KeyRight = alwayspress;
            else if (KeyPressNum == 5) KeyCenter = alwayspress;
            else if (KeyPressNum == 6) KeyPageUp = alwayspress;
            else if (KeyPressNum == 7) KeyPageDown = alwayspress;
            Beep_DiDi(2, 50);
            if (KEY_UP == 1 && KEY_DOWN == 1 && KEY_LEFT == 1 && KEY_RIGHT == 1 && \
                    KEY_CENTER == 1 && KEY_PAGE_DOWN == 1 && KEY_PAGE_UP == 1)
            {
                SomeKeyPressFlag = 0;
                KeyPressTime = 0;
                KeyUp = nopress;
                KeyDown = nopress;
                KeyLeft = nopress;
                KeyRight = nopress;
                KeyPageUp = nopress;
                KeyPageDown = nopress;
                KeyCenter = nopress;
            }
        }
    }
}




