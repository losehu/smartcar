/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
��ƽ    ̨�������������ܿƼ�TC264DA���İ�
����    д��ZYF/chiusir
��E-mail  ��chiusir@163.com
������汾��V1.1 ��Ȩ���У���λʹ��������ϵ��Ȩ
�������¡�2020��10��28��
�������Ϣ�ο����е�ַ��
����    վ��http://www.lqist.cn
���Ա����̡�http://longqiu.taobao.com
------------------------------------------------
��dev.env.��AURIX Development Studio1.2.2�����ϰ汾
��Target �� TC264DA/TC264D
��Crystal�� 20.000Mhz
��SYS PLL�� 200MHz
________________________________________________________________
����iLLD_1_0_1_11_0�ײ����,

ʹ�����̵�ʱ�򣬽������û�пո��Ӣ��·����
����CIFΪTC264DA�����⣬�����Ĵ������TC264D
����Ĭ�ϳ�ʼ����EMEM��512K������û�ʹ��TC264D��ע�͵�EMEM_InitConfig()��ʼ��������
������\Libraries\iLLD\TC26B\Tricore\Cpu\CStart\IfxCpu_CStart0.c��164�����ҡ�
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/

#ifndef __LQ_CAMERA_H
#define __LQ_CAMERA_H

#include "SEEKFREE_MT9V03X.h"


#define MT9V03X
#define USEIPS114

#define IMAGEH  MT9V03X_H   /*!< ����ͷ�ɼ��߶� */
#define IMAGEW  MT9V03X_W   /*!< ����ͷ�ɼ���� */

/* ʹ�������� �޸����Ｔ�� */
#ifdef USEOLED
#define LCDH    60  /*!< OLED��ʾ�߶ȣ��û�ʹ�ã��߶� */
#define LCDW    80  /*!< OLED��ʾ��ȣ��û�ʹ�ã���� */
#else
#define IPS114_W                135
#define IPS114_H                240
#endif

//#define SMALLIMAGE  // ʹ��С�ߴ���ʾ60*64
#define MAX_ROW   IPS114_H
#define MAX_COL   IPS114_W
///** ͼ��ԭʼ���ݴ�� */
//extern unsigned char Image_Data[IMAGEH][IMAGEW];

/** ѹ����֮�����ڴ����Ļ��ʾ����  */
extern unsigned char Image_Use[IPS114_H][IPS114_W];

/** ��ֵ��������OLED��ʾ������ */
extern uint8 Bin_Image[60][128];
extern unsigned short Threshold1;
extern unsigned short Threshold2;
/*!
  * @brief    �����ϱ���λ��
  *
  * @param    ��
  *
  * @return   ��
  *
  * @note     ��λ����֡ͷ������������
  *
  * @see      CAMERA_Reprot();
  *
  * @date     2019/9/24 ���ڶ�

  */

void CRM123(void);


void Get_Bin_Image(unsigned char mode);



/*!
  * @brief    �������ֵ��С
  *
  * @param    tmImage �� ͼ������
  *
  * @return   ��ֵ
  *
  * @note     �ο���https://blog.csdn.net/zyzhangyue/article/details/45841255
  * @note     https://www.cnblogs.com/moon1992/p/5092726.html
  * @note     https://www.cnblogs.com/zhonghuasong/p/7250540.html
  * @note     Ostu������������������ͨ��ͳ������ͼ���ֱ��ͼ������ʵ��ȫ����ֵT���Զ�ѡȡ�����㷨����Ϊ��
  * @note     1) �ȼ���ͼ���ֱ��ͼ������ͼ�����е����ص㰴��0~255��256��bin��ͳ������ÿ��bin�����ص�����
  * @note     2) ��һ��ֱ��ͼ��Ҳ����ÿ��bin�����ص����������ܵ����ص�
  * @note     3) i��ʾ�������ֵ��Ҳ��һ���Ҷȼ�����0��ʼ����   1
  * @note     4) ͨ����һ����ֱ��ͼ��ͳ��0~i �Ҷȼ�������(��������ֵ�ڴ˷�Χ�����ؽ���ǰ������) ��ռ����ͼ��ı���w0����ͳ��ǰ�����ص�ƽ���Ҷ�u0��ͳ��i~255�Ҷȼ�������(��������ֵ�ڴ˷�Χ�����ؽ�����������) ��ռ����ͼ��ı���w1����ͳ�Ʊ������ص�ƽ���Ҷ�u1��
  * @note     5) ����ǰ�����غͱ������صķ��� g = w0*w1*(u0-u1) (u0-u1)
  * @note     6) i++��ת��4)��ֱ��iΪ256ʱ��������
  * @note     7) �����g��Ӧ��iֵ��Ϊͼ���ȫ����ֵ
  * @note     ȱ��:OSTU�㷨�ڴ�����ղ����ȵ�ͼ���ʱ��Ч�������Բ��ã���Ϊ���õ���ȫ��������Ϣ��
  * @note     ������ղ�����  https://blog.csdn.net/kk55guang2/article/details/78475414
  * @note     https://blog.csdn.net/kk55guang2/article/details/78490069
  * @note     https://wenku.baidu.com/view/84e5eb271a37f111f0855b2d.html
  *
  * @see      GetOSTU(Image_Use);//�����ֵ
  *
  * @date     2019/6/25 ���ڶ�
  */
short GetOSTU(uint8 tmImage[60][128]);


/*!
  * @brief    ����ͷ��������
  *
  * @param
  *
  * @return
  *
  * @note     ����MT9V034  ע����Ҫʹ��  ������Ű棨��ɫ��ת����
  *
  * @example
  *
  * @date     2019/10/22 ���ڶ�
  */
void Test_CAMERA(void);


/*!
  * @brief    ����soble���ؼ�����ӵ�һ�ֱ��ؼ��
  *
  * @param    imageIn    ��������
  *           imageOut   �������      ����Ķ�ֵ����ı�����Ϣ
  *           Threshold  ��ֵ
  *
  * @return
  *
  * @note
  *
  * @example
  *
  * @date     2020/5/15
  */
void lq_sobel(unsigned char imageIn[60][128], unsigned char imageOut[60][128], unsigned char Threshold);


/*!
  * @brief    ����soble���ؼ�����ӵ�һ���Զ���ֵ���ؼ��
  *
  * @param    imageIn    ��������
  *           imageOut   �������      ����Ķ�ֵ����ı�����Ϣ
  *
  * @return
  *
  * @note
  *
  * @example
  *
  * @date     2020/5/15
  */
void lq_sobelAutoThreshold(unsigned char imageIn[60][128], unsigned char imageOut[60][128]);
void Seek_Road(void);
void Bin_Image_Filter(void);
#endif



