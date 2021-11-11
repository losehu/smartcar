/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             SEEKFREE_7725
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
*                   SDA                 A9
*                   SCL                 A8
*                   场中断(VSY)         A0
*                   行中断(HREF)        未使用，因此不接
*                   像素中断(PCLK)      A1
*                   数据口(D0-D7)       B24-B31
*                   ------------------------------------
********************************************************************************************************************/

#include "zf_exti.h"
#include "zf_systick.h"
#include "zf_uart.h"
#include "zf_gpio.h"
#include "zf_camera.h"
#include "SEEKFREE_FONT.h"
#include "SEEKFREE_7725.h"
#include "SEEKFREE_IPS114_SPI.h"
#include "Control.h"


//配置摄像头帧率
#define OV7725_FPS   150//可选参数 50 75 100 112 150


uint8_t ov7725_finish_flag = 0;
uint8_t ov7725_image_bin0[OV7725_H][OV7725_W / 8];
uint8_t ov7725_image_bin1[OV7725_H][OV7725_W / 8];
static uint16 ov7725_simiic_delay_time = 200;                                       // 如果初始化异常请增大延时数值
uint8   ov7725_threshold = 40;
//-------------------------------------------------------------------------------------------------------------------
// @brief       模拟IIC延时 时间设置
// @return      void
// Sample usage:
// @note        内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static void ov7725_simiic_delay(void)
{
    uint16 delay_time;
    delay_time = ov7725_simiic_delay_time;
    while (delay_time--);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       模拟IIC起始信号
// @return      void
// Sample usage:
// @note        内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static void ov7725_simiic_start(void)
{
    OV7725_SDA1();
    OV7725_SCL1();
    ov7725_simiic_delay();
    OV7725_SDA0();
    ov7725_simiic_delay();
    OV7725_SCL0();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       模拟IIC停止信号
// @return      void
// Sample usage:
// @note        内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static void ov7725_simiic_stop(void)
{
    OV7725_SDA0();
    OV7725_SCL0();
    ov7725_simiic_delay();
    OV7725_SCL1();
    ov7725_simiic_delay();
    OV7725_SDA1();
    ov7725_simiic_delay();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       模拟IIC ACK信号
// @param       ack_dat         主应答 (包含ack:SDA=0和no_ack:SDA=0)
// @return      void
// Sample usage:
// @note        内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static void ov7725_simiic_sendack(unsigned char ack_dat)
{
    OV7725_SCL0();
    ov7725_simiic_delay();
    if (ack_dat)
        OV7725_SDA0();
    else
        OV7725_SDA1();

    OV7725_SCL1();
    ov7725_simiic_delay();
    OV7725_SCL0();
    ov7725_simiic_delay();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       模拟IIC等待应答
// @return      void
// Sample usage:
// @note        内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static int ov7725_sccb_waitack(void)
{
    OV7725_SCL0();
    //  OV7725_DIR_IN();
    ov7725_simiic_delay();

    OV7725_SCL1();
    ov7725_simiic_delay();

    if (OV7725_SDA)                                                                 // 应答为高电平，异常，通信失败
    {
        //      OV7725_DIR_OUT();
        OV7725_SCL0();
        return 0;
    }
    //  OV7725_DIR_OUT();
    OV7725_SCL0();
    ov7725_simiic_delay();
    return 1;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       模拟IIC 字节发送 送完后接收从应答 不考虑从应答位
// @param       c               发送c(可以是数据也可是地址)
// @return      void
// Sample usage:
// @note        内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static void ov7725_send_ch(uint8 c)
{
    uint8 i = 8;
    while (i--)
    {
        if (c & 0x80)
            OV7725_SDA1();                                                          // SDA 输出数据
        else
            OV7725_SDA0();
        c <<= 1;
        ov7725_simiic_delay();
        OV7725_SCL1();                                                              // SCL 拉高，采集信号
        ov7725_simiic_delay();
        OV7725_SCL0();                                                              // SCL 时钟线拉低
    }
    ov7725_sccb_waitack();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       模拟IIC 字节接收 此程序应配合|主应答函数|使用
// @param       ack_x           应答
// @return      uint8           数据
// Sample usage:
// @note        内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static uint8 ov7725_read_ch(uint8 ack_x)
{
    uint8 i;
    uint8 c;
    c = 0;
    OV7725_SCL0();
    ov7725_simiic_delay();
    OV7725_SDA1();
    //  OV7725_DIR_IN();                                                                // 置数据线为输入方式
    for (i = 0; i < 8; i++)
    {
        ov7725_simiic_delay();
        OV7725_SCL0();                                                              // 置时钟线为低，准备接收数据位
        ov7725_simiic_delay();
        OV7725_SCL1();                                                              // 置时钟线为高，使数据线上数据有效
        ov7725_simiic_delay();
        c <<= 1;
        if (OV7725_SDA)
            c += 1;                                                                 // 读数据位，将接收的数据存c
    }
    //  OV7725_DIR_OUT();
    OV7725_SCL0();
    ov7725_simiic_delay();
    ov7725_simiic_sendack(ack_x);

    return c;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       模拟IIC 写数据到设备寄存器
// @param       dev_add         设备地址
// @param       reg             寄存器地址
// @param       dat             写入的数据
// @return      void
// Sample usage:
// @note        内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
void ov7725_simiic_write_reg(uint8 dev_add, uint8 reg, uint8 dat)
{
    ov7725_simiic_start();
    ov7725_send_ch((dev_add << 1) | 0x00);                                           // 发送器件地址加写位
    ov7725_send_ch(reg);                                                             // 发送从机寄存器地址
    ov7725_send_ch(dat);                                                             // 发送需要写入的数据
    ov7725_simiic_stop();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       模拟IIC 从设备寄存器读取数据
// @param       dev_add         设备地址
// @param       reg             寄存器地址
// @param       type            选择通信方式是IIC  还是 SCCB
// @return      uint8           返回寄存器的数据
// Sample usage:
// @note        内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static uint8 ov7725_simiic_read_reg(uint8 dev_add, uint8 reg, IIC_type type)
{
    uint8 dat;
    ov7725_simiic_start();
    ov7725_send_ch((dev_add << 1) | 0x00);                                           // 发送器件地址加写位
    ov7725_send_ch(reg);                                                             // 发送从机寄存器地址
    if (type == SCCB)ov7725_simiic_stop();

    ov7725_simiic_start();
    ov7725_send_ch((dev_add << 1) | 0x01);                                           // 发送器件地址加读位
    dat = ov7725_read_ch(OV7725_NACK);                                              // 读取数据
    ov7725_simiic_stop();

    return dat;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       模拟IIC 读取多字节数据
// @param       dev_add         设备地址
// @param       reg             寄存器地址
// @param       dat_add         数据保存的地址指针
// @param       num             读取字节数量
// @param       type            选择通信方式是IIC  还是 SCCB
// @return      uint8           返回寄存器的数据
// Sample usage:
// @note        内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
//static void ov7725_simiic_read_regs (uint8 dev_add, uint8 reg, uint8 *dat_add, uint8 num, IIC_type type)
//{
//  ov7725_simiic_start();
//  ov7725_send_ch( (dev_add<<1) | 0x00);                                           // 发送器件地址加写位
//  ov7725_send_ch( reg );                                                          // 发送从机寄存器地址
//  if(type == SCCB)ov7725_simiic_stop();
//
//  ov7725_simiic_start();
//  ov7725_send_ch( (dev_add<<1) | 0x01);                                           // 发送器件地址加读位
//  while(--num)
//  {
//      *dat_add = ov7725_read_ch(OV7725_ACK);                                      // 读取数据
//      dat_add++;
//  }
//  *dat_add = ov7725_read_ch(OV7725_NACK);                                         // 读取数据
//  ov7725_simiic_stop();
//}

//-------------------------------------------------------------------------------------------------------------------
// @brief       模拟IIC端口初始化
// @return      void
// Sample usage:
// @note        内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static void ov7725_simiic_init(void)
{
    gpio_init(OV7725_SCL_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(OV7725_SDA_PIN, GPO, GPIO_HIGH, GPO_OPEN_DTAIN);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       小钻风摄像头内部寄存器初始化(内部使用，用户无需调用)
// @param       NULL
// @return      uint8           返回0则出错，返回1则成功
// @since       v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 ov7725_reg_init(void)
{
    uint8 ov7725_idcode = 0;

    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_COM7, 0x80);                      // 复位摄像头
    systick_delay_ms(50);
    ov7725_idcode = ov7725_simiic_read_reg(OV7725_DEV_ADD, OV7725_VER, SCCB);
    if (ov7725_idcode != OV7725_ID)    return 0;                                     // 校验摄像头ID号

    //ID号确认无误   然后配置寄存器
#if(OV7725_FPS == 50)
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_COM4, 0xC1);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_CLKRC, 0x01);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_REG28, 0x01);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_EXHCH, 0x10);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_EXHCL, 0x1F);
#endif
#if(OV7725_FPS == 75)
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_COM4, 0x41);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_CLKRC, 0x00);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_REG28, 0x00);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_EXHCH, 0x00);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_EXHCL, 0x00);
#endif
#if(OV7725_FPS == 100)
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_COM4, 0xC1);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_CLKRC, 0x00);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_REG28, 0x01);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_EXHCH, 0x10);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_EXHCL, 0x1F);
#endif
#if(OV7725_FPS == 112)
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_COM4, 0x81);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_CLKRC, 0x00);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_REG28, 0x00);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_EXHCH, 0x00);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_EXHCL, 0x00);
#endif
#if(OV7725_FPS == 150)
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_COM4, 0xC1);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_CLKRC, 0x00);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_REG28, 0x00);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_EXHCH, 0x00);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_EXHCL, 0x00);
#endif
    //  ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_COM4         , 0xC1);
    //  ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_CLKRC        , 0x00);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_COM2, 0x03);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_COM3, 0xD0);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_COM7, 0x40);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_COM8, 0xCE);                     // 0xCE:关闭自动曝光  0xCF：开启自动曝光
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_HSTART, 0x3F);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_HSIZE, 0x50);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_VSTRT, 0x03);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_VSIZE, 0x78);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_HREF, 0x00);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_SCAL0, 0x0A);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_AWB_Ctrl0, 0xE0);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_DSPAuto, 0xff);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_DSP_Ctrl2, 0x0C);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_DSP_Ctrl3, 0x00);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_DSP_Ctrl4, 0x00);

    if (OV7725_W == 80)          ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_HOutSize, 0x14);
    else if (OV7725_W == 160)    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_HOutSize, 0x28);
    else if (OV7725_W == 240)    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_HOutSize, 0x3c);
    else if (OV7725_W == 320)    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_HOutSize, 0x50);

    if (OV7725_H == 60)          ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_VOutSize, 0x1E);
    else if (OV7725_H == 120)    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_VOutSize, 0x3c);
    else if (OV7725_H == 180)    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_VOutSize, 0x5a);
    else if (OV7725_H == 240)    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_VOutSize, 0x78);

    //  ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_REG28        , 0x01);
    //  ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_EXHCH        , 0x10);
    //  ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_EXHCL        , 0x1F);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM1, 0x0c);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM2, 0x16);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM3, 0x2a);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM4, 0x4e);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM5, 0x61);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM6, 0x6f);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM7, 0x7b);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM8, 0x86);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM9, 0x8e);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM10, 0x97);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM11, 0xa4);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM12, 0xaf);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM13, 0xc5);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM14, 0xd7);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM15, 0xe8);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_SLOP, 0x20);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_LC_RADI, 0x00);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_LC_COEF, 0x13);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_LC_XC, 0x08);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_LC_COEFB, 0x14);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_LC_COEFR, 0x17);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_LC_CTR, 0x05);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_BDBase, 0x99);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_BDMStep, 0x03);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_SDE, 0x04);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_BRIGHT, 0x00);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_CNST, ov7725_threshold);         //阈值
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_SIGN, 0x06);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_UVADJ0, 0x11);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_UVADJ1, 0x02);
    return 1;
}


//-------------------------------------------------------------------------------------------------------------------
// @brief       初始化摄像头场DMA
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void ov7725_uart_dma_init(void)
{
    uint8 num;
    for (num = 0; num < 8; num++)

    {
        gpio_init((PIN_enum)(OV7725_UART_DATA_PIN + num), GPI, GPIO_LOW, GPI_FLOATING_IN);
    }

    //DMA1总线初始化
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    //DMA摄像头初始化
    camera_dma_init(OV7725_UART_DMA_CH, (uint32)OV7725_UART_DATA_ADD, (uint32)camera_buffer_addr, OV7725_UART_SIZE);
    nvic_init(OV7725_UART_DMA_IRQN, 0x00, 0x01, ENABLE);                                // 中断配置

    OV7725_UART_DMA_CH -> CCR &= (uint16)(~DMA_CCR_EN);
    OV7725_UART_DMA_CH -> CNDTR = OV7725_UART_SIZE;
    OV7725_UART_DMA_CH -> CCR |= DMA_CCR_EN;
}


//-------------------------------------------------------------------------------------------------------------------
// @brief       小钻风摄像头初始化(调用之后设置好相关中断函数即可采集图像)
// @param       NULL
// @return      0
// @since       v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 ov7725_init(void)
{
    uint8 temp = 0;
    camera_type = CAMERA_BIN;                                                       // 设置连接摄像头类型
    camera_buffer_addr = ov7725_image_bin0[0];

    gpio_init(OV7725_UART_VSYNC_PIN, GPI, GPIO_LOW, GPI_FLOATING_IN);
    while (!temp)
    {
        temp = gpio_get(OV7725_UART_VSYNC_PIN);
    }

    ov7725_simiic_init();
    ov7725_reg_init();                                                              // 摄像头寄存器配置

    //DMA初始化
    ov7725_uart_dma_init();
    //GPIO触发定时器初始化,PLCK引脚初始化
    camera_tim_etr_init(OV7725_UART_PCLK, CAMERA_BIN_UART);
    //VSYNC初始化
    gpio_init(OV7725_UART_VSYNC_PIN, GPI, GPIO_LOW, GPI_FLOATING_IN);
    exti_interrupt_init(OV7725_UART_VSYNC_PIN, EXTI_Trigger_Rising, 0x00, 0x00);
    nvic_init(OV7725_UART_VSYNC_IRQN, 0x00, 0x00, ENABLE);

    NowCollectImg = 0;//设置取用的缓冲区
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ov7725 摄像头场中断
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:                在isr.c里面先创建对应的中断函数，然后调用该函数(之后别忘记清除中断标志位)
//-------------------------------------------------------------------------------------------------------------------
void ov7725_vsync(void)
{
    OV7725_UART_DMA_CH->CCR &= (uint16)(~DMA_CCR_EN);
    OV7725_UART_DMA_CH->CNDTR = OV7725_UART_SIZE;                                   // 设置当前DMA传输的剩余数量，向下递减，该寄存器只能在DMA不工作时更改。
    OV7725_UART_DMA_CH->CCR |= DMA_CCR_EN;                                          // 开启DMA1
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ov7725 摄像头DMA完成中断
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void ov7725_dma(void)
{
    OV7725_UART_DMA_CH->CCR &= (uint16)(~DMA_CCR_EN);                               //关闭DMA1
    ov7725_finish_flag = 1;                                                         //摄像头数据采集置1
    //下一帧数据来的时候设置的buff
    //  NowCollectImg = !NowCollectImg;//取反
    //  if(NowCollectImg == 1)
    //      DMA1_Channel4->CMAR = (u32)ov7725_image_bin1[0];
    //  else if(NowCollectImg == 0)
    //      DMA1_Channel4->CMAR = (u32)ov7725_image_bin0[0];
}


//--------------------------------------USER-----------------------------------------//
bool NowCollectImg = 0;

//////逆透视变换 把图像还原成真实图像
////uint16 ImageReal[IMAGEREAL_ROW][IMAGEREAL_COL];
////uint8 ImgLeftBorder[IMAGEREAL_ROW], ImgRightBorder[IMAGEREAL_ROW];
//////还原了一整幅图像
////float Hd[3][3] =
////{
////    104.6116, -0.0000 , -347.4925,
////    12.2805 , 13.6450 , -927.8595,
////    0.0546  , -0.0000 ,  0.8187
////};
//////Hd[0][1] = 0;
//////Hd[2][1] = 0;


//////160*80个点每个点对应的位置  写入数组
////uint8 CamToRealY[80][160];
////uint8 CamToRealX[80];
//////整幅图像坐标记录 只在开始时进行计算 注意坐标系转换
////uint8 PositionChangeArrayInit(void)
////{
////    uint16 x = 0,y = 0;
////    float px = 0,py = 0;
////    int8 i = 0;
////    uint8 lastX = 0;
////    for(x = 0;x < 95;x ++)//纵坐标
////    {
////        for(y = 0;y < 125;y ++)
////        {
////            //赋值转换后的背景色 灰色
////            ImageReal[x][y] = GRAY;
////        }
////    }
////    for(x = 1;x <= 80;x ++)//纵坐标
////    {
////        px = (((x * Hd[0][0] + Hd[0][2]) / \
////                (x * Hd[2][0] + + Hd[2][2]) + 279) / 10);
////        px += 0.5f;//四舍五入
////        px /= 2.0f;//缩小为原来的1/2
////        CamToRealX[x-1] = (uint8)px;
////        for(y = 1;y <= 160;y ++)
////        {
////            py = (((x * Hd[1][0] + y*Hd[1][1] + Hd[1][2]) / \
////                (x * Hd[2][0] + Hd[2][2]) + 1034) / 10);
////            py += 0.5f;//四舍五入
////            py /= 2.0f;//缩小为原来的1/2
////            CamToRealY[x-1][y-1] =  (uint8)py;
////        }
////        //记录图像的左右坐标值
////        //ImgLeftBorder[80 - x] = (uint8)py;
////        if((uint8)px >= 1)
////        {
////            for(i = lastX;i <= (uint8)px;i ++)
////            {
////                ImgLeftBorder[94 - i]  = CamToRealY[x-1][0];
////                ImgRightBorder[94 - i] = CamToRealY[x-1][159];
////            }
////        }
////        lastX = (uint8)px;
////    }
////
////    //ret
////    return 1;
////}



//////图像解压缩 8位图像解压为二位数组
//////舍弃后40行的图像 宽度不做改变 以获得完整的图像边界
//////读取二值化图像并且做插值处理
//////插值坐标记录点
////uint8 last_x = 0;
////uint8 last_y = 0;
//////优化后的
////void Image_Inverse_Perspective0(uint8* src0,uint8 height,uint8 wide)
////{
////    uint8 i= 0,j= 0,k = 0,m = 0,n = 0;
////    uint8 temp = 0;
////    for(i = 0;i < 80;i ++)//height 80
////    {
////        for(j = 0;j < (160/8);j ++)//wide 160
////        {
////            temp = *src0;
////            src0 ++;
////            for(k = 0;k < 8;k ++)//8个位为一个点
////            {
////                //读取像素点
////                ImageReal[CamToRealX[i]][CamToRealY[i][j * 8 + k]] =  !(((temp<<k)&0x80)>>7) * WHITE;
////                for(m = last_y ;m < CamToRealY[i][j * 8 + k];m ++)//水平插值
////                {
////                    ImageReal[CamToRealX[i]][m] = ImageReal[CamToRealX[i]][last_y];
////                    for(n = last_x;n < CamToRealX[i];n ++)//竖直插值
////                    {
////                        *(*(ImageReal + n) + m) = *(*(ImageReal + last_x) +  CamToRealY[i][j * 8 + k]);
////                    }
////                }
////                last_y = CamToRealY[i][j * 8 + k];
////            }
////        }
////        last_x = CamToRealX[i];
////    }
////}

/////***************************************************************
////* @team       :风影
////* @name       :void  CameraExtract(void)
////* @param      :none
////* @function   :摄像头数据copy 并且解压车成逆透视后的数据
////* @return     :none
////* @invocation :CameraExtract();
////***************************************************************/
////void  CameraExtract(void)
////{
////    if(NowCollectImg == 1 )
////    {
////        Image_Inverse_Perspective0(&ov7725_image_bin0[0][0],120,160);
////    }
////    else if(NowCollectImg == 0)
////    {
////        Image_Inverse_Perspective0(&ov7725_image_bin1[0][0],120,160);
////    }
////////    ips114_display_myimage(&ImageReal[0][0],0,0,125,95,125, 95);
////////    if(NowCollectImg == 0)
////////    {
////////        ips114_displayimage7725(ov7725_image_bin1[0], OV7725_W, OV7725_H);
////////    }
////////    if(NowCollectImg == 1)
////////    {
////////        ips114_displayimage7725(ov7725_image_bin0[0], OV7725_W, OV7725_H);
////////    }
////}


//---------------------------非逆透视图像解压--------------------------------//

uint8 ImageUsed[IMAGEUSED_ROW][IMAGEUSED_COL];

//摄像头数据解压
void ImageUnzip(uint8 *zipsrc, uint8 *unzipsrc)
{
    int i, j, k;
    uint16 height = 120, width = 160;
    uint16 temp = 0;
    for (i = 0; i < height - 60; i++)
    {
        for (k = 0; k < width / 8; k ++)
        {
            temp = *zipsrc;
            zipsrc++;
            if (k >= 2 && k < 18)
            {
                for (j = 0; j < 8; j++)
                {
                    if ((temp << j) & 0x80)
                        *unzipsrc = 0;
                    else
                        *unzipsrc = 255;
                    unzipsrc ++;
                }
            }
        }
    }
}


//双缓冲解压
void  CameraExtract0(void)
{

    CRM123();

    Get_Bin_Image(2);
    //    else if(NowCollectImg == 0)
    //    {
    //      ImageUnzip(&ov7725_image_bin1[0][0],&ImageUsed[0][0]);
    //    }
    //  ips114_displayimage032_zoom(&ImageUsed[0][0],128,60,128,60);
    //  if(NowCollectImg == 0)
    //  {
    //      ips114_displayimage7725(ov7725_image_bin1[0], OV7725_W, OV7725_H);
    //  }
    //  if(NowCollectImg == 1)
    //  {
    //      ips114_displayimage7725(ov7725_image_bin0[0], OV7725_W, OV7725_H);
    //  }
}


