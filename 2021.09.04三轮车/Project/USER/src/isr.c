/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				isr
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.28
* @Target core		MM32F3277
* @Taobao			https://seekfree.taobao.com/
* @date				2021-02-22
********************************************************************************************************************/

#include "headfile.h"
#include "isr.h"
//#include "ANO_DT.h"


void TIM1_UP_IRQHandler (void)
{
    uint32 state = TIM1->SR;

    TIM1->SR &= ~state;
}

void TIM8_UP_IRQHandler (void)
{
    uint32 state = TIM8->SR;														// 读取中断状态
    TIM8->SR &= ~state;																// 清空中断状态
}

void TIM2_IRQHandler (void)
{
    uint32 state = TIM2->SR;														// 读取中断状态
    TIM2->SR &= ~state;																// 清空中断状态
}

void TIM5_IRQHandler (void)
{
    uint32 state = TIM5->SR;														// 读取中断状态
    TIM5->SR &= ~state;																// 清空中断状态
}

void TIM3_IRQHandler (void)
{
    uint32 state = TIM3->SR;														// 读取中断状态

    TIM3->SR &= ~state;																// 清空中断状态
}

void TIM4_IRQHandler (void)
{
    uint32 state = TIM4->SR;														// 读取中断状态
    TIM4->SR &= ~state;																// 清空中断状态
}


void TIM6_IRQHandler (void)
{
    uint32 state = TIM6->SR;														// 读取中断状态
    TIM6->SR &= ~state;																// 清空中断状态
    Timer_IT();
}

void TIM7_IRQHandler (void)
{
    uint32 state = TIM7->SR;														// 读取中断状态
    TIM7->SR &= ~state;																// 清空中断状态
}


void UART1_IRQHandler(void)
{
    if(UART1->ISR & UART_ISR_TX_INTF)												// 串口发送缓冲空中断
    {
        UART1->ICR |= UART_ICR_TXICLR;												// 清除中断标志位
    }
    if(UART1->ISR & UART_ISR_RX_INTF)												// 串口接收缓冲中断
    {
        UART1->ICR |= UART_ICR_RXICLR;												// 清除中断标志位
        //数据处理
    }
}

void UART2_IRQHandler(void)
{
    if(UART2->ISR & UART_ISR_TX_INTF)												// 串口发送缓冲空中断
    {
        UART2->ICR |= UART_ICR_TXICLR;												// 清除中断标志位
    }
    if(UART2->ISR & UART_ISR_RX_INTF)												// 串口接收缓冲中断
    {
        UART2->ICR |= UART_ICR_RXICLR;												// 清除中断标志位
    }
}

uint8 _datxx;
void UART3_IRQHandler(void)
{
    if(UART3->ISR & UART_ISR_TX_INTF)												// 串口发送缓冲空中断
    {
        UART3->ICR |= UART_ICR_TXICLR;												// 清除中断标志位
    }
    if(UART3->ISR & UART_ISR_RX_INTF)												// 串口接收缓冲中断
    {
        _datxx = (UART3->RDR ) & 0xFFU;
#if(WIRELESS_MODE == 1)//调参
        ANO_Cmd_Reslove(&_datxx);
#endif
#if(WIRELESS_MODE == 2)//通信
        ConfirmBalanceCarState(_datxx);
#endif
        UART3->ICR |= UART_ICR_RXICLR;												// 清除中断标志位
    }
}

void UART4_IRQHandler(void)
{
    if(UART4->ISR & UART_ISR_TX_INTF)												// 串口发送缓冲空中断
    {
        UART4->ICR |= UART_ICR_TXICLR;												// 清除中断标志位
    }
    if(UART4->ISR & UART_ISR_RX_INTF)												// 串口接收缓冲中断
    {
        UART4->ICR |= UART_ICR_RXICLR;												// 清除中断标志位
    }
}

void UART5_IRQHandler(void)
{
    if(UART5->ISR & UART_ISR_TX_INTF)												// 串口发送缓冲空中断
    {
        UART5->ICR |= UART_ICR_TXICLR;												// 清除中断标志位
    }
    if(UART5->ISR & UART_ISR_RX_INTF)												// 串口接收缓冲中断
    {
        UART5->ICR |= UART_ICR_RXICLR;												// 清除中断标志位
    }
}

void UART6_IRQHandler(void)
{
    if(UART6->ISR & UART_ISR_TX_INTF)												// 串口发送缓冲空中断
    {
        UART6->ICR |= UART_ICR_TXICLR;												// 清除中断标志位
    }
    if(UART6->ISR & UART_ISR_RX_INTF)												// 串口接收缓冲中断
    {

        UART6->ICR |= UART_ICR_RXICLR;												// 清除中断标志位
    }
}

void UART7_IRQHandler(void)
{
    if(UART7->ISR & UART_ISR_TX_INTF)												// 串口发送缓冲空中断
    {
        UART7->ICR |= UART_ICR_TXICLR;												// 清除中断标志位
    }
    if(UART7->ISR & UART_ISR_RX_INTF)												// 串口接收缓冲中断
    {
        UART7->ICR |= UART_ICR_RXICLR;												// 清除中断标志位
    }
}

void UART8_IRQHandler(void)
{
    if(UART8->ISR & UART_ISR_TX_INTF)												// 串口发送缓冲空中断
    {
        UART8->ICR |= UART_ICR_TXICLR;												// 清除中断标志位
    }
    if(UART8->ISR & UART_ISR_RX_INTF)												// 串口接收缓冲中断
    {
        UART8->ICR |= UART_ICR_RXICLR;												// 清除中断标志位
        switch(camera_type)															// 查询摄像头类型 未初始化摄像头则此处会进入default
        {
        default:
            break;
        }
    }
}

void EXTI0_IRQHandler(void)
{
    // 检测与清除中断标志可以根据实际应用进行删改
    EXTI_ClearFlag(EXTI_Line0);														// 清除 line0 触发标志
}

void EXTI1_IRQHandler(void)
{
    // 检测与清除中断标志可以根据实际应用进行删改
    EXTI_ClearFlag(EXTI_Line1);														// 清除 line1 触发标志
}

void EXTI2_IRQHandler(void)
{
    // 检测与清除中断标志可以根据实际应用进行删改
    EXTI_ClearFlag(EXTI_Line2);														// 清除 line2 触发标志
}

void EXTI3_IRQHandler(void)
{
    // 检测与清除中断标志可以根据实际应用进行删改
    EXTI_ClearFlag(EXTI_Line3);														// 清除 line3 触发标志
}

void EXTI4_IRQHandler(void)
{
    // 检测与清除中断标志可以根据实际应用进行删改
    EXTI_ClearFlag(EXTI_Line4);														// 清除 line4 触发标志
}

void EXTI9_5_IRQHandler(void)
{
    // 检测与清除中断标志可以根据实际应用进行删改
    if(EXTI_GetITStatus(EXTI_Line5))												// 检测 line5 是否触发
    {
        EXTI_ClearFlag(EXTI_Line5);													// 清除 line5 触发标志
    }
    if(EXTI_GetITStatus(EXTI_Line6))												// 检测 line6 是否触发
    {
        EXTI_ClearFlag(EXTI_Line6);													// 清除 line6 触发标志
    }
    if(EXTI_GetITStatus(EXTI_Line7))												// 检测 line7 是否触发
    {
        EXTI_ClearFlag(EXTI_Line7);													// 清除 line8 触发标志
    }
    if(EXTI_GetITStatus(EXTI_Line8))												// 检测 line8 是否触发
    {
        switch(camera_type)															// 查询摄像头类型 未初始化摄像头则此处会进入default
        {
        case CAMERA_BIN:  														// IIC小钻风
            ov7725_vsync();
            break;
        //			case CAMERA_BIN_UART:  													// 串口小钻风
        //				ov7725_uart_vsync();
        //				break;
        //			case CAMERA_GRAYSCALE: 													// 总钻风
        //				mt9v03x_vsync();
        //				break;
        default:
            break;
        }
        EXTI_ClearFlag(EXTI_Line8);													// 清除 line8 触发标志
    }
    if(EXTI_GetITStatus(EXTI_Line9))												// 检测 line9 是否触发
    {
        EXTI_ClearFlag(EXTI_Line9);													// 清除 line9 触发标志
    }
}

void EXTI15_10_IRQHandler (void)
{
    // 检测与清除中断标志可以根据实际应用进行删改
    if(EXTI_GetITStatus(EXTI_Line10))												// 检测 line10 是否触发
    {
        EXTI_ClearFlag(EXTI_Line10);												// 清除 line10 触发标志
    }
    if(EXTI_GetITStatus(EXTI_Line11))												// 检测 line11 是否触发
    {
        EXTI_ClearFlag(EXTI_Line11);												// 清除 line11 触发标志
    }
    if(EXTI_GetITStatus(EXTI_Line12))												// 检测 line12 是否触发
    {
        EXTI_ClearFlag(EXTI_Line12);												// 清除 line12 触发标志
    }
    if(EXTI_GetITStatus(EXTI_Line13))												// 检测 line13 是否触发
    {
        EXTI_ClearFlag(EXTI_Line13);												// 清除 line13 触发标志
    }
    if(EXTI_GetITStatus(EXTI_Line14))												// 检测 line14 是否触发
    {
        EXTI_ClearFlag(EXTI_Line14);												// 清除 line14 触发标志
    }
    if(EXTI_GetITStatus(EXTI_Line15))												// 检测 line15 是否触发
    {
        EXTI_ClearFlag(EXTI_Line15);												// 清除 line15 触发标志
    }
}

void DMA1_Channel1_IRQHandler(void)
{
    if(SET == DMA_GetFlagStatus(DMA1_FLAG_TC1))										// 判断触发通道
    {
        //DMA_ClearITPendingBit(DMA1_IT_TC1);
        DMA_ClearFlag(DMA1_FLAG_TC1);												// 清空该通道中断标志
    }
}

void DMA1_Channel2_IRQHandler(void)
{
    if(SET == DMA_GetFlagStatus(DMA1_FLAG_TC2))										// 判断触发通道
    {
        DMA_ClearFlag(DMA1_FLAG_TC2);												// 清空该通道中断标志
    }
}

void DMA1_Channel3_IRQHandler(void)
{
    if(SET == DMA_GetFlagStatus(DMA1_FLAG_TC3))										// 判断触发通道
    {
        DMA_ClearFlag(DMA1_FLAG_TC3);												// 清空该通道中断标志
    }
}

void DMA1_Channel4_IRQHandler(void)
{
    if(SET == DMA_GetFlagStatus(DMA1_FLAG_TC4))										// 判断触发通道
    {
        DMA_ClearFlag(DMA1_FLAG_TC4);												// 清空该通道中断标志
        switch(camera_type)															// 查询摄像头类型 未初始化摄像头则此处会进入default
        {
        case CAMERA_BIN:  														// IIC小钻风
            ov7725_dma();
            break;
        //			case CAMERA_BIN_UART:  													// 串口小钻风
        //				ov7725_uart_dma();
        //				break;
        //			case CAMERA_GRAYSCALE: 													// 总钻风
        //				mt9v03x_dma();
        //				break;
        default:
            break;
        }
    }
    if(DMA_GetITStatus(DMA1_IT_TC4))
    {
        //DMA_ClearITPendingBit(DMA1_IT_TC4);
        DMA_Cmd(DMA1_Channel4, DISABLE);
    }
}

void DMA1_Channel5_IRQHandler(void)
{
    if(SET == DMA_GetFlagStatus(DMA1_FLAG_TC5))										// 判断触发通道
    {
        DMA_ClearFlag(DMA1_FLAG_TC5);												// 清空该通道中断标志
    }
}

void DMA1_Channel6_IRQHandler(void)
{
    if(SET == DMA_GetFlagStatus(DMA1_FLAG_TC6))										// 判断触发通道
    {
        DMA_ClearFlag(DMA1_FLAG_TC6);												// 清空该通道中断标志
    }
}

void DMA1_Channel7_IRQHandler(void)
{
    if(SET == DMA_GetFlagStatus(DMA1_FLAG_TC7))										// 判断触发通道
    {
        DMA_ClearFlag(DMA1_FLAG_TC7);												// 清空该通道中断标志
    }
}

void DMA2_Channel1_IRQHandler(void)
{
    if(SET == DMA_GetFlagStatus(DMA1_FLAG_TC1))										// 判断触发通道
    {
        DMA_ClearFlag(DMA1_FLAG_TC1);												// 清空该通道中断标志
    }
}

void DMA2_Channel2_3_IRQHandler(void)
{
    if(SET == DMA_GetFlagStatus(DMA1_FLAG_TC2))										// 判断触发通道
    {
        DMA_ClearFlag(DMA1_FLAG_TC2);												// 清空该通道中断标志
    }
}

void DMA2_Channel3_IRQHandler(void)
{
    if(SET == DMA_GetFlagStatus(DMA1_FLAG_TC3))										// 判断触发通道
    {
        DMA_ClearFlag(DMA1_FLAG_TC3);												// 清空该通道中断标志
    }
}

void DMA2_Channel4_IRQHandler(void)
{
    if(SET == DMA_GetFlagStatus(DMA2_FLAG_TC4))										// 判断触发通道
    {
        DMA_ClearFlag(DMA2_FLAG_TC4);												// 清空该通道中断标志
    }
}

void DMA2_Channel5_IRQHandler(void)
{
    if(SET == DMA_GetFlagStatus(DMA2_FLAG_TC5))										// 判断触发通道
    {
        DMA_ClearFlag(DMA2_FLAG_TC5);												// 清空该通道中断标志
    }
}

#ifdef Will_never_be_defined
WWDG_IRQHandler
PVD_IRQHandler
TAMPER_IRQHandler
RTC_IRQHandler
FLASH_IRQHandler
RCC_CRS_IRQHandler
EXTI0_IRQHandler
EXTI1_IRQHandler
EXTI2_IRQHandler
EXTI3_IRQHandler
EXTI4_IRQHandler
DMA1_Channel1_IRQHandler
DMA1_Channel2_IRQHandler
DMA1_Channel3_IRQHandler
DMA1_Channel4_IRQHandler
DMA1_Channel5_IRQHandler
DMA1_Channel6_IRQHandler
DMA1_Channel7_IRQHandler
ADC1_2_IRQHandler
FlashCache_IRQHandler
CAN1_RX_IRQHandler
EXTI9_5_IRQHandler
TIM1_BRK_IRQHandler
TIM1_UP_IRQHandler
TIM1_TRG_COM_IRQHandler
TIM1_CC_IRQHandler
TIM2_IRQHandler
TIM3_IRQHandler
TIM4_IRQHandler
I2C1_IRQHandler
I2C2_IRQHandler
SPI1_IRQHandler
SPI2_IRQHandler
UART1_IRQHandler
UART2_IRQHandler
UART3_IRQHandler
EXTI15_10_IRQHandler
RTCAlarm_IRQHandler
OTG_FS_WKUP_IRQHandler
TIM8_BRK_IRQHandler
TIM8_UP_IRQHandler
TIM8_TRG_COM_IRQHandler
TIM8_CC_IRQHandler
ADC3_IRQHandler
SDIO_IRQHandler
TIM5_IRQHandler
SPI3_IRQHandler
UART4_IRQHandler
UART5_IRQHandler
TIM6_IRQHandler
TIM7_IRQHandler
DMA2_Channel1_IRQHandler
DMA2_Channel2_IRQHandler
DMA2_Channel3_IRQHandler
DMA2_Channel4_IRQHandler
DMA2_Channel5_IRQHandler
ETH_IRQHandler
COMP1_2_IRQHandler
OTG_FS_IRQHandler
UART6_IRQHandler
UART7_IRQHandler
UART8_IRQHandler
#endif
