/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,��ɿƼ�
* All rights reserved.
* ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897
*
* �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
* ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
*
* @file             SEEKFREE_ICM20602
* @company          �ɶ���ɿƼ����޹�˾
* @author           ��ɿƼ�(QQ3184284598)
* @version          �鿴doc��version�ļ� �汾˵��
* @Software         IAR 8.3 or MDK 5.24
* @Taobao           https://seekfree.taobao.com/
* @date             2020-11-23
* @note
*                   ���߶��壺
*                   ------------------------------------
*                   ģ��ܽ�            ��Ƭ���ܽ�
*                   SCL                 �鿴SEEKFREE_IIC�ļ��ڵ�SEEKFREE_SCL�궨��
*                   SDA                 �鿴SEEKFREE_IIC�ļ��ڵ�SEEKFREE_SDA�궨��
*                   ------------------------------------
********************************************************************************************************************/

#include "zf_systick.h"
#include "zf_gpio.h"
#include "zf_spi.h"
#include "SEEKFREE_IIC.h"
#include "SEEKFREE_ICM20602.h"
#include "All_Init.h"

extern int16 BalanceControlOut;
extern float BalancePIDParam[][7];
extern int angle_out;
extern float angle_v_out;
int  extern v_out;
extern int  Direct_Parameter;// ת��ϵ��;

int16 icm_gyro_x, icm_gyro_y, icm_gyro_z;
int16 icm_acc_x, icm_acc_y, icm_acc_z;
//-------------------------------------------------------------------------------------------------------------------
// ���º�����ʹ�����IICͨ��
//-------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------
// @brief       ICM20602�Լ캯��
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:
// @note        �ڲ����� �û��������
//-------------------------------------------------------------------------------------------------------------------
static void icm20602_self1_check(void)
{
    uint8 dat = 0;
    while (0x12 != dat)                                                             // �ж� ID �Ƿ���ȷ
    {
        dat = simiic_read_reg(ICM20602_DEV_ADDR, ICM20602_WHO_AM_I, SIMIIC);        // ��ȡICM20602 ID
        systick_delay_ms(10);
        //��������ԭ�������¼���
        //1 ICM20602���ˣ�������µ������ĸ��ʼ���
        //2 ���ߴ������û�нӺ�
        //3 ��������Ҫ����������裬������3.3V
        //4 ����û�е���ģ��IIC�ĳ�ʼ������
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ��ʼ��ICM20602
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void icm20602_init(void)
{
    simiic_init();
    systick_delay_ms(10);  //�ϵ���ʱ

    //���
    icm20602_self1_check();

    //��λ
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_PWR_MGMT_1, 0x80);                 // ��λ�豸
    systick_delay_ms(2);                                                            // ��ʱ
    while (0x80 & simiic_read_reg(ICM20602_DEV_ADDR, ICM20602_PWR_MGMT_1, SIMIIC)); // �ȴ���λ���

    //���ò���
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_PWR_MGMT_1, 0x01);                 // ʱ������
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_PWR_MGMT_2, 0x00);                 // ���������Ǻͼ��ٶȼ�
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_CONFIG, 0x01);                     // 176HZ 1KHZ
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_SMPLRT_DIV, 0x07);                 // �������� SAMPLE_RATE = INTERNAL_SAMPLE_RATE / (1 + SMPLRT_DIV)
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_GYRO_CONFIG, 0x18);                // ��2000 dps
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_ACCEL_CONFIG, 0x10);               // ��8g
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_ACCEL_CONFIG_2, 0x03);             // Average 4 samples   44.8HZ   //0x23 Average 16 samples
}


//-------------------------------------------------------------------------------------------------------------------
// @brief       ��ȡICM20602���ٶȼ�����
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:                ִ�иú�����ֱ�Ӳ鿴��Ӧ�ı�������
//-------------------------------------------------------------------------------------------------------------------
void get_icm20602_accdata(void)
{
    uint8 dat[6];

    simiic_read_regs(ICM20602_DEV_ADDR, ICM20602_ACCEL_XOUT_H, dat, 6, SIMIIC);
    icm_acc_x = (int16)(((uint16)dat[0] << 8 | dat[1]));
    icm_acc_y = (int16)(((uint16)dat[2] << 8 | dat[3]));
    icm_acc_z = (int16)(((uint16)dat[4] << 8 | dat[5]));
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ��ȡICM20602����������
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:                ִ�иú�����ֱ�Ӳ鿴��Ӧ�ı�������
//-------------------------------------------------------------------------------------------------------------------
void get_icm20602_gyro(void)
{
    uint8 dat[6];

    simiic_read_regs(ICM20602_DEV_ADDR, ICM20602_GYRO_XOUT_H, dat, 6, SIMIIC);
    icm_gyro_x = (int16)(((uint16)dat[0] << 8 | dat[1]));
    icm_gyro_y = (int16)(((uint16)dat[2] << 8 | dat[3]));
    icm_gyro_z = (int16)(((uint16)dat[4] << 8 | dat[5]));
}

//-------------------------------------------------------------------------------------------------------------------
// ���Ϻ�����ʹ�����IICͨ��
//-------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------
// ���º�����ʹ��Ӳ��SPIͨ�� ��Ƚ�IIC �ٶȱ�IIC��ǳ���
//-------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------
// @brief       ICM20602 SPIд�Ĵ���
// @param       cmd             �Ĵ�����ַ
// @param       val             ��Ҫд�������
// @return      void
// @since       v1.0
// Sample usage:
// @note        �ڲ����� �û��������
//-------------------------------------------------------------------------------------------------------------------
static void icm_spi_w_reg_byte(uint8 cmd, uint8 val)
{
    uint8 dat[2];
    ICM20602_CS(0);
    dat[0] = cmd | ICM20602_SPI_W;
    dat[1] = val;

    spi_mosi(ICM20602_SPI, dat, dat, 2);
    ICM20602_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ICM20602 SPI���Ĵ���
// @param       cmd             �Ĵ�����ַ
// @param       *val            �������ݵĵ�ַ
// @return      void
// @since       v1.0
// Sample usage:
// @note        �ڲ����� �û��������
//-------------------------------------------------------------------------------------------------------------------
static void icm_spi_r_reg_byte(uint8 cmd, uint8 *val)
{
    uint8 dat[2];
    ICM20602_CS(0);
    dat[0] = cmd | ICM20602_SPI_R;
    dat[1] = *val;

    spi_mosi(ICM20602_SPI, dat, dat, 2);

    *val = dat[1];
    ICM20602_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ICM20602 SPI���ֽڶ��Ĵ���
// @param       cmd             �Ĵ�����ַ
// @param       *val            �������ݵĵ�ַ
// @param       num             ��ȡ����
// @return      void
// @since       v1.0
// Sample usage:
// @note        �ڲ����� �û��������
//-------------------------------------------------------------------------------------------------------------------
static void icm_spi_r_reg_bytes(uint8 *val, uint8 num)
{
    ICM20602_CS(0);
    spi_mosi(ICM20602_SPI, val, val, num);
    ICM20602_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ICM20602�Լ캯��
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:
// @note        �ڲ����� �û��������
//-------------------------------------------------------------------------------------------------------------------
static void icm20602_self3_check(void)
{
    uint8 dat = 0;

    while (0x12 != dat)                                                             // �ж� ID �Ƿ���ȷ
    {
        icm_spi_r_reg_byte(ICM20602_WHO_AM_I, &dat);                                // ��ȡICM20602 ID
        systick_delay_ms(10);
        //��������ԭ�������¼���
        //1 ICM20602���ˣ�������µ������ĸ��ʼ���
        //2 ���ߴ������û�нӺ�
        //3 ��������Ҫ����������裬������3.3V
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ��ʼ��ICM20602
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void icm20602_init_spi(void)
{
    uint8 val = 0x0;

    systick_delay_ms(10);  //�ϵ���ʱ

    spi_init(ICM20602_SPI, ICM20602_SCK_PIN, ICM20602_MOSI_PIN, ICM20602_MISO_PIN, SPI_NSS_NULL, 0, SystemCoreClock / 4); // Ӳ��SPI��ʼ��

    gpio_init(ICM20602_CS_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);

    icm20602_self3_check();//���

    icm_spi_w_reg_byte(ICM20602_PWR_MGMT_1, 0x80); //��λ�豸
    systick_delay_ms(2);
    do                                                                              // �ȴ���λ�ɹ�
    {
        icm_spi_r_reg_byte(ICM20602_PWR_MGMT_1, &val);
    }
    while (0x41 != val);

    icm_spi_w_reg_byte(ICM20602_PWR_MGMT_1,     0x01);                              // ʱ������
    icm_spi_w_reg_byte(ICM20602_PWR_MGMT_2,     0x00);                              // ���������Ǻͼ��ٶȼ�
    icm_spi_w_reg_byte(ICM20602_CONFIG,         0x01);                              // 176HZ 1KHZ
    icm_spi_w_reg_byte(ICM20602_SMPLRT_DIV,     0x07);                              // �������� SAMPLE_RATE = INTERNAL_SAMPLE_RATE / (1 + SMPLRT_DIV)
    icm_spi_w_reg_byte(ICM20602_GYRO_CONFIG,    0x18);                              // ��2000 dps
    icm_spi_w_reg_byte(ICM20602_ACCEL_CONFIG,   0x10);                              // ��8g
    icm_spi_w_reg_byte(ICM20602_ACCEL_CONFIG_2, 0x03);                              // Average 4 samples   44.8HZ   //0x23 Average 16 samples
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ��ȡICM20602���ٶȼ�����
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:                ִ�иú�����ֱ�Ӳ鿴��Ӧ�ı�������
//-------------------------------------------------------------------------------------------------------------------
void get_icm20602_accdata_spi(void)
{
    struct
    {
        uint8 reg;
        uint8 dat[6];
    } buf;

    buf.reg = ICM20602_ACCEL_XOUT_H | ICM20602_SPI_R;

    icm_spi_r_reg_bytes(&buf.reg, 7);
    icm_acc_x = (int16)(((uint16)buf.dat[0] << 8 | buf.dat[1]));
    icm_acc_y = (int16)(((uint16)buf.dat[2] << 8 | buf.dat[3]));
    icm_acc_z = (int16)(((uint16)buf.dat[4] << 8 | buf.dat[5]));
    ICM_State.accdata.x = icm_acc_x;
    ICM_State.accdata.y = icm_acc_y;
    ICM_State.accdata.z = icm_acc_z;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ��ȡICM20602����������
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:                ִ�иú�����ֱ�Ӳ鿴��Ӧ�ı�������
//-------------------------------------------------------------------------------------------------------------------
void get_icm20602_gyro_spi(void)
{
    struct
    {
        uint8 reg;
        uint8 dat[6];
    } buf;

    buf.reg = ICM20602_GYRO_XOUT_H | ICM20602_SPI_R;

    icm_spi_r_reg_bytes(&buf.reg, 7);
    icm_gyro_x = (int16)(((uint16)buf.dat[0] << 8 | buf.dat[1]));
    icm_gyro_y = (int16)(((uint16)buf.dat[2] << 8 | buf.dat[3]));
    icm_gyro_z = (int16)(((uint16)buf.dat[4] << 8 | buf.dat[5]));
    ICM_State.gyro.x = icm_gyro_x;
    ICM_State.gyro.y = icm_gyro_y;
    ICM_State.gyro.z = icm_gyro_z;
}

//-------------------------------------------------------------------------------------------------------------------
// ���º�����ʹ��Ӳ��SPIͨ�� ��Ƚ�IIC �ٶȱ�IIC��ǳ���
//-------------------------------------------------------------------------------------------------------------------
//------------------------------˽�п�------------------------------------//
ICM20602 ICM_State; //��ǰ����ֵ
ICM20602 ICM_Offset;//��Ưֵ
ICM20602_Treated ICM_Treated; //�����������������
uint8 ICM20602_Offset_Finished = 0;
ICM20602_IntegrationTypedef ICMIntegrate;

/***************************************************************
@Fucation ��ICM20602��ƫ
@Author   �� bc
@Data     �� 2019/2/1
@Note     �� ��ʼ������
***************************************************************/

bool ICM20602_Offset(void)
{
    uint8 i, j, Count = 100;
    int64 temp[6] = {0};

    //���û��ֱ�־
    ICMIntegrate.Yaw_I_Enable = 'F';
    ICMIntegrate.TurnLeft_I = 0.000f;
    ICMIntegrate.TurnRight_I = 0.000f;
    ICMIntegrate.Pitch_I_Enable = 'F';
    ICMIntegrate.Up_I = 0.000f;
    ICMIntegrate.Down_I = 0.000f;
    ICMIntegrate.Row_I_Enable = 'F';
    ICMIntegrate.Clockwise_I = 0.000f;
    ICMIntegrate.Anticlockwise_I = 0.000f;


    for (i = 0; i < Count; i++)
    {
        get_icm20602_accdata_spi();//���ٶȼ�
        get_icm20602_gyro_spi();//������
        systick_delay_ms(1);

        temp[0] += ICM_State.accdata.x;
        temp[1] += ICM_State.accdata.y;
        temp[2] += ICM_State.accdata.z;

        temp[3] += ICM_State.gyro.x;
        temp[4] += ICM_State.gyro.y;
        temp[5] += ICM_State.gyro.z;
    }
    ICM_Offset.accdata.x = temp[0] / Count;
    ICM_Offset.accdata.y = temp[1] / Count;
    ICM_Offset.accdata.z = temp[2] / Count;

    ICM_Offset.gyro.x = temp[3] / Count;
    ICM_Offset.gyro.y = temp[4] / Count;
    ICM_Offset.gyro.z = temp[5] / Count;
    ICM20602_Offset_Finished = 1;

    //����ǶȲ����˲� ���������ǵ�׼ȷ��
    for (i = 0; i < 100; i++)
    {
        for (j = 0; j < 3; j++)
        {
            get_icm20602_accdata_spi();
            get_icm20602_gyro_spi();
            Data_Filter();                  // ��ԭʼ���ݻ����˲�
        }
    }
    return true;
}

/***************************************************************
@Fucation �� ���ݻ����˲�
@Author �� bc
@Data �� 2018/2/1
@Note ��
***************************************************************/
#define AcceRatio   16384.0f
#define GyroRatio   16.384f     //�����ĵ�λΪ ��/s
#define ACC_FILTER_NUM 5        // ���ٶȼ��˲����
#define GYRO_FILTER_NUM 20      // �������˲����

int32 ACC_X_BUF[ACC_FILTER_NUM], ACC_Y_BUF[ACC_FILTER_NUM], ACC_Z_BUF[ACC_FILTER_NUM];  // �˲���������
int32 GYRO_X_BUF[GYRO_FILTER_NUM], GYRO_Y_BUF[GYRO_FILTER_NUM], GYRO_Z_BUF[GYRO_FILTER_NUM];


void Data_Filter(void)  // �����˲�
{
    int i;
    int64 temp1 = 0, temp2 = 0, temp3 = 0, temp4 = 0, temp5 = 0, temp6 = 0;

    get_icm20602_accdata_spi();
    get_icm20602_gyro_spi();

    ACC_X_BUF[0] = ICM_State.accdata.x; // ���»�����������
    ACC_Y_BUF[0] = ICM_State.accdata.y;
    ACC_Z_BUF[0] = ICM_State.accdata.z;
    GYRO_X_BUF[0] = ICM_State.gyro.x;
    GYRO_Y_BUF[0] = ICM_State.gyro.y;
    GYRO_Z_BUF[0] = ICM_State.gyro.z;

    for (i = 0; i < ACC_FILTER_NUM; i++)
    {
        temp1 += ACC_X_BUF[i];
        temp2 += ACC_Y_BUF[i];
        temp3 += ACC_Z_BUF[i];

    }
    for (i = 0; i < GYRO_FILTER_NUM; i++)
    {
        temp4 += GYRO_X_BUF[i];
        temp5 += GYRO_Y_BUF[i];
        temp6 += GYRO_Z_BUF[i];
    }

    ICM_Treated.accdata.x = temp1 / ACC_FILTER_NUM / AcceRatio;
    ICM_Treated.accdata.y = temp2 / ACC_FILTER_NUM / AcceRatio;
    ICM_Treated.accdata.z = temp3 / ACC_FILTER_NUM / AcceRatio;
    ICM_Treated.gyro.x = temp4 / GYRO_FILTER_NUM / GyroRatio;
    ICM_Treated.gyro.y = temp5 / GYRO_FILTER_NUM / GyroRatio;
    ICM_Treated.gyro.z = temp6 / GYRO_FILTER_NUM / GyroRatio;

    for (i = 0; i < ACC_FILTER_NUM - 1; i++)
    {
        ACC_X_BUF[ACC_FILTER_NUM - 1 - i] = ACC_X_BUF[ACC_FILTER_NUM - 2 - i];
        ACC_Y_BUF[ACC_FILTER_NUM - 1 - i] = ACC_Y_BUF[ACC_FILTER_NUM - 2 - i];
        ACC_Z_BUF[ACC_FILTER_NUM - 1 - i] = ACC_Z_BUF[ACC_FILTER_NUM - 2 - i];

    }
    for (i = 0; i < GYRO_FILTER_NUM - 1; i++)
    {
        GYRO_X_BUF[GYRO_FILTER_NUM - 1 - i] = GYRO_X_BUF[GYRO_FILTER_NUM - 2 - i];
        GYRO_Y_BUF[GYRO_FILTER_NUM - 1 - i] = GYRO_Y_BUF[GYRO_FILTER_NUM - 2 - i];
        GYRO_Z_BUF[GYRO_FILTER_NUM - 1 - i] = GYRO_Z_BUF[GYRO_FILTER_NUM - 2 - i];
    }
}


void ICM_I_Int(void)
{
    if (ICMIntegrate.Yaw_I_Enable == 'T')
    {
        ICMIntegrate.TurnRight_I += (ICM_Treated.gyro.z < 0 ? ABS(ICM_Treated.gyro.z) * 0.005  : 0);
        ICMIntegrate.TurnLeft_I += (ICM_Treated.gyro.z > 0 ? ABS(ICM_Treated.gyro.z) * 0.005  : 0);
    }
    if (ICMIntegrate.Pitch_I_Enable == 'T')
    {
        ICMIntegrate.Down_I += (ICM_Treated.gyro.y < 0 ? ABS(ICM_Treated.gyro.y) * 0.005  : 0);
        ICMIntegrate.Up_I += (ICM_Treated.gyro.y > 0 ? ABS(ICM_Treated.gyro.y) * 0.005  : 0);
    }
    if (ICMIntegrate.Row_I_Enable == 'T')
    {
        ICMIntegrate.Clockwise_I += (ICM_Treated.gyro.x < 0 ? ABS(ICM_Treated.gyro.x) * 0.005  : 0);
        ICMIntegrate.Anticlockwise_I += (ICM_Treated.gyro.x > 0 ? ABS(ICM_Treated.gyro.x) * 0.005  : 0);
    }
}
//��ʾ�����Ǽ��ٶȼ���ֵ�Լ��ܹ���������Z����
float max_angle, min_angle;
void ICM20602_Show(void)
{
    int cntt = 239;
    max_angle = set_angle - 10, min_angle = set_angle + 10;

    ips114_clear(WHITE);
    bool start_draw = true;
    while (KeyPageUp != onepress)
    {
        if (KeyPageDown == onepress)
        {
            KeyPageDown = nopress;
            start_draw = !start_draw;
        }
        if (KeyLeft == onepress)
        {
            KeyLeft = nopress;
            BalancePIDParam[0][0] -= 0.25;
        }
        else if (KeyRight == onepress)
        {
            KeyRight = nopress;
            BalancePIDParam[0][0] += 00.25;
        }
        else if (KeyUp == onepress)
        {
            KeyUp = nopress;
            set_angle += 0.1;
        }
        else if (KeyDown == onepress)
        {
            KeyDown = nopress;
            set_angle -= 0.1;
        }

        if (start_draw)
        {
            if (cntt >= 239)
            {
                cntt = 0;
                {
                    ips114_clear(WHITE);
                    for (int i = 0; i < 240; i++)
                        ips114_drawpoint(i, 67, RED);
                }
            }
            int y = (int)(Attitude_Angel_P * 134 / (min_angle - max_angle) - max_angle * 134 / (min_angle - max_angle));
            ips114_drawpoint(cntt, y, BLACK);
            ips114_showfloat(3, 5, Attitude_Angel_P, 3, 2);
            ips114_showfloat(3, 6, (float) BalancePIDParam[0][0], 3, 2);
            ips114_showfloat(3, 7, (float)set_angle, 3, 2);
            ips114_showfloat(3, 0, (float)icm_gyro_y, 5, 2);
            ips114_showfloat(5, 1, (float)angle_out, 3, 2);
            ips114_showfloat(5, 2, (float)angle_v_out, 5, 2);
            ips114_showfloat(5, 3, (float)v_out / 300, 5, 2);
						//    ips114_showstr(0, 6, "D_P:");
		        ips114_showint16(100, 6, Direct_Parameter);

            cntt++;
        }



    }

    KeyPageUp = nopress;
}


float Attitude_Angel_P = 0.0000f;
float Attitude_Angel_R = 0.0000f;
float Attitude_Angel_Y = 0.0000f;


//===============================��Ԫ��============================================
#define Kp 25.0f //  1.2            // proportional gain governs rate of convergence to accelerometer/magnetometer
#define Ki 0.02f//0.001     // integral gain governs rate of convergence of gyroscope biases
#define halfT 0.003f  //0.003                   // half the sample period�������ڵ�һ��
float q0 = 1, q1 = 0, q2 = 0, q3 = 0;   // quaternion elements representing the estimated orientation
float exInt = 0, eyInt = 0, ezInt = 0;  // scaled integral error
/*
 * ��������IMUupdate
 * ����  ����Ԫ�ؽ���ŷ����
 * ����  �������� ���ٶȼ�
 * ���  ����
 * ����  ���ڲ�����
 */
void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az)
{
    float norm;
    float vx, vy, vz;
    float ex, ey, ez;

    // �Ȱ���Щ�õõ���ֵ���
    float q0q0 = q0 * q0;
    float q0q1 = q0 * q1;
    float q0q2 = q0 * q2;
    float q1q1 = q1 * q1;
    float q1q3 = q1 * q3;
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    float q3q3 = q3 * q3;

    if (ax * ay * az == 0)
    {
        return;
    }

    norm = sqrt(ax * ax + ay * ay + az * az); // acc���ݹ�һ��
    ax = ax / norm;
    ay = ay / norm;
    az = az / norm;

    // estimated direction of gravity and flux (v and w)    �����������������/��Ǩ
    vx = 2 * (q1q3 - q0q2);                                 // ��Ԫ����xyz�ı�ʾ
    vy = 2 * (q0q1 + q2q3);
    vz = q0q0 - q1q1 - q2q2 + q3q3 ;

    // error is sum of cross product between reference direction of fields and direction measured by sensors
    ex = (ay * vz - az * vy) ;  // �������������õ���־������
    ey = (az * vx - ax * vz) ;
    ez = (ax * vy - ay * vx) ;

    exInt = exInt + ex * Ki;    // �������л���
    eyInt = eyInt + ey * Ki;
    ezInt = ezInt + ez * Ki;

    // adjusted gyroscope measurements
    gx = gx + Kp * ex + exInt;  // �����PI�󲹳��������ǣ����������Ư��
    gy = gy + Kp * ey + eyInt;
    gz = gz + Kp * ez + ezInt;  // �����gz����û�й۲��߽��н��������Ư�ƣ����ֳ����ľ��ǻ����������Լ�

    // integrate quaternion rate and normalise  // ��Ԫ�ص�΢�ַ���
    q0 = q0 + (-q1 * gx - q2 * gy - q3 * gz) * halfT;
    q1 = q1 + (q0 * gx + q2 * gz - q3 * gy) * halfT;
    q2 = q2 + (q0 * gy - q1 * gz + q3 * gx) * halfT;
    q3 = q3 + (q0 * gz + q1 * gy - q2 * gx) * halfT;

    // normalise quaternion
    norm = my_sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 = q0 / norm;
    q1 = q1 / norm;
    q2 = q2 / norm;
    q3 = q3 / norm;

    Attitude_Angel_P = asin(-2 * q1 * q3 + 2 * q0 * q2) * 57.3; // pitch
    Attitude_Angel_R = fast_atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) * 57.3; // roll
    Attitude_Angel_Y = fast_atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2 * q2 - 2 * q3 * q3 + 1) * 57.3; // yaw
    //  Attitude_Angle.Z = 0;
}

float my_sqrt(float number)
{
    long i;
    float x, y;
    const float f = 1.5F;
    x = number * 0.5F;
    y = number;
    i = * (long *) &y;
    i = 0x5f3759df - (i >> 1);

    y = * (float *) &i;
    y = y * (f - (x * y * y));
    y = y * (f - (x * y * y));
    return number * y;
}
float  fast_atan2(float y, float x)
{
    float ax = ABS_FLOAT(x), ay = ABS_FLOAT(y);
    float a = MIN(ax, ay) / (MAX(ax, ay) + (float)DBL_EPSILON);
    float s = a * a;
    float r = ((-0.0464964749 * s + 0.15931422) * s - 0.327622764) * s * a + a;
    if (ay > ax) r = 1.57079637 - r;
    if (x < 0) return 3.14159274f - r;
    if (y < 0) return   -r;
    return r;
}

////////////////////////////////////////////////////////////////////////////////

