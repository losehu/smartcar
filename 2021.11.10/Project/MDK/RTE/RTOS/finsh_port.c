/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */
#include "hal_uart.h"
#include "zf_uart.h"
#include "reg_common.h"
#include <rthw.h>
#include <rtconfig.h>
uint8 uart3_get_buffer;
bool uart3_interrupt_flag = false;													// ��������� isr.c �� UART2_IRQHandler ����λ
uint8 ch = -1;

#ifndef RT_USING_FINSH
//#error Please uncomment the line <#include "finsh_config.h"> in the rtconfig.h
#endif

#ifdef RT_USING_FINSH

RT_WEAK uint8 rt_hw_console_getchar(void)
{  
	ch=-1;
    uart_rx_irq(UART_3, ENABLE);													// ʹ�� UART2 �Ľ����ж�
    if(uart3_interrupt_flag)													// UART2 �����˽����ж�
    {
        ch=uart3_get_buffer;
        uart3_interrupt_flag = false;											// UART2 �жϱ�־��λ
        if(ch!=0&&ch!=255)
            uart_rx_irq(UART_3, DISABLE);													// ʹ�� UART2 �Ľ����ж�
    }
    uart_rx_irq(UART_3, DISABLE);													// ʹ�� UART2 �Ľ����ж�
if(ch==0||ch==255)
        return 0;
else return ch;

   
}
void uart_interrupt_handler (void)													// ��������� isr.c �� UART2_IRQHandler �е���
{
    uart3_interrupt_flag = true;													// UART2 �жϱ�־��λ
    uart_getchar(UART_3, &uart3_get_buffer);										// ��ȡ����
}
#endif /* RT_USING_FINSH */

