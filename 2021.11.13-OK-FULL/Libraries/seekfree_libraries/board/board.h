/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,��ɿƼ�
* All rights reserved.
* ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897
*
* �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
* ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
*
* @file				board.h
* @company			�ɶ���ɿƼ����޹�˾
* @author			��ɿƼ�(QQ3184284598)
* @version			�鿴doc��version�ļ� �汾˵��
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
