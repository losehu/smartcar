/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-24                  the first version
 */

#include <rthw.h>
#include <rtthread.h>
#include "ANO_DT.h"
#include "zf_uart.h"

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
/*
 * Please modify RT_HEAP_SIZE if you enable RT_USING_HEAP
 * the RT_HEAP_SIZE max value = (sram size - ZI size), 1024 means 1024 bytes
 */
#define RT_HEAP_SIZE (15*1024)
static rt_uint8_t rt_heap[RT_HEAP_SIZE];

RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

void rt_os_tick_callback(void)
{
    rt_interrupt_enter();

    rt_tick_increase();

    rt_interrupt_leave();
}

/**
 * This function will initial your board.
 */
void rt_hw_board_init(void)
{
//	uart_init();
//#error "TODO 1: OS Tick Configuration."
    /*
     * TODO 1: OS Tick Configuration
     * Enable the hardware timer and call the rt_os_tick_callback function
     * periodically with the frequency RT_TICK_PER_SECOND.
     */

    /* Call components board initial (use INIT_BOARD_EXPORT()) */

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}

#ifdef RT_USING_CONSOLE

static int uart_init(void)
{
	board_init(true);
 //   ANO_DT_Init();
		//uart_init(UART_3, 115200, UART3_TX_C10, UART3_RX_C11);						// 默认初始化 UART1 用以支持 printf 输出

    rt_kprintf("\r\nuart_init ok!\r\n");

    return 0;
}
INIT_BOARD_EXPORT(uart_init);

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

#endif

