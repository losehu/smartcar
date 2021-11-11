/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,��ɿƼ�
* All rights reserved.
* ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897
*
* �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
* ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
*
* @file				board.c
* @company			�ɶ���ɿƼ����޹�˾
* @author			��ɿƼ�(QQ3184284598)
* @version			�鿴doc��version�ļ� �汾˵��
* @Software			IAR 8.32.4 or MDK 5.28
* @Target core		MM32F3277
* @Taobao			https://seekfree.taobao.com/
* @date				2021-02-22
********************************************************************************************************************/

#include "board.h"
#include "zf_uart.h"
#include <rtthread.h>
#include "mm32_reg_redefine_v1.h"

extern uint32_t SystemCoreClock;

//finsh������մ������ݣ���ͨ���ڴ����ж��ڷ����ʼ���finsh�߳̽����ʼ����л�ȡ��
rt_mailbox_t uart_mb;


static uint32_t systick_config(rt_uint32_t ticks)
{
    if ((ticks - 1) > 0xFFFFFF)
    {
        return 1;
    }

    SysTick->LOAD = ticks - 1; 
    nvic_init(SysTick_IRQn, 7,0x00, ENABLE);
    SysTick->VAL  = 0;
    SysTick->CTRL = 0x07;  
    
    return 0;
}


void rt_hw_board_init()
{
    systick_config(SystemCoreClock / RT_TICK_PER_SECOND);
    
    board_init(1);
    
    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_HEAP)
    rt_system_heap_init((void *)HEAP_BEGIN, (void *)HEAP_END);
#endif
    
    uart_mb = rt_mb_create("uart_mb", 10, RT_IPC_FLAG_FIFO);
}


void SysTick_Handler(void)
{
    rt_interrupt_enter();

    rt_tick_increase();


    rt_interrupt_leave();
}

//void rt_hw_console_output(const char *str)
//{    rt_enter_critical();

//    while(RT_NULL != *str)
//    {
//        if('\n' == *str)
//        {
//            uart_putchar(DEBUG_UART, '\r');
//        }
//        uart_putchar(DEBUG_UART, *str++);
//    }
//		    rt_exit_critical();

//}
void rt_hw_console_output(const char *str)
{
    rt_enter_critical();
//uart_tx_irq(UART_3,ENABLE);
    while(*str!='\0')
    {
        while(UART_GetFlagStatus(UART3,UART_FLAG_TXEPT )==RESET);
        UART_SendData(UART3,*str);
        str++;
    }

    //printf("%s",str);
    /* �˳��ٽ�� */
		//uart_tx_irq(UART_3,DISABLE);

    rt_exit_critical();

}
char ch=-1;
uint8 uart3_get_buffer;
bool uart3_interrupt_flag = false;													// ��������� isr.c �� UART2_IRQHandler ����λ

char rt_hw_console_getchar(void)
{
//    uint32 dat;
//    //�ȴ��ʼ�
//    rt_mb_recv(uart_mb, &dat, RT_WAITING_FOREVER);
//    //uart_getchar(DEBUG_UART, &dat);
//    return (char)dat;
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

//-------------------------------------------------------------------------------------------------------------------
// @brief		���İ��ʼ��
// @param		debug_enable	�Ƿ���Ĭ�� debug ��� Ĭ�� UART1 
// @return		void
// Sample usage:				board_init(TRUE);
//-------------------------------------------------------------------------------------------------------------------
void board_init (bool debug_enable)
{
	__NVIC_SetPriorityGrouping(4);																	// �����ж����ȼ�������
	if(debug_enable)
	{
		uart_init(DEBUG_UART, DEBUG_UART_BAUD, DEBUG_UART_TX, DEBUG_UART_RX);						// Ĭ�ϳ�ʼ�� UART1 ����֧�� printf ���
	}
	uart_rx_irq(DEBUG_UART, 1);
}

//void UART1_IRQHandler(void)
//{
//    uint8 dat;
//    rt_interrupt_enter();
//	if(UART3->ISR & UART_ISR_TX_INTF)		// ���ڷ��ͻ�����ж�
//	{
//		UART3->ICR |= UART_ICR_TXICLR;		// ����жϱ�־λ
//	}
//	if(UART3->ISR & UART_ISR_RX_INTF)		// ���ڽ��ջ����ж�
//	{
//		uart_getchar(DEBUG_UART, &dat);
//        rt_mb_send(uart_mb, dat);           // �����ʼ�
//        UART3->ICR |= UART_ICR_RXICLR;		// ����жϱ�־λ
//	}

//    rt_interrupt_leave();
//}
void uart_interrupt_handler (void)													// ��������� isr.c �� UART2_IRQHandler �е���
{
	//    rt_interrupt_enter();

    uart3_interrupt_flag = true;													// UART2 �жϱ�־��λ
    uart_getchar(UART_3, &uart3_get_buffer);										// ��ȡ����

    //rt_interrupt_enter();
}
