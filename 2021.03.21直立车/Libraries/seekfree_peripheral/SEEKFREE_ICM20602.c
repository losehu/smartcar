/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				SEEKFREE_ICM20602
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.24
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
* @note
* 					接线定义：
* 					------------------------------------
* 					模块管脚			单片机管脚
* 					SCL					查看SEEKFREE_IIC文件内的SEEKFREE_SCL宏定义
* 					SDA					查看SEEKFREE_IIC文件内的SEEKFREE_SDA宏定义
* 					------------------------------------
********************************************************************************************************************/

#include "zf_systick.h"
#include "zf_gpio.h"
#include "zf_spi.h"
#include "SEEKFREE_IIC.h"
#include "SEEKFREE_ICM20602.h"

#include "Balance.h"
int ICM20602_Offset_Finished = 0; //零漂计算完成标志位

int16 icm_gyro_x, icm_gyro_y, icm_gyro_z;
int16 icm_acc_x, icm_acc_y, icm_acc_z;

ICM20602 IMC_State; //当前返回值
ICM20602 IMC_Offset;//零漂值
ICM20602_Treated IMC_Treated; //经过滑动处理的数据



//-------------------------------------------------------------------------------------------------------------------
// 以下函数是使用软件IIC通信
//-------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------
// @brief		ICM20602自检函数
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:
// @note		内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void icm20602_self1_check(void)
{
    uint8 dat = 0;
    while(0x12 != dat)																// 判断 ID 是否正确
    {
        dat = simiic_read_reg(ICM20602_DEV_ADDR, ICM20602_WHO_AM_I, SIMIIC);			// 读取ICM20602 ID
        systick_delay_ms(10);
        //卡在这里原因有以下几点
        //1 ICM20602坏了，如果是新的这样的概率极低
        //2 接线错误或者没有接好
        //3 可能你需要外接上拉电阻，上拉到3.3V
        //4 可能没有调用模拟IIC的初始化函数
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		初始化ICM20602
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void icm20602_init(void)
{
    simiic_init();
    systick_delay_ms(10);  //上电延时

    //检测
    icm20602_self1_check();

    //复位
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_PWR_MGMT_1, 0x80);					// 复位设备
    systick_delay_ms(2);															// 延时
    while(0x80 & simiic_read_reg(ICM20602_DEV_ADDR, ICM20602_PWR_MGMT_1, SIMIIC));	// 等待复位完成

    //配置参数
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_PWR_MGMT_1, 0x01);					// 时钟设置
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_PWR_MGMT_2, 0x00);					// 开启陀螺仪和加速度计
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_CONFIG, 0x01);						// 176HZ 1KHZ
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_SMPLRT_DIV, 0x07);					// 采样速率 SAMPLE_RATE = INTERNAL_SAMPLE_RATE / (1 + SMPLRT_DIV)
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_GYRO_CONFIG, 0x18);					// ±2000 dps
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_ACCEL_CONFIG, 0x10);					// ±8g
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_ACCEL_CONFIG_2, 0x03);				// Average 4 samples   44.8HZ   //0x23 Average 16 samples
}


//-------------------------------------------------------------------------------------------------------------------
// @brief		获取ICM20602加速度计数据
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:				执行该函数后，直接查看对应的变量即可
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
// @brief		获取ICM20602陀螺仪数据
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:				执行该函数后，直接查看对应的变量即可
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
// 以上函数是使用软件IIC通信
//-------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------
// 以下函数是使用硬件SPI通信 相比较IIC 速度比IIC快非常多
//-------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------
// @brief		ICM20602 SPI写寄存器
// @param		cmd				寄存器地址
// @param		val				需要写入的数据
// @return		void
// @since		v1.0
// Sample usage:
// @note		内部调用 用户无需关心
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
// @brief		ICM20602 SPI读寄存器
// @param		cmd				寄存器地址
// @param		*val			接收数据的地址
// @return		void
// @since		v1.0
// Sample usage:
// @note		内部调用 用户无需关心
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
// @brief		ICM20602 SPI多字节读寄存器
// @param		cmd				寄存器地址
// @param		*val			接收数据的地址
// @param		num				读取数量
// @return		void
// @since		v1.0
// Sample usage:
// @note		内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void icm_spi_r_reg_bytes(uint8 *val, uint8 num)
{
    SPI_CS(0);
    spi_mosi(SPI_NUM, val, val, num);
    SPI_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ICM20602自检函数
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:
// @note		内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void icm20602_self3_check(void)
{
    uint8 dat = 0;

    while(0x12 != dat)																// 判断 ID 是否正确
    {
        icm_spi_r_reg_byte(ICM20602_WHO_AM_I, &dat);								// 读取ICM20602 ID
        systick_delay_ms(10);
        //卡在这里原因有以下几点
        //1 ICM20602坏了，如果是新的这样的概率极低
        //2 接线错误或者没有接好
        //3 可能你需要外接上拉电阻，上拉到3.3V
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		初始化ICM20602
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void icm20602_init_spi(void)
{
    uint8 val = 0x0;

    systick_delay_ms(10);  //上电延时

    spi_init(SPI_NUM, SPI_SCK_PIN, SPI_MOSI_PIN, SPI_MISO_PIN, SPI_NSS_NULL, 0, SystemCoreClock / 4);	// 硬件SPI初始化

    gpio_init(SPI_CS_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);

    icm20602_self3_check();//检测

    icm_spi_w_reg_byte(ICM20602_PWR_MGMT_1, 0x80); //复位设备
    systick_delay_ms(2);
    do																				// 等待复位成功
    {
        icm_spi_r_reg_byte(ICM20602_PWR_MGMT_1, &val);
    }
    while(0x41 != val);

    icm_spi_w_reg_byte(ICM20602_PWR_MGMT_1,		0x01);								// 时钟设置
    icm_spi_w_reg_byte(ICM20602_PWR_MGMT_2,		0x00);								// 开启陀螺仪和加速度计
    icm_spi_w_reg_byte(ICM20602_CONFIG,			0x01);								// 176HZ 1KHZ
    icm_spi_w_reg_byte(ICM20602_SMPLRT_DIV,		0x07);								// 采样速率 SAMPLE_RATE = INTERNAL_SAMPLE_RATE / (1 + SMPLRT_DIV)
    icm_spi_w_reg_byte(ICM20602_GYRO_CONFIG,	0x18);								// ±2000 dps
    icm_spi_w_reg_byte(ICM20602_ACCEL_CONFIG,	0x10);								// ±8g
    icm_spi_w_reg_byte(ICM20602_ACCEL_CONFIG_2,	0x03);								// Average 4 samples   44.8HZ   //0x23 Average 16 samples
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		获取ICM20602加速度计数据
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:				执行该函数后，直接查看对应的变量即可
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
// @brief		获取ICM20602陀螺仪数据
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:				执行该函数后，直接查看对应的变量即可
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
// 以下函数是使用硬件SPI通信 相比较IIC 速度比IIC快非常多
//-------------------------------------------------------------------------------------------------------------------



/***************************************************************
@Fucation ：ICM20602零偏
@Author ： bc
@Data ： 2019/2/1
@Note ： 初始化调用
***************************************************************/

void ICM20602_Offset(void)
{
    uint8 i, Count = 100;
    int64 temp[6] = {0};

    for (i = 0; i < Count; i++)
    {
        get_icm20602_accdata_spi();//加速度计
        get_icm20602_gyro_spi();//陀螺仪
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
@Fucation ： 数据滑动滤波
@Author ： bc
@Data ： 2018/2/1
@Note ：
***************************************************************/
#define AcceRatio   4096.0f
#define GyroRatio   16.4f
#define ACC_FILTER_NUM 5		// 加速度计滤波深度
#define GYRO_FILTER_NUM 1		// 陀螺仪滤波深度

int32 ACC_X_BUF[ACC_FILTER_NUM], ACC_Y_BUF[ACC_FILTER_NUM], ACC_Z_BUF[ACC_FILTER_NUM];	// 滤波缓存数组
int32 GYRO_X_BUF[GYRO_FILTER_NUM], GYRO_Y_BUF[GYRO_FILTER_NUM], GYRO_Z_BUF[GYRO_FILTER_NUM];


void Data_Filter(void)	// 数据滤波
{
    int i;
    int64 temp1 = 0, temp2 = 0, temp3 = 0, temp4 = 0, temp5 = 0, temp6 = 0;

    ACC_X_BUF[0] = IMC_State.accdata.x;	// 更新滑动窗口数组
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
@Fucation ： 非矩阵卡尔曼滤波，这些参数不用改
@Author ： bc
@Data ： 2018/2/1
@Note ：
***************************************************************/
#define Peried 1/500.0f		//卡尔曼积分周期
#define Q 3.0f			//过程噪声2.0		越小积分越慢，跟踪加速度计越慢越平滑
#define R 1500.0f		//测量噪声5000.0	越小跟踪加速度计越快
float KalmanGain = 1.0f;	//卡尔曼增益

void KalmanFilter(float ACC_Angle, float Gyro_y)
{
    //卡尔曼滤波局部参量
    static float Priori_Estimation = 0;//先验估计
    static float Posterior_Estimation = 0;//后验估计
    static float Priori_Convariance = 0;//先验方差
    static float Posterior_Convariance = 0;//后验方差

    //卡尔曼滤波
    //1.时间更新(预测) : X(k|k-1) = A(k,k-1)*X(k-1|k-1) + B(k)*u(k)
    Priori_Estimation = Posterior_Estimation - Gyro_y * Peried;		//先验估计，积分获得角度
    if(Priori_Estimation != Priori_Estimation)
    {
        Priori_Estimation = 0;
    }

    //2.更新先验协方差  : P(k|k-1) = A(k,k-1)*P(k-1|k-1)*A(k,k-1)'+Q(k)
    Priori_Convariance = (float)sqrt( Posterior_Convariance * Posterior_Convariance + Q * Q );
    if(Priori_Convariance != Priori_Convariance)
    {
        Priori_Convariance = 0;
    }

    //  卡尔曼后验估计：测量更新
    // 1.计算卡尔曼增益  : K(k) = P(k|k-1)*H(k)' / (H(k)*P(k|k-1)*H(k)' + R(k)) /
    KalmanGain = (float)sqrt( Priori_Convariance * Priori_Convariance / ( Priori_Convariance * Priori_Convariance + R * R ) );
    if(KalmanGain != KalmanGain)
    {
        KalmanGain = 1;
    }

    //2.测量更新(校正): X(k|k) = X(k|k-1)+K(k)*(Z(k)-H(k)*X(k|k-1))
    Posterior_Estimation  = Priori_Estimation + KalmanGain * (ACC_Angle - Priori_Estimation );
    if(Posterior_Estimation != Posterior_Estimation)
    {
        Posterior_Estimation = 0;
    }

    // 3.更新后验协方差  : P(k|k) =（I-K(k)*H(k)）*P(k|k-1)
    Posterior_Convariance = (float)sqrt(( 1 - KalmanGain ) * Priori_Convariance * Priori_Convariance );
    if(Posterior_Convariance != Posterior_Convariance)
    {
        Posterior_Convariance = 0;
    }

    //得到最终角度
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

