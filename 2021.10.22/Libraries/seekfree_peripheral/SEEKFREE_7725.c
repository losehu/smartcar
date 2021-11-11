/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,��ɿƼ�
* All rights reserved.
* ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897
*
* �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
* ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
*
* @file             SEEKFREE_7725
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
*                   SDA                 A9
*                   SCL                 A8
*                   ���ж�(VSY)         A0
*                   ���ж�(HREF)        δʹ�ã���˲���
*                   �����ж�(PCLK)      A1
*                   ���ݿ�(D0-D7)       B24-B31
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


//��������ͷ֡��
#define OV7725_FPS   150//��ѡ���� 50 75 100 112 150


uint8_t ov7725_finish_flag = 0;
uint8_t ov7725_image_bin0[OV7725_H][OV7725_W / 8];
uint8_t ov7725_image_bin1[OV7725_H][OV7725_W / 8];
static uint16 ov7725_simiic_delay_time = 200;                                       // �����ʼ���쳣��������ʱ��ֵ
uint8   ov7725_threshold = 40;
//-------------------------------------------------------------------------------------------------------------------
// @brief       ģ��IIC��ʱ ʱ������
// @return      void
// Sample usage:
// @note        �ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������ov7725_simiic_delay_time��ֵ
//-------------------------------------------------------------------------------------------------------------------
static void ov7725_simiic_delay(void)
{
    uint16 delay_time;
    delay_time = ov7725_simiic_delay_time;
    while (delay_time--);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ģ��IIC��ʼ�ź�
// @return      void
// Sample usage:
// @note        �ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������ov7725_simiic_delay_time��ֵ
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
// @brief       ģ��IICֹͣ�ź�
// @return      void
// Sample usage:
// @note        �ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������ov7725_simiic_delay_time��ֵ
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
// @brief       ģ��IIC ACK�ź�
// @param       ack_dat         ��Ӧ�� (����ack:SDA=0��no_ack:SDA=0)
// @return      void
// Sample usage:
// @note        �ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������ov7725_simiic_delay_time��ֵ
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
// @brief       ģ��IIC�ȴ�Ӧ��
// @return      void
// Sample usage:
// @note        �ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������ov7725_simiic_delay_time��ֵ
//-------------------------------------------------------------------------------------------------------------------
static int ov7725_sccb_waitack(void)
{
    OV7725_SCL0();
    //  OV7725_DIR_IN();
    ov7725_simiic_delay();

    OV7725_SCL1();
    ov7725_simiic_delay();

    if (OV7725_SDA)                                                                 // Ӧ��Ϊ�ߵ�ƽ���쳣��ͨ��ʧ��
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
// @brief       ģ��IIC �ֽڷ��� �������մ�Ӧ�� �����Ǵ�Ӧ��λ
// @param       c               ����c(����������Ҳ���ǵ�ַ)
// @return      void
// Sample usage:
// @note        �ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������ov7725_simiic_delay_time��ֵ
//-------------------------------------------------------------------------------------------------------------------
static void ov7725_send_ch(uint8 c)
{
    uint8 i = 8;
    while (i--)
    {
        if (c & 0x80)
            OV7725_SDA1();                                                          // SDA �������
        else
            OV7725_SDA0();
        c <<= 1;
        ov7725_simiic_delay();
        OV7725_SCL1();                                                              // SCL ���ߣ��ɼ��ź�
        ov7725_simiic_delay();
        OV7725_SCL0();                                                              // SCL ʱ��������
    }
    ov7725_sccb_waitack();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ģ��IIC �ֽڽ��� �˳���Ӧ���|��Ӧ����|ʹ��
// @param       ack_x           Ӧ��
// @return      uint8           ����
// Sample usage:
// @note        �ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������ov7725_simiic_delay_time��ֵ
//-------------------------------------------------------------------------------------------------------------------
static uint8 ov7725_read_ch(uint8 ack_x)
{
    uint8 i;
    uint8 c;
    c = 0;
    OV7725_SCL0();
    ov7725_simiic_delay();
    OV7725_SDA1();
    //  OV7725_DIR_IN();                                                                // ��������Ϊ���뷽ʽ
    for (i = 0; i < 8; i++)
    {
        ov7725_simiic_delay();
        OV7725_SCL0();                                                              // ��ʱ����Ϊ�ͣ�׼����������λ
        ov7725_simiic_delay();
        OV7725_SCL1();                                                              // ��ʱ����Ϊ�ߣ�ʹ��������������Ч
        ov7725_simiic_delay();
        c <<= 1;
        if (OV7725_SDA)
            c += 1;                                                                 // ������λ�������յ����ݴ�c
    }
    //  OV7725_DIR_OUT();
    OV7725_SCL0();
    ov7725_simiic_delay();
    ov7725_simiic_sendack(ack_x);

    return c;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ģ��IIC д���ݵ��豸�Ĵ���
// @param       dev_add         �豸��ַ
// @param       reg             �Ĵ�����ַ
// @param       dat             д�������
// @return      void
// Sample usage:
// @note        �ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������ov7725_simiic_delay_time��ֵ
//-------------------------------------------------------------------------------------------------------------------
void ov7725_simiic_write_reg(uint8 dev_add, uint8 reg, uint8 dat)
{
    ov7725_simiic_start();
    ov7725_send_ch((dev_add << 1) | 0x00);                                           // ����������ַ��дλ
    ov7725_send_ch(reg);                                                             // ���ʹӻ��Ĵ�����ַ
    ov7725_send_ch(dat);                                                             // ������Ҫд�������
    ov7725_simiic_stop();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ģ��IIC ���豸�Ĵ�����ȡ����
// @param       dev_add         �豸��ַ
// @param       reg             �Ĵ�����ַ
// @param       type            ѡ��ͨ�ŷ�ʽ��IIC  ���� SCCB
// @return      uint8           ���ؼĴ���������
// Sample usage:
// @note        �ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������ov7725_simiic_delay_time��ֵ
//-------------------------------------------------------------------------------------------------------------------
static uint8 ov7725_simiic_read_reg(uint8 dev_add, uint8 reg, IIC_type type)
{
    uint8 dat;
    ov7725_simiic_start();
    ov7725_send_ch((dev_add << 1) | 0x00);                                           // ����������ַ��дλ
    ov7725_send_ch(reg);                                                             // ���ʹӻ��Ĵ�����ַ
    if (type == SCCB)ov7725_simiic_stop();

    ov7725_simiic_start();
    ov7725_send_ch((dev_add << 1) | 0x01);                                           // ����������ַ�Ӷ�λ
    dat = ov7725_read_ch(OV7725_NACK);                                              // ��ȡ����
    ov7725_simiic_stop();

    return dat;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ģ��IIC ��ȡ���ֽ�����
// @param       dev_add         �豸��ַ
// @param       reg             �Ĵ�����ַ
// @param       dat_add         ���ݱ���ĵ�ַָ��
// @param       num             ��ȡ�ֽ�����
// @param       type            ѡ��ͨ�ŷ�ʽ��IIC  ���� SCCB
// @return      uint8           ���ؼĴ���������
// Sample usage:
// @note        �ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������ov7725_simiic_delay_time��ֵ
//-------------------------------------------------------------------------------------------------------------------
//static void ov7725_simiic_read_regs (uint8 dev_add, uint8 reg, uint8 *dat_add, uint8 num, IIC_type type)
//{
//  ov7725_simiic_start();
//  ov7725_send_ch( (dev_add<<1) | 0x00);                                           // ����������ַ��дλ
//  ov7725_send_ch( reg );                                                          // ���ʹӻ��Ĵ�����ַ
//  if(type == SCCB)ov7725_simiic_stop();
//
//  ov7725_simiic_start();
//  ov7725_send_ch( (dev_add<<1) | 0x01);                                           // ����������ַ�Ӷ�λ
//  while(--num)
//  {
//      *dat_add = ov7725_read_ch(OV7725_ACK);                                      // ��ȡ����
//      dat_add++;
//  }
//  *dat_add = ov7725_read_ch(OV7725_NACK);                                         // ��ȡ����
//  ov7725_simiic_stop();
//}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ģ��IIC�˿ڳ�ʼ��
// @return      void
// Sample usage:
// @note        �ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������ov7725_simiic_delay_time��ֵ
//-------------------------------------------------------------------------------------------------------------------
static void ov7725_simiic_init(void)
{
    gpio_init(OV7725_SCL_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(OV7725_SDA_PIN, GPO, GPIO_HIGH, GPO_OPEN_DTAIN);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       С�������ͷ�ڲ��Ĵ�����ʼ��(�ڲ�ʹ�ã��û��������)
// @param       NULL
// @return      uint8           ����0���������1��ɹ�
// @since       v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 ov7725_reg_init(void)
{
    uint8 ov7725_idcode = 0;

    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_COM7, 0x80);                      // ��λ����ͷ
    systick_delay_ms(50);
    ov7725_idcode = ov7725_simiic_read_reg(OV7725_DEV_ADD, OV7725_VER, SCCB);
    if (ov7725_idcode != OV7725_ID)    return 0;                                     // У������ͷID��

    //ID��ȷ������   Ȼ�����üĴ���
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
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_COM8, 0xCE);                     // 0xCE:�ر��Զ��ع�  0xCF�������Զ��ع�
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
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_CNST, ov7725_threshold);         //��ֵ
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_SIGN, 0x06);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_UVADJ0, 0x11);
    ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_UVADJ1, 0x02);
    return 1;
}


//-------------------------------------------------------------------------------------------------------------------
// @brief       ��ʼ������ͷ��DMA
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

    //DMA1���߳�ʼ��
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    //DMA����ͷ��ʼ��
    camera_dma_init(OV7725_UART_DMA_CH, (uint32)OV7725_UART_DATA_ADD, (uint32)camera_buffer_addr, OV7725_UART_SIZE);
    nvic_init(OV7725_UART_DMA_IRQN, 0x00, 0x01, ENABLE);                                // �ж�����

    OV7725_UART_DMA_CH -> CCR &= (uint16)(~DMA_CCR_EN);
    OV7725_UART_DMA_CH -> CNDTR = OV7725_UART_SIZE;
    OV7725_UART_DMA_CH -> CCR |= DMA_CCR_EN;
}


//-------------------------------------------------------------------------------------------------------------------
// @brief       С�������ͷ��ʼ��(����֮�����ú�����жϺ������ɲɼ�ͼ��)
// @param       NULL
// @return      0
// @since       v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 ov7725_init(void)
{
    uint8 temp = 0;
    camera_type = CAMERA_BIN;                                                       // ������������ͷ����
    camera_buffer_addr = ov7725_image_bin0[0];

    gpio_init(OV7725_UART_VSYNC_PIN, GPI, GPIO_LOW, GPI_FLOATING_IN);
    while (!temp)
    {
        temp = gpio_get(OV7725_UART_VSYNC_PIN);
    }

    ov7725_simiic_init();
    ov7725_reg_init();                                                              // ����ͷ�Ĵ�������

    //DMA��ʼ��
    ov7725_uart_dma_init();
    //GPIO������ʱ����ʼ��,PLCK���ų�ʼ��
    camera_tim_etr_init(OV7725_UART_PCLK, CAMERA_BIN_UART);
    //VSYNC��ʼ��
    gpio_init(OV7725_UART_VSYNC_PIN, GPI, GPIO_LOW, GPI_FLOATING_IN);
    exti_interrupt_init(OV7725_UART_VSYNC_PIN, EXTI_Trigger_Rising, 0x00, 0x00);
    nvic_init(OV7725_UART_VSYNC_IRQN, 0x00, 0x00, ENABLE);

    NowCollectImg = 0;//����ȡ�õĻ�����
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ov7725 ����ͷ���ж�
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:                ��isr.c�����ȴ�����Ӧ���жϺ�����Ȼ����øú���(֮�����������жϱ�־λ)
//-------------------------------------------------------------------------------------------------------------------
void ov7725_vsync(void)
{
    OV7725_UART_DMA_CH->CCR &= (uint16)(~DMA_CCR_EN);
    OV7725_UART_DMA_CH->CNDTR = OV7725_UART_SIZE;                                   // ���õ�ǰDMA�����ʣ�����������µݼ����üĴ���ֻ����DMA������ʱ���ġ�
    OV7725_UART_DMA_CH->CCR |= DMA_CCR_EN;                                          // ����DMA1
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       ov7725 ����ͷDMA����ж�
// @param       NULL
// @return      void
// @since       v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void ov7725_dma(void)
{
    OV7725_UART_DMA_CH->CCR &= (uint16)(~DMA_CCR_EN);                               //�ر�DMA1
    ov7725_finish_flag = 1;                                                         //����ͷ���ݲɼ���1
    //��һ֡��������ʱ�����õ�buff
    //  NowCollectImg = !NowCollectImg;//ȡ��
    //  if(NowCollectImg == 1)
    //      DMA1_Channel4->CMAR = (u32)ov7725_image_bin1[0];
    //  else if(NowCollectImg == 0)
    //      DMA1_Channel4->CMAR = (u32)ov7725_image_bin0[0];
}


//--------------------------------------USER-----------------------------------------//
bool NowCollectImg = 0;

//////��͸�ӱ任 ��ͼ��ԭ����ʵͼ��
////uint16 ImageReal[IMAGEREAL_ROW][IMAGEREAL_COL];
////uint8 ImgLeftBorder[IMAGEREAL_ROW], ImgRightBorder[IMAGEREAL_ROW];
//////��ԭ��һ����ͼ��
////float Hd[3][3] =
////{
////    104.6116, -0.0000 , -347.4925,
////    12.2805 , 13.6450 , -927.8595,
////    0.0546  , -0.0000 ,  0.8187
////};
//////Hd[0][1] = 0;
//////Hd[2][1] = 0;


//////160*80����ÿ�����Ӧ��λ��  д������
////uint8 CamToRealY[80][160];
////uint8 CamToRealX[80];
//////����ͼ�������¼ ֻ�ڿ�ʼʱ���м��� ע������ϵת��
////uint8 PositionChangeArrayInit(void)
////{
////    uint16 x = 0,y = 0;
////    float px = 0,py = 0;
////    int8 i = 0;
////    uint8 lastX = 0;
////    for(x = 0;x < 95;x ++)//������
////    {
////        for(y = 0;y < 125;y ++)
////        {
////            //��ֵת����ı���ɫ ��ɫ
////            ImageReal[x][y] = GRAY;
////        }
////    }
////    for(x = 1;x <= 80;x ++)//������
////    {
////        px = (((x * Hd[0][0] + Hd[0][2]) / \
////                (x * Hd[2][0] + + Hd[2][2]) + 279) / 10);
////        px += 0.5f;//��������
////        px /= 2.0f;//��СΪԭ����1/2
////        CamToRealX[x-1] = (uint8)px;
////        for(y = 1;y <= 160;y ++)
////        {
////            py = (((x * Hd[1][0] + y*Hd[1][1] + Hd[1][2]) / \
////                (x * Hd[2][0] + Hd[2][2]) + 1034) / 10);
////            py += 0.5f;//��������
////            py /= 2.0f;//��СΪԭ����1/2
////            CamToRealY[x-1][y-1] =  (uint8)py;
////        }
////        //��¼ͼ�����������ֵ
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



//////ͼ���ѹ�� 8λͼ���ѹΪ��λ����
//////������40�е�ͼ�� ��Ȳ����ı� �Ի��������ͼ��߽�
//////��ȡ��ֵ��ͼ��������ֵ����
//////��ֵ�����¼��
////uint8 last_x = 0;
////uint8 last_y = 0;
//////�Ż����
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
////            for(k = 0;k < 8;k ++)//8��λΪһ����
////            {
////                //��ȡ���ص�
////                ImageReal[CamToRealX[i]][CamToRealY[i][j * 8 + k]] =  !(((temp<<k)&0x80)>>7) * WHITE;
////                for(m = last_y ;m < CamToRealY[i][j * 8 + k];m ++)//ˮƽ��ֵ
////                {
////                    ImageReal[CamToRealX[i]][m] = ImageReal[CamToRealX[i]][last_y];
////                    for(n = last_x;n < CamToRealX[i];n ++)//��ֱ��ֵ
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
////* @team       :��Ӱ
////* @name       :void  CameraExtract(void)
////* @param      :none
////* @function   :����ͷ����copy ���ҽ�ѹ������͸�Ӻ������
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


//---------------------------����͸��ͼ���ѹ--------------------------------//

uint8 ImageUsed[IMAGEUSED_ROW][IMAGEUSED_COL];

//����ͷ���ݽ�ѹ
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


//˫�����ѹ
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


