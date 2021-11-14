/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,��ɿƼ�
* All rights reserved.
* ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897
*
* �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
* ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
*
* @file				SEEKFREE_FONT
* @company			�ɶ���ɿƼ����޹�˾
* @author			��ɿƼ�(QQ3184284598)
* @version			�鿴doc��version�ļ� �汾˵��
* @Software			IAR 8.3 or MDK 5.24
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
* @note
********************************************************************************************************************/

#ifndef _SEEKFREE_FONT_h
#define _SEEKFREE_FONT_h

#include "common.h"

//-------������ɫ----------
#define   RED		0xF800		  // ��ɫ
#define   BLUE		0x001F		  // ��ɫ
#define   YELLOW	0xFFE0		  // ��ɫ
#define   GREEN		0x07E0		  // ��ɫ
#define   WHITE		0xFFFF		  // ��ɫ
#define   BLACK		0x0000	      // ��ɫ 
#define   GRAY		0X8430	      // ��ɫ
#define   BROWN		0XBC40		  // ��ɫ
#define   PURPLE	0XF81F		  // ��ɫ
#define   PINK		0XFE19	      // ��ɫ   
#define   NAVY      0x000F        // ����ɫ  
#define   DGREEN    0x03E0        // ����ɫ  
#define   DCYAN     0x03EF        // ����ɫ  
#define   MAROON    0x7800        // ���ɫ       
#define   OLIVE     0x7BE0        // �����      
#define   LGRAY     0xC618        // �Ұ�ɫ
#define   DGRAY     0x7BEF        // ���ɫ               
#define   CYAN      0x07FF        // ��ɫ        
#define   MAGENTA   0xF81F        // Ʒ��     
#define   CAMEOBROWN 0xC779		  // ��ɳɫ



extern const uint8 tft_ascii[95][16];
extern const uint8 chinese_test[8][16];

#endif
