/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				board.h
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.32.4 or MDK 5.28
* @Target core		MM32F3277
* @Taobao			https://seekfree.taobao.com/
* @date				2021-02-22
********************************************************************************************************************/

#ifndef _board_h
#define _board_h

#include "common.h"

/* Internal SRAM memory size[Kbytes] <8-64>, Default: 64*/
#define MM32_SRAM_SIZE      128
#define MM32_SRAM_END       (0x20000000 + MM32_SRAM_SIZE * 1024)

#define MM32_FLASH_START_ADRESS     ((uint32_t)0x08000000)
#define MM32_FLASH_SIZE             (512 * 1024)
#define MM32_FLASH_END_ADDRESS      ((uint32_t)(MM32_FLASH_START_ADRESS + MM32_FLASH_SIZE))

#if defined(__CC_ARM) || defined(__CLANG_ARM)
extern int Image$$RW_IRAM1$$ZI$$Limit;
#define HEAP_BEGIN      ((void *)&Image$$RW_IRAM1$$ZI$$Limit)
#elif __ICCARM__
#pragma section="CSTACK"
#define HEAP_BEGIN      (__segment_end("CSTACK"))
#else
extern int __bss_end;
#define HEAP_BEGIN      ((void *)&__bss_end)
#endif

#define HEAP_END        MM32_SRAM_END

#define DEBUG_UART			UART_3
#define DEBUG_UART_BAUD		115200
#define DEBUG_UART_TX		UART3_TX_C10
#define DEBUG_UART_RX		UART3_RX_C11

void board_init (bool debug_enable);

#endif
