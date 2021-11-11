/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             SEEKFREE_ICM20602
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ3184284598)
* @version          查看doc内version文件 版本说明
* @Software         IAR 8.3 or MDK 5.24
* @Taobao           https://seekfree.taobao.com/
* @date             2020-11-23
* @note
*                   接线定义：
*                   ------------------------------------
*                   模块管脚            单片机管脚
*                   SCL                 查看SEEKFREE_IIC文件内的SEEKFREE_SCL宏定义
*                   SDA                 查看SEEKFREE_IIC文件内的SEEKFREE_SDA宏定义
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
extern int  Direct_Parameter;// 转向系数;

int16 icm_gyro_x, icm_gyro_y, icm_gyro_z;
int16 icm_acc_x, icm_acc_y, icm_acc_z;
//-------------------------------------------------------------------------------------------------------------------
// 以下函数是使用软件IIC通信
//-------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------
// @brief       ICM20602自检函数
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:
// @note        内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void icm20602_self1_check(void)
{
    uint8 dat = 0;
    while (0x12 != dat)                                                             // 判断 ID 是否正确
    {
        dat = simiic_read_reg(ICM20602_DEV_ADDR, ICM20602_WHO_AM_I, SIMIIC);        // 读取ICM20602 ID
        systick_delay_ms(10);
        //卡在这里原因有以下几点
        //1 ICM20602坏了，如果是新的这样的概率极低
        //2 接线错误或者没有接好
        //3 可能你需要外接上拉电阻，上拉到3.3V
        //4 可能没有调用模拟IIC的初始化函数
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       初始化ICM20602
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void icm20602_init(void)
{
    simiic_init();
    systick_delay_ms(10);  //上电延时

    //检测
    icm20602_self1_check();

    //复位
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_PWR_MGMT_1, 0x80);                 // 复位设备
    systick_delay_ms(2);                                                            // 延时
    while (0x80 & simiic_read_reg(ICM20602_DEV_ADDR, ICM20602_PWR_MGMT_1, SIMIIC)); // 等待复位完成

    //配置参数
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_PWR_MGMT_1, 0x01);                 // 时钟设置
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_PWR_MGMT_2, 0x00);                 // 开启陀螺仪和加速度计
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_CONFIG, 0x01);                     // 176HZ 1KHZ
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_SMPLRT_DIV, 0x07);                 // 采样速率 SAMPLE_RATE = INTERNAL_SAMPLE_RATE / (1 + SMPLRT_DIV)
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_GYRO_CONFIG, 0x18);                // ±2000 dps
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_ACCEL_CONFIG, 0x10);               // ±8g
    simiic_write_reg(ICM20602_DEV_ADDR, ICM20602_ACCEL_CONFIG_2, 0x03);             // Average 4 samples   44.8HZ   //0x23 Average 16 samples
}


//-------------------------------------------------------------------------------------------------------------------
// @brief       获取ICM20602加速度计数据
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:                执行该函数后，直接查看对应的变量即可
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
// @brief       获取ICM20602陀螺仪数据
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:                执行该函数后，直接查看对应的变量即可
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
// @brief       ICM20602 SPI写寄存器
// @param       cmd             寄存器地址
// @param       val             需要写入的数据
// @return      void
// @since       v1.0
// Sample usage:
// @note        内部调用 用户无需关心
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
// @brief       ICM20602 SPI读寄存器
// @param       cmd             寄存器地址
// @param       *val            接收数据的地址
// @return      void
// @since       v1.0
// Sample usage:
// @note        内部调用 用户无需关心
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
// @brief       ICM20602 SPI多字节读寄存器
// @param       cmd             寄存器地址
// @param       *val            接收数据的地址
// @param       num             读取数量
// @return      void
// @since       v1.0
// Sample usage:
// @note        内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void icm_spi_r_reg_bytes(uint8 *val, uint8 num)
{
    ICM20602_CS(0);
    spi_mosi(ICM20602_SPI, val, val, num);
    ICM20602_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ICM20602自检函数
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:
// @note        内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void icm20602_self3_check(void)
{
    uint8 dat = 0;

    while (0x12 != dat)                                                             // 判断 ID 是否正确
    {
        icm_spi_r_reg_byte(ICM20602_WHO_AM_I, &dat);                                // 读取ICM20602 ID
        systick_delay_ms(10);
        //卡在这里原因有以下几点
        //1 ICM20602坏了，如果是新的这样的概率极低
        //2 接线错误或者没有接好
        //3 可能你需要外接上拉电阻，上拉到3.3V
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       初始化ICM20602
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void icm20602_init_spi(void)
{
    uint8 val = 0x0;

    systick_delay_ms(10);  //上电延时

    spi_init(ICM20602_SPI, ICM20602_SCK_PIN, ICM20602_MOSI_PIN, ICM20602_MISO_PIN, SPI_NSS_NULL, 0, SystemCoreClock / 4); // 硬件SPI初始化

    gpio_init(ICM20602_CS_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);

    icm20602_self3_check();//检测

    icm_spi_w_reg_byte(ICM20602_PWR_MGMT_1, 0x80); //复位设备
    systick_delay_ms(2);
    do                                                                              // 等待复位成功
    {
        icm_spi_r_reg_byte(ICM20602_PWR_MGMT_1, &val);
    }
    while (0x41 != val);

    icm_spi_w_reg_byte(ICM20602_PWR_MGMT_1,     0x01);                              // 时钟设置
    icm_spi_w_reg_byte(ICM20602_PWR_MGMT_2,     0x00);                              // 开启陀螺仪和加速度计
    icm_spi_w_reg_byte(ICM20602_CONFIG,         0x01);                              // 176HZ 1KHZ
    icm_spi_w_reg_byte(ICM20602_SMPLRT_DIV,     0x07);                              // 采样速率 SAMPLE_RATE = INTERNAL_SAMPLE_RATE / (1 + SMPLRT_DIV)
    icm_spi_w_reg_byte(ICM20602_GYRO_CONFIG,    0x18);                              // ±2000 dps
    icm_spi_w_reg_byte(ICM20602_ACCEL_CONFIG,   0x10);                              // ±8g
    icm_spi_w_reg_byte(ICM20602_ACCEL_CONFIG_2, 0x03);                              // Average 4 samples   44.8HZ   //0x23 Average 16 samples
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       获取ICM20602加速度计数据
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:                执行该函数后，直接查看对应的变量即可
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
// @brief       获取ICM20602陀螺仪数据
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:                执行该函数后，直接查看对应的变量即可
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
// 以下函数是使用硬件SPI通信 相比较IIC 速度比IIC快非常多
//-------------------------------------------------------------------------------------------------------------------
//------------------------------私有库------------------------------------//
ICM20602 ICM_State; //当前返回值
ICM20602 ICM_Offset;//零漂值
ICM20602_Treated ICM_Treated; //经过滑动处理的数据
uint8 ICM20602_Offset_Finished = 0;
ICM20602_IntegrationTypedef ICMIntegrate;

/***************************************************************
@Fucation ：ICM20602零偏
@Author   ： bc
@Data     ： 2019/2/1
@Note     ： 初始化调用
***************************************************************/

bool ICM20602_Offset(void)
{
    uint8 i, j, Count = 100;
    int64 temp[6] = {0};

    //设置积分标志
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
        get_icm20602_accdata_spi();//加速度计
        get_icm20602_gyro_spi();//陀螺仪
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

    //计算角度并且滤波 检验陀螺仪的准确性
    for (i = 0; i < 100; i++)
    {
        for (j = 0; j < 3; j++)
        {
            get_icm20602_accdata_spi();
            get_icm20602_gyro_spi();
            Data_Filter();                  // 对原始数据滑动滤波
        }
    }
    return true;
}

/***************************************************************
@Fucation ： 数据滑动滤波
@Author ： bc
@Data ： 2018/2/1
@Note ：
***************************************************************/
#define AcceRatio   16384.0f
#define GyroRatio   16.384f     //计算后的单位为 °/s
#define ACC_FILTER_NUM 5        // 加速度计滤波深度
#define GYRO_FILTER_NUM 20      // 陀螺仪滤波深度

int32 ACC_X_BUF[ACC_FILTER_NUM], ACC_Y_BUF[ACC_FILTER_NUM], ACC_Z_BUF[ACC_FILTER_NUM];  // 滤波缓存数组
int32 GYRO_X_BUF[GYRO_FILTER_NUM], GYRO_Y_BUF[GYRO_FILTER_NUM], GYRO_Z_BUF[GYRO_FILTER_NUM];


void Data_Filter(void)  // 数据滤波
{
    int i;
    int64 temp1 = 0, temp2 = 0, temp3 = 0, temp4 = 0, temp5 = 0, temp6 = 0;

    get_icm20602_accdata_spi();
    get_icm20602_gyro_spi();

    ACC_X_BUF[0] = ICM_State.accdata.x; // 更新滑动窗口数组
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
//显示陀螺仪加速度计数值以及总共的陀螺仪Z积分
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


//===============================四元数============================================
#define Kp 25.0f //  1.2            // proportional gain governs rate of convergence to accelerometer/magnetometer
#define Ki 0.02f//0.001     // integral gain governs rate of convergence of gyroscope biases
#define halfT 0.003f  //0.003                   // half the sample period采样周期的一半
float q0 = 1, q1 = 0, q2 = 0, q3 = 0;   // quaternion elements representing the estimated orientation
float exInt = 0, eyInt = 0, ezInt = 0;  // scaled integral error
/*
 * 函数名：IMUupdate
 * 描述  ：四元素解算欧拉角
 * 输入  ：陀螺仪 加速度计
 * 输出  ：无
 * 调用  ：内部调用
 */
void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az)
{
    float norm;
    float vx, vy, vz;
    float ex, ey, ez;

    // 先把这些用得到的值算好
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

    norm = sqrt(ax * ax + ay * ay + az * az); // acc数据归一化
    ax = ax / norm;
    ay = ay / norm;
    az = az / norm;

    // estimated direction of gravity and flux (v and w)    估计重力方向和流量/变迁
    vx = 2 * (q1q3 - q0q2);                                 // 四元素中xyz的表示
    vy = 2 * (q0q1 + q2q3);
    vz = q0q0 - q1q1 - q2q2 + q3q3 ;

    // error is sum of cross product between reference direction of fields and direction measured by sensors
    ex = (ay * vz - az * vy) ;  // 向量外积在相减得到差分就是误差
    ey = (az * vx - ax * vz) ;
    ez = (ax * vy - ay * vx) ;

    exInt = exInt + ex * Ki;    // 对误差进行积分
    eyInt = eyInt + ey * Ki;
    ezInt = ezInt + ez * Ki;

    // adjusted gyroscope measurements
    gx = gx + Kp * ex + exInt;  // 将误差PI后补偿到陀螺仪，即补偿零点漂移
    gy = gy + Kp * ey + eyInt;
    gz = gz + Kp * ez + ezInt;  // 这里的gz由于没有观测者进行矫正会产生漂移，表现出来的就是积分自增或自减

    // integrate quaternion rate and normalise  // 四元素的微分方程
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

