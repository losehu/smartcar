/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				board.c
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
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

//finsh组件接收串口数据，是通过在串口中断内发送邮件，finsh线程接收邮件进行获取的
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
    /* 退出临界段 */
		//uart_tx_irq(UART_3,DISABLE);

    rt_exit_critical();

}
char ch=-1;
uint8 uart3_get_buffer;
bool uart3_interrupt_flag = false;													// 这个变量在 isr.c 的 UART2_IRQHandler 中置位

char rt_hw_console_getchar(void)
{
//    uint32 dat;
//    //等待邮件
//    rt_mb_recv(uart_mb, &dat, RT_WAITING_FOREVER);
//    //uart_getchar(DEBUG_UART, &dat);
//    return (char)dat;
		ch=-1;
    uart_rx_irq(UART_3, ENABLE);													// 使能 UART2 的接收中断
    if(uart3_interrupt_flag)													// UART2 触发了接收中断
    {
        ch=uart3_get_buffer;
        uart3_interrupt_flag = false;											// UART2 中断标志复位
        if(ch!=0&&ch!=255)
            uart_rx_irq(UART_3, DISABLE);													// 使能 UART2 的接收中断
    }
    uart_rx_irq(UART_3, DISABLE);													// 使能 UART2 的接收中断
if(ch==0||ch==255)
        return 0;
else return ch;

}

//-------------------------------------------------------------------------------------------------------------------
// @brief		核心板初始化
// @param		debug_enable	是否开启默认 debug 输出 默认 UART1 
// @return		void
// Sample usage:				board_init(TRUE);
//-------------------------------------------------------------------------------------------------------------------
void board_init (bool debug_enable)
{
	__NVIC_SetPriorityGrouping(4);																	// 设置中断优先级不分组
	if(debug_enable)
	{
		uart_init(DEBUG_UART, DEBUG_UART_BAUD, DEBUG_UART_TX, DEBUG_UART_RX);						// 默认初始化 UART1 用以支持 printf 输出
	}
	uart_rx_irq(DEBUG_UART, 1);
}

//void UART1_IRQHandler(void)
//{
//    uint8 dat;
//    rt_interrupt_enter();
//	if(UART3->ISR & UART_ISR_TX_INTF)		// 串口发送缓冲空中断
//	{
//		UART3->ICR |= UART_ICR_TXICLR;		// 清除中断标志位
//	}
//	if(UART3->ISR & UART_ISR_RX_INTF)		// 串口接收缓冲中断
//	{
//		uart_getchar(DEBUG_UART, &dat);
//        rt_mb_send(uart_mb, dat);           // 发送邮件
//        UART3->ICR |= UART_ICR_RXICLR;		// 清除中断标志位
//	}

//    rt_interrupt_leave();
//}
void uart_interrupt_handler (void)													// 这个函数在 isr.c 的 UART2_IRQHandler 中调用
{
	//    rt_interrupt_enter();

    uart3_interrupt_flag = true;													// UART2 中断标志置位
    uart_getchar(UART_3, &uart3_get_buffer);										// 读取数据

    //rt_interrupt_enter();
}
