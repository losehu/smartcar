/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				zf_systick.h
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.32.4 or MDK 5.28
* @Target core		MM32F3277
* @Taobao			https://seekfree.taobao.com/
* @date				2021-02-22
********************************************************************************************************************/

#ifndef _zf_systick_h
#define _zf_systick_h

#include "common.h"

#define DELAY_CLK SystemCoreClock

#define USEC_TO_COUNT(us, clockFreqInHz)		(uint64_t)(((uint64_t)(us) * (clockFreqInHz)) / 1000000U)
#define COUNT_TO_USEC(count, clockFreqInHz)		(uint64_t)((uint64_t)count * 1000000U / clockFreqInHz)

#define MSEC_TO_COUNT(ms, clockFreqInHz)		(uint64_t)((uint64_t)ms * clockFreqInHz / 1000U)
#define COUNT_TO_MSEC(count, clockFreqInHz)		(uint64_t)((uint64_t)count * 1000U / clockFreqInHz)

//------------------------------------以下宏定义用于延时------------------------------------
void systick_delay (uint32 time);
void systick_delay_ms(uint32 ms);														// 毫秒级延时函数 由于滴答定时器最大只有24位，因此用函数实现，这样延时时间范围较宽
#define systick_delay_us(time)		systick_delay(USEC_TO_COUNT(time, DELAY_CLK))		// 设置延时时间 单位us 范围0 - 16777215(0xffffff)/(DELAY_CLK*1000)ms	主频96M systick最大延时时间大约为33ms


//------------------------------------以下宏定义用于获取当前时间------------------------------------
uint32 systick_getval(void);
#define systick_getval_ms()			COUNT_TO_MSEC(systick_getval(),DELAY_CLK)			// 获取当前计时时间 单位ms
#define systick_getval_us()			COUNT_TO_USEC(systick_getval(),DELAY_CLK)			// 获取当前计时时间 单位us
#define systick_getval_ns()			COUNT_TO_USEC(systick_getval()*1000,DELAY_CLK)		// 获取当前计时时间 单位ns

#endif
