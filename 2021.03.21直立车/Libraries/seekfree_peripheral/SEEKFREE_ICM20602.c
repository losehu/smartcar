/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,��ɿƼ�
* All rights reserved.
* ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897
*
* �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
* ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
*
* @file				SEEKFREE_ICM20602
* @company			�ɶ���ɿƼ����޹�˾
* @author			��ɿƼ�(QQ3184284598)
* @version			�鿴doc��version�ļ� �汾˵��
* @Software			IAR 8.3 or MDK 5.24
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
* @note
* 					���߶��壺
* 					------------------------------------
* 					ģ��ܽ�			��Ƭ���ܽ�
* 					SCL					�鿴SEEKFREE_IIC�ļ��ڵ�SEEKFREE_SCL�궨��
* 					SDA					�鿴SEEKFREE_IIC�ļ��ڵ�SEEKFREE_SDA�궨��
* 					------------------------------------
********************************************************************************************************************/

#include "zf_systick.h"
#include "zf_gpio.h"
#include "zf_spi.h"
#include "SEEKFREE_IIC.h"
#include "SEEKFREE_ICM20602.h"

#include "Balance.h"
int ICM20602_Offset_Finished = 0; //��Ư������ɱ�־λ

int16 icm_gyro_x, icm_gyro_y, icm_gyro_z;
int16 icm_acc_x, icm_acc_y, icm_acc_z;

ICM20602 IMC_State; //��ǰ����ֵ
ICM20602 IMC_Offset;//��Ưֵ
ICM20602_Treated IMC_Treated; //�����������������



//-------------------------------------------------------------------------------------------------------------------
// ���º�����ʹ�����IICͨ��
//-------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------
// @brief		ICM20602�Լ캯��
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:
// @note		�ڲ����� �û��������
//-------------------------------------------------------------------------------------------------------------------
static void icm20602_self1_check(void)
{
    uint8 dat = 0;
    while(0x12 != dat)																// �ж� ID �Ƿ���ȷ
    {
        dat = simiic_read_reg(ICM20602_DEV_ADDR, ICM20602_WHO_AM_I, SIMIIC);			// ��ȡICM20602 ID
        systick_delay_ms(10);
        //��������ԭ�������¼���
        //1 ICM20602���ˣ�������µ������ĸ��ʼ���
        //2 ���ߴ������û�нӺ�
        //3 ��������Ҫ����������裬������3.3V
        //4 ����û�е���ģ��IIC�ĳ�ʼ������
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		��ʼ��ICM20602
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void icm20602_init(void)
{
    simiic_init();
    systick_delay_ms(10);  //�ϵ���ʱ

    //���
    icm20602_self1_check();

    //��λ
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_PWR_MGMT_1, 0x80);					// ��λ�豸
    systick_delay_ms(2);															// ��ʱ
    while(0x80 & simiic_read_reg(ICM20602_DEV_ADDR, ICM20602_PWR_MGMT_1, SIMIIC));	// �ȴ���λ���

    //���ò���
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_PWR_MGMT_1, 0x01);					// ʱ������
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_PWR_MGMT_2, 0x00);					// ���������Ǻͼ��ٶȼ�
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_CONFIG, 0x01);						// 176HZ 1KHZ
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_SMPLRT_DIV, 0x07);					// �������� SAMPLE_RATE = INTERNAL_SAMPLE_RATE / (1 + SMPLRT_DIV)
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_GYRO_CONFIG, 0x18);					// ��2000 dps
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_ACCEL_CONFIG, 0x10);					// ��8g
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_ACCEL_CONFIG_2, 0x03);				// Average 4 samples   44.8HZ   //0x23 Average 16 samples
}


//-------------------------------------------------------------------------------------------------------------------
// @brief		��ȡICM20602���ٶȼ�����
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:				ִ�иú�����ֱ�Ӳ鿴��Ӧ�ı�������
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
// @brief		��ȡICM20602����������
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:				ִ�иú�����ֱ�Ӳ鿴��Ӧ�ı�������
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
// @brief		ICM20602 SPIд�Ĵ���
// @param		cmd				�Ĵ�����ַ
// @param		val				��Ҫд�������
// @return		void
// @since		v1.0
// Sample usage:
// @note		�ڲ����� �û��������
//-------------------------------------------------------------------------------------------------------------------
static void icm_spi_w_reg_byte(uint8 cmd, uint8 val)
{
    uint8 dat[2];
    SPI_CS(0);
    dat[0] = cmd | ICM20602_SPI_W;
    dat[1] = val;

    spi_mosi(SPI_NUM, dat, dat, 2);
    SPI_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ICM20602 SPI���Ĵ���
// @param		cmd				�Ĵ�����ַ
// @param		*val			�������ݵĵ�ַ
// @return		void
// @since		v1.0
// Sample usage:
// @note		�ڲ����� �û��������
//-------------------------------------------------------------------------------------------------------------------
static void icm_spi_r_reg_byte(uint8 cmd, uint8 *val)
{
    uint8 dat[2];
    SPI_CS(0);
    dat[0] = cmd | ICM20602_SPI_R;
    dat[1] = *val;

    spi_mosi(SPI_NUM, dat, dat, 2);

    *val = dat[1];
    SPI_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ICM20602 SPI���ֽڶ��Ĵ���
// @param		cmd				�Ĵ�����ַ
// @param		*val			�������ݵĵ�ַ
// @param		num				��ȡ����
// @return		void
// @since		v1.0
// Sample usage:
// @note		�ڲ����� �û��������
//-------------------------------------------------------------------------------------------------------------------
static void icm_spi_r_reg_bytes(uint8 *val, uint8 num)
{
    SPI_CS(0);
    spi_mosi(SPI_NUM, val, val, num);
    SPI_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ICM20602�Լ캯��
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:
// @note		�ڲ����� �û��������
//-------------------------------------------------------------------------------------------------------------------
static void icm20602_self3_check(void)
{
    uint8 dat = 0;

    while(0x12 != dat)																// �ж� ID �Ƿ���ȷ
    {
        icm_spi_r_reg_byte(ICM20602_WHO_AM_I, &dat);								// ��ȡICM20602 ID
        systick_delay_ms(10);
        //��������ԭ�������¼���
        //1 ICM20602���ˣ�������µ������ĸ��ʼ���
        //2 ���ߴ������û�нӺ�
        //3 ��������Ҫ����������裬������3.3V
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		��ʼ��ICM20602
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void icm20602_init_spi(void)
{
    uint8 val = 0x0;

    systick_delay_ms(10);  //�ϵ���ʱ

    spi_init(SPI_NUM, SPI_SCK_PIN, SPI_MOSI_PIN, SPI_MISO_PIN, SPI_NSS_NULL, 0, SystemCoreClock / 4);	// Ӳ��SPI��ʼ��

    gpio_init(SPI_CS_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);

    icm20602_self3_check();//���

    icm_spi_w_reg_byte(ICM20602_PWR_MGMT_1, 0x80); //��λ�豸
    systick_delay_ms(2);
    do																				// �ȴ���λ�ɹ�
    {
        icm_spi_r_reg_byte(ICM20602_PWR_MGMT_1, &val);
    }
    while(0x41 != val);

    icm_spi_w_reg_byte(ICM20602_PWR_MGMT_1,		0x01);								// ʱ������
    icm_spi_w_reg_byte(ICM20602_PWR_MGMT_2,		0x00);								// ���������Ǻͼ��ٶȼ�
    icm_spi_w_reg_byte(ICM20602_CONFIG,			0x01);								// 176HZ 1KHZ
    icm_spi_w_reg_byte(ICM20602_SMPLRT_DIV,		0x07);								// �������� SAMPLE_RATE = INTERNAL_SAMPLE_RATE / (1 + SMPLRT_DIV)
    icm_spi_w_reg_byte(ICM20602_GYRO_CONFIG,	0x18);								// ��2000 dps
    icm_spi_w_reg_byte(ICM20602_ACCEL_CONFIG,	0x10);								// ��8g
    icm_spi_w_reg_byte(ICM20602_ACCEL_CONFIG_2,	0x03);								// Average 4 samples   44.8HZ   //0x23 Average 16 samples
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		��ȡICM20602���ٶȼ�����
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:				ִ�иú�����ֱ�Ӳ鿴��Ӧ�ı�������
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
    IMC_State.accdata.x = (int16)(((uint16)buf.dat[0] << 8 | buf.dat[1]));
    IMC_State.accdata.y = (int16)(((uint16)buf.dat[2] << 8 | buf.dat[3]));
    IMC_State.accdata.z = (int16)(((uint16)buf.dat[4] << 8 | buf.dat[5]));
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		��ȡICM20602����������
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:				ִ�иú�����ֱ�Ӳ鿴��Ӧ�ı�������
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
    IMC_State.gyro.x = (int16)(((uint16)buf.dat[0] << 8 | buf.dat[1]));
    IMC_State.gyro.y = (int16)(((uint16)buf.dat[2] << 8 | buf.dat[3]));
    IMC_State.gyro.z = (int16)(((uint16)buf.dat[4] << 8 | buf.dat[5]));
    if(ICM20602_Offset_Finished)
    {
        IMC_State.gyro.x = IMC_State.gyro.x - IMC_Offset.gyro.x;
        IMC_State.gyro.y =  IMC_State.gyro.y - IMC_Offset.gyro.y;
        IMC_State.gyro.z = IMC_State.gyro.z - IMC_Offset.gyro.z;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// ���º�����ʹ��Ӳ��SPIͨ�� ��Ƚ�IIC �ٶȱ�IIC��ǳ���
//-------------------------------------------------------------------------------------------------------------------



/***************************************************************
@Fucation ��ICM20602��ƫ
@Author �� bc
@Data �� 2019/2/1
@Note �� ��ʼ������
***************************************************************/

void ICM20602_Offset(void)
{
    uint8 i, Count = 100;
    int64 temp[6] = {0};

    for (i = 0; i < Count; i++)
    {
        get_icm20602_accdata_spi();//���ٶȼ�
        get_icm20602_gyro_spi();//������
        systick_delay_ms(2);

        temp[0] += IMC_State.accdata.x;
        temp[1] += IMC_State.accdata.y;
        temp[2] += IMC_State.accdata.z;

        temp[3] += IMC_State.gyro.x;
        temp[4] += IMC_State.gyro.y;
        temp[5] += IMC_State.gyro.z;
    }
    IMC_Offset.accdata.x = temp[0] / Count;
    IMC_Offset.accdata.y = temp[1] / Count;
    IMC_Offset.accdata.z = temp[2] / Count;

    IMC_Offset.gyro.x = temp[3] / Count;
    IMC_Offset.gyro.y = temp[4] / Count;
    IMC_Offset.gyro.z = temp[5] / Count;
    ICM20602_Offset_Finished = 1;
}

/***************************************************************
@Fucation �� ���ݻ����˲�
@Author �� bc
@Data �� 2018/2/1
@Note ��
***************************************************************/
#define AcceRatio   4096.0f
#define GyroRatio   16.4f
#define ACC_FILTER_NUM 5		// ���ٶȼ��˲����
#define GYRO_FILTER_NUM 1		// �������˲����

int32 ACC_X_BUF[ACC_FILTER_NUM], ACC_Y_BUF[ACC_FILTER_NUM], ACC_Z_BUF[ACC_FILTER_NUM];	// �˲���������
int32 GYRO_X_BUF[GYRO_FILTER_NUM], GYRO_Y_BUF[GYRO_FILTER_NUM], GYRO_Z_BUF[GYRO_FILTER_NUM];


void Data_Filter(void)	// �����˲�
{
    int i;
    int64 temp1 = 0, temp2 = 0, temp3 = 0, temp4 = 0, temp5 = 0, temp6 = 0;

    ACC_X_BUF[0] = IMC_State.accdata.x;	// ���»�����������
    ACC_Y_BUF[0] = IMC_State.accdata.y;
    ACC_Z_BUF[0] = IMC_State.accdata.z;
    GYRO_X_BUF[0] = IMC_State.gyro.x;
    GYRO_Y_BUF[0] = IMC_State.gyro.y;
    GYRO_Z_BUF[0] = IMC_State.gyro.z;

    for(i = 0; i < ACC_FILTER_NUM; i++)
    {
        temp1 += ACC_X_BUF[i];
        temp2 += ACC_Y_BUF[i];
        temp3 += ACC_Z_BUF[i];

    }
    for(i = 0; i < GYRO_FILTER_NUM; i++)
    {
        temp4 += GYRO_X_BUF[i];
        temp5 += GYRO_Y_BUF[i];
        temp6 += GYRO_Z_BUF[i];
    }

    IMC_Treated.accdata.x = temp1 / ACC_FILTER_NUM / AcceRatio;
    IMC_Treated.accdata.y = temp2 / ACC_FILTER_NUM / AcceRatio;
    IMC_Treated.accdata.z = temp3 / ACC_FILTER_NUM / AcceRatio;
    IMC_Treated.gyro.x = temp4 / GYRO_FILTER_NUM / GyroRatio;
    IMC_Treated.gyro.y = temp5 / GYRO_FILTER_NUM / GyroRatio;
    IMC_Treated.gyro.z = temp6 / GYRO_FILTER_NUM / GyroRatio;

    for(i = 0; i < ACC_FILTER_NUM - 1; i++)
    {
        ACC_X_BUF[ACC_FILTER_NUM - 1 - i] = ACC_X_BUF[ACC_FILTER_NUM - 2 - i];
        ACC_Y_BUF[ACC_FILTER_NUM - 1 - i] = ACC_Y_BUF[ACC_FILTER_NUM - 2 - i];
        ACC_Z_BUF[ACC_FILTER_NUM - 1 - i] = ACC_Z_BUF[ACC_FILTER_NUM - 2 - i];

    }
    for(i = 0; i < GYRO_FILTER_NUM - 1; i++)
    {
        GYRO_X_BUF[GYRO_FILTER_NUM - 1 - i] = GYRO_X_BUF[GYRO_FILTER_NUM - 2 - i];
        GYRO_Y_BUF[GYRO_FILTER_NUM - 1 - i] = GYRO_Y_BUF[GYRO_FILTER_NUM - 2 - i];
        GYRO_Z_BUF[GYRO_FILTER_NUM - 1 - i] = GYRO_Z_BUF[GYRO_FILTER_NUM - 2 - i];
    }
}

/***************************************************************
@Fucation �� �Ǿ��󿨶����˲�����Щ�������ø�
@Author �� bc
@Data �� 2018/2/1
@Note ��
***************************************************************/
#define Peried 1/500.0f		//��������������
#define Q 3.0f			//��������2.0		ԽС����Խ�������ټ��ٶȼ�Խ��Խƽ��
#define R 1500.0f		//��������5000.0	ԽС���ټ��ٶȼ�Խ��
float KalmanGain = 1.0f;	//����������

void KalmanFilter(float ACC_Angle, float Gyro_y)
{
    //�������˲��ֲ�����
    static float Priori_Estimation = 0;//�������
    static float Posterior_Estimation = 0;//�������
    static float Priori_Convariance = 0;//���鷽��
    static float Posterior_Convariance = 0;//���鷽��

    //�������˲�
    //1.ʱ�����(Ԥ��) : X(k|k-1) = A(k,k-1)*X(k-1|k-1) + B(k)*u(k)
    Priori_Estimation = Posterior_Estimation - Gyro_y * Peried;		//������ƣ����ֻ�ýǶ�
    if(Priori_Estimation != Priori_Estimation)
    {
        Priori_Estimation = 0;
    }

    //2.��������Э����  : P(k|k-1) = A(k,k-1)*P(k-1|k-1)*A(k,k-1)'+Q(k)
    Priori_Convariance = (float)sqrt( Posterior_Convariance * Posterior_Convariance + Q * Q );
    if(Priori_Convariance != Priori_Convariance)
    {
        Priori_Convariance = 0;
    }

    //  ������������ƣ���������
    // 1.���㿨��������  : K(k) = P(k|k-1)*H(k)' / (H(k)*P(k|k-1)*H(k)' + R(k)) /
    KalmanGain = (float)sqrt( Priori_Convariance * Priori_Convariance / ( Priori_Convariance * Priori_Convariance + R * R ) );
    if(KalmanGain != KalmanGain)
    {
        KalmanGain = 1;
    }

    //2.��������(У��): X(k|k) = X(k|k-1)+K(k)*(Z(k)-H(k)*X(k|k-1))
    Posterior_Estimation  = Priori_Estimation + KalmanGain * (ACC_Angle - Priori_Estimation );
    if(Posterior_Estimation != Posterior_Estimation)
    {
        Posterior_Estimation = 0;
    }

    // 3.���º���Э����  : P(k|k) =��I-K(k)*H(k)��*P(k|k-1)
    Posterior_Convariance = (float)sqrt(( 1 - KalmanGain ) * Priori_Convariance * Priori_Convariance );
    if(Posterior_Convariance != Posterior_Convariance)
    {
        Posterior_Convariance = 0;
    }

    //�õ����սǶ�
    Attitude_Angel_P = Posterior_Estimation;

    if(Attitude_Angel_P != Attitude_Angel_P)
    {
        Attitude_Angel_P = 1;
    }
}

void Get_Attitude(void)
{
    KalmanFilter(IMC_Treated.accdata.x * 90 , IMC_Treated.gyro.x);
}

