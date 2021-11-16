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
 QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
 *  ��    ע��TC264ֻ��15���̶���GPIO֧���ⲿ�ж� ��15���̶�GPIO��Ϊ4�飬ÿ��ֻ��ѡ������һ����Ϊ�ⲿ�жϹܽ�ʹ��
 *           0��P15_4  P33_7  P15_5                             1��P14_3  P15_8
 *           2��P10_2  P02_1  P00_4  P20_0  P11_10              3��P10_3  P14_1  P02_0  P20_9  P15_1
 *          �ⲿ�жϷ�������LQ_GPIO.c��  �ж����ȼ�������LQ_GPIO.h�� ���������޸�
 *
 ________________________________________________________________
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ����ͷ�ܽ�  TC264DA�ܽ�   ˵��              DMA��ʽ�ɼ����ùܽ�ͨ����������
 D0        P02_0       ����ͷ����bit0
 D1        P02_1       ����ͷ����bit1
 D2        P02_2       ����ͷ����bit2
 D3        P02_3       ����ͷ����bit3
 D4        P02_4       ����ͷ����bit4
 D5        P02_5       ����ͷ����bit5
 D6        P02_6       ����ͷ����bit6
 D7        P02_7       ����ͷ����bit7
 CLK       P00_4       ����ʱ��        �ⲿ�жϵ�2�飺P00_4
 VSNC      P15_1       ���ź�             �ⲿ�жϵ�3�飺P15_1

 SCL       P11_2       ����ģ��SCCB SCL
 SDA       P11_3       ����ģ��SCCB SDA
 QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/


#include "LQ_CAMERA.h"
#include "SEEKFREE_IPS114_SPI.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//����
void CRM123(void)
{
    int i, j;
    for (i = 0; i < 60; i++)
    {
        for (j = 0; j < 128; j++)
        {
            CRM[i][j] = mt9v03x_image[i][j];
        }
    }

}
/** ͼ��ԭʼ���ݴ�� */
unsigned char Image_Data[IMAGEH][IMAGEW];

/** ѹ����֮�����ڴ����Ļ��ʾ����  */
unsigned char Image_Use[IPS114_H][IPS114_W];

/** ��ֵ��������OLED��ʾ������ */
uint8 Bin_Image[60][128];

int16 OFFSET0 = 0;      //��Զ������������ֵ�ۺ�ƫ����
int16 OFFSET1 = 0;      //�ڶ���
int16 OFFSET2 = 0;      //�����������
int16 TXV = 0;          //���ε���߶ȣ��Ҹ߶�



/*************************************************************************
 *  �������ƣ�void Get_Use_Image (void)
 *  ����˵����������ͷ�ɼ���ԭʼͼ�����ŵ�����ʶ�������С
 *  ����˵������
 *  �������أ���
 *  �޸�ʱ�䣺2020��10��28��
 *  ��    ע��  IMAGEWΪԭʼͼ��Ŀ�ȣ�����Ϊ188��OV7725Ϊ320
 *       IMAGEHΪԭʼͼ��ĸ߶ȣ�����Ϊ120��OV7725Ϊ240
 *************************************************************************/
#ifdef SMALLIMAGE
void Get_Use_Image(void)
{
    short i = 0, j = 0, row = 0, line = 0;

    for (i = 0; i < IMAGEH; i += 2)          //���۸� 120 / 2  = 60��
        // for (i = 0; i < IMAGEH; i += 3)       //OV7725�� 240 / 3  = 80��
    {
        for (j = 0; j <= IMAGEW; j += 2)     //���ۿ�188 / 2  = 94��
            // for (j = 0; j <= IMAGEW; j += 3)  //OV7725��320 / 3  = 106��
        {
            Image_Use[row][line] = Image_Data[i][j];
            line++;
        }
        line = 0;
        row++;
    }
}
#else
void Get_Use_Image(void)
{
    short i = 0, j = 0, row = 0, line = 0;

    for (i = 0; i < IPS114_H; i ++)          //���۸� 120 / 2  = 60��
        // for (i = 0; i < IMAGEH; i += 3)       //OV7725�� 240 / 3  = 80��
    {
        for (j = 0; j <= IPS114_W; j ++)     //���ۿ�188 / 2  = 94��
            // for (j = 0; j <= IMAGEW; j += 3)  //OV7725��320 / 3  = 106��
        {
            Image_Use[row][line] = mt9v03x_image[i][j];
            line++;
        }
        line = 0;
        row++;
    }
}
#endif
/*************************************************************************
 *  �������ƣ�void Get_Bin_Image (unsigned char mode)
 *  ����˵����ͼ���ֵ����Bin_Image[][]
 *  ����˵����mode  ��
 *    0��ʹ�ô����ֵ
 *    1��ʹ��ƽ����ֵ
 *    2: sobel ���ӸĽ���  �ֶ���ֵ��ͬʱ�����Ϊ��ȡ���ص�ͼ��
 *    3��sobel ���ӸĽ���   ��̬��ֵ��ͬʱ�����Ϊ��ȡ���ص�ͼ��
 *  �������أ���
 *  �޸�ʱ�䣺2020��10��28��
 *  ��    ע��  Get_Bin_Image(0); //ʹ�ô�򷨶�ֵ��
 *************************************************************************/
unsigned short Threshold1 = 0;
unsigned short Threshold2 = 80;
void Get_Bin_Image(unsigned char mode)
{
    unsigned short i = 0, j = 0;
    unsigned long tv = 0;
    if (mode == 0)
    {
        Threshold1 = GetOSTU(CRM);  //�����ֵ
    }
    else if (mode == 1)
    {
        //�ۼ�
        for (i = 0; i < 60; i++)
        {
            for (j = 0; j < 128; j++)
            {
                tv += CRM[i][j];   //�ۼ�
            }
        }
        Threshold1 = (unsigned short)(tv / 60 / 128);  //��ƽ��ֵ,����Խ��ԽС��ȫ��Լ35��������ĻԼ160��һ������´�Լ100
        Threshold1 = Threshold1 + 20;      //�˴���ֵ���ã����ݻ����Ĺ������趨
    }
    else if (mode == 2)
    {
        Threshold1 = Threshold2;                          //�ֶ�������ֵ
    }
    else if (mode == 3)
    {
        lq_sobelAutoThreshold(CRM, Bin_Image);  //��̬������ֵ
        return;
    }
    /*
    #ifdef USEOLED
        sprintf(txt,"%03d",Threshold);//��ʾ��ֵ
        OLED_P6x8Str(80,0,txt);
    #else
        sprintf(txt, "%03d", Threshold);  //��ʾ��ֵ
        TFTSPI_P6X8Str(0,7, txt, u16RED, u16BLUE);
    #endif
    */
    /* ��ֵ�� */
//    for (i = 0; i < 60; i++)
//    {
//        for (j = 0; j < 128; j++)
//        {
//            if (CRM[i][j] > Threshold1) //��ֵԽ����ʾ������Խ�࣬��ǳ��ͼ��Ҳ����ʾ����
//                Bin_Image[i][j] = 255;
//            else
//                Bin_Image[i][j] = 0;
//        }
//    }
}
/*************************************************************************
 *  �������ƣ�short GetOSTU (unsigned char tmImage[LCDH][LCDW])
 *  ����˵�����������ֵ��С
 *  ����˵����tmImage �� ͼ������
 *  �������أ���
 *  �޸�ʱ�䣺2011��10��28��
 *  ��    ע��  GetOSTU(Image_Use);//�����ֵ
Ostu������������������ͨ��ͳ������ͼ���ֱ��ͼ������ʵ��ȫ����ֵT���Զ�ѡȡ�����㷨����Ϊ��
1) �ȼ���ͼ���ֱ��ͼ������ͼ�����е����ص㰴��0~255��256��bin��ͳ������ÿ��bin�����ص�����
2) ��һ��ֱ��ͼ��Ҳ����ÿ��bin�����ص����������ܵ����ص�
3) i��ʾ�������ֵ��Ҳ��һ���Ҷȼ�����0��ʼ���� 1
4) ͨ����һ����ֱ��ͼ��ͳ��0~i �Ҷȼ�������(��������ֵ�ڴ˷�Χ�����ؽ���ǰ������) ��ռ����ͼ��
        �ı���w0��        ��ͳ��ǰ�����ص�ƽ���Ҷ�u0��ͳ��i~255�Ҷȼ�������(��������ֵ�ڴ˷�Χ�����ؽ�����
        ������)  * ��ռ����ͼ��ı���w1����ͳ�Ʊ������ص�ƽ���Ҷ�u1��
5) ����ǰ�����غͱ������صķ��� g = w0*w1*(u0-u1) (u0-u1)
6) i++��ת��4)��ֱ��iΪ256ʱ��������
7) �����g��Ӧ��iֵ��Ϊͼ���ȫ����ֵ
ȱ��:OSTU�㷨�ڴ�����ղ����ȵ�ͼ���ʱ��Ч�������Բ��ã���Ϊ���õ���ȫ��������Ϣ��
*************************************************************************/
short GetOSTU(uint8 tmImage[MT9V03X_H][MT9V03X_W])
{
    signed short i, j;
    unsigned long Amount = 0;
    unsigned long PixelBack = 0;
    unsigned long PixelshortegralBack = 0;
    unsigned long Pixelshortegral = 0;
    signed long PixelshortegralFore = 0;
    signed long PixelFore = 0;
    float OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // ��䷽��;
    signed short MinValue, MaxValue;
    signed short Threshold0 = 0;
    unsigned char HistoGram[256];              //

    for (j = 0; j < 256; j++)
        HistoGram[j] = 0; //��ʼ���Ҷ�ֱ��ͼ

    for (j = 0; j < 60; j++)
    {
        for (i = 0; i < 128; i++)
        {
            HistoGram[tmImage[j][i]]++; //ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
        }
    }

    for (MinValue = 0; MinValue < 256 && HistoGram[MinValue] == 0; MinValue++);        //��ȡ��С�Ҷȵ�ֵ
    for (MaxValue = 255; MaxValue > MinValue && HistoGram[MinValue] == 0; MaxValue--); //��ȡ���Ҷȵ�ֵ

    if (MaxValue == MinValue)
        return MaxValue;         // ͼ����ֻ��һ����ɫ
    if (MinValue + 1 == MaxValue)
        return MinValue;        // ͼ����ֻ�ж�����ɫ

    for (j = MinValue; j <= MaxValue; j++)
        Amount += HistoGram[j];        //  ��������

    Pixelshortegral = 0;
    for (j = MinValue; j <= MaxValue; j++)
    {
        Pixelshortegral += HistoGram[j] * j;        //�Ҷ�ֵ����
    }
    SigmaB = -1;
    for (j = MinValue; j < MaxValue; j++)
    {
        PixelBack = PixelBack + HistoGram[j];     //ǰ�����ص���
        PixelFore = Amount - PixelBack;           //�������ص���
        OmegaBack = (float) PixelBack / Amount;   //ǰ�����ذٷֱ�
        OmegaFore = (float) PixelFore / Amount;   //�������ذٷֱ�
        PixelshortegralBack += HistoGram[j] * j;  //ǰ���Ҷ�ֵ
        PixelshortegralFore = Pixelshortegral - PixelshortegralBack;  //�����Ҷ�ֵ
        MicroBack = (float) PixelshortegralBack / PixelBack;   //ǰ���ҶȰٷֱ�
        MicroFore = (float) PixelshortegralFore / PixelFore;   //�����ҶȰٷֱ�
        Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);   //������䷽��
        if (Sigma > SigmaB)                    //����������䷽��g //�ҳ������䷽���Լ���Ӧ����ֵ
        {
            SigmaB = Sigma;
            Threshold0 = j;
        }
    }
    return Threshold0;                        //���������ֵ;
}

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
void lq_sobel(unsigned char imageIn[60][128], unsigned char imageOut[60][128], unsigned char Threshold)
{
    /** ����˴�С */
    short KERNEL_SIZE = 3;
    short xStart = KERNEL_SIZE / 2;
    short xEnd = IPS114_W - KERNEL_SIZE / 2;
    short yStart = KERNEL_SIZE / 2;
    short yEnd = IPS114_H - KERNEL_SIZE / 2;
    short i, j, k;
    short temp[4];
    for (i = yStart; i < yEnd; i++)
    {
        for (j = xStart; j < xEnd; j++)
        {
            /* ���㲻ͬ�����ݶȷ�ֵ  */
            temp[0] = -(short) imageIn[i - 1][j - 1] + (short) imageIn[i - 1][j + 1]     //{{-1, 0, 1},
                      - (short) imageIn[i][j - 1] + (short) imageIn[i][j + 1]        // {-1, 0, 1},
                      - (short) imageIn[i + 1][j - 1] + (short) imageIn[i + 1][j + 1];    // {-1, 0, 1}};

            temp[1] = -(short) imageIn[i - 1][j - 1] + (short) imageIn[i + 1][j - 1]     //{{-1, -1, -1},
                      - (short) imageIn[i - 1][j] + (short) imageIn[i + 1][j]       // { 0,  0,  0},
                      - (short) imageIn[i - 1][j + 1] + (short) imageIn[i + 1][j + 1];    // { 1,  1,  1}};

            temp[2] = -(short) imageIn[i - 1][j] + (short) imageIn[i][j - 1]       //  0, -1, -1
                      - (short) imageIn[i][j + 1] + (short) imageIn[i + 1][j]       //  1,  0, -1
                      - (short) imageIn[i - 1][j + 1] + (short) imageIn[i + 1][j - 1];    //  1,  1,  0

            temp[3] = -(short) imageIn[i - 1][j] + (short) imageIn[i][j + 1]       // -1, -1,  0
                      - (short) imageIn[i][j - 1] + (short) imageIn[i + 1][j]       // -1,  0,  1
                      - (short) imageIn[i - 1][j - 1] + (short) imageIn[i + 1][j + 1];    //  0,  1,  1

            temp[0] = abs(temp[0]);
            temp[1] = abs(temp[1]);
            temp[2] = abs(temp[2]);
            temp[3] = abs(temp[3]);

            /* �ҳ��ݶȷ�ֵ���ֵ  */
            for (k = 1; k < 4; k++)
            {
                if (temp[0] < temp[k])
                {
                    temp[0] = temp[k];
                }
            }

            if (temp[0] > Threshold)
            {
                imageOut[i][j] = 1;
            }
            else
            {
                imageOut[i][j] = 0;
            }
        }
    }
}

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
void lq_sobelAutoThreshold(unsigned char imageIn[60][128], unsigned char imageOut[60][128])
{
    /** ����˴�С */
    short KERNEL_SIZE = 3;
    short xStart = KERNEL_SIZE / 2;
    short xEnd = IPS114_W - KERNEL_SIZE / 2;
    short yStart = KERNEL_SIZE / 2;
    short yEnd = IPS114_H - KERNEL_SIZE / 2;
    short i, j, k;
    short temp[4];
    for (i = yStart; i < yEnd; i++)
    {
        for (j = xStart; j < xEnd; j++)
        {
            /* ���㲻ͬ�����ݶȷ�ֵ  */
            temp[0] = -(short) imageIn[i - 1][j - 1] + (short) imageIn[i - 1][j + 1]     //{{-1, 0, 1},
                      - (short) imageIn[i][j - 1] + (short) imageIn[i][j + 1]       // {-1, 0, 1},
                      - (short) imageIn[i + 1][j - 1] + (short) imageIn[i + 1][j + 1];    // {-1, 0, 1}};

            temp[1] = -(short) imageIn[i - 1][j - 1] + (short) imageIn[i + 1][j - 1]     //{{-1, -1, -1},
                      - (short) imageIn[i - 1][j] + (short) imageIn[i + 1][j]       // { 0,  0,  0},
                      - (short) imageIn[i - 1][j + 1] + (short) imageIn[i + 1][j + 1];    // { 1,  1,  1}};

            temp[2] = -(short) imageIn[i - 1][j] + (short) imageIn[i][j - 1]       //  0, -1, -1
                      - (short) imageIn[i][j + 1] + (short) imageIn[i + 1][j]       //  1,  0, -1
                      - (short) imageIn[i - 1][j + 1] + (short) imageIn[i + 1][j - 1];    //  1,  1,  0

            temp[3] = -(short) imageIn[i - 1][j] + (short) imageIn[i][j + 1]       // -1, -1,  0
                      - (short) imageIn[i][j - 1] + (short) imageIn[i + 1][j]       // -1,  0,  1
                      - (short) imageIn[i - 1][j - 1] + (short) imageIn[i + 1][j + 1];    //  0,  1,  1

            temp[0] = abs(temp[0]);
            temp[1] = abs(temp[1]);
            temp[2] = abs(temp[2]);
            temp[3] = abs(temp[3]);

            /* �ҳ��ݶȷ�ֵ���ֵ  */
            for (k = 1; k < 4; k++)
            {
                if (temp[0] < temp[k])
                {
                    temp[0] = temp[k];
                }
            }

            /* ʹ�����ص����������ص�֮�͵�һ������    ��Ϊ��ֵ  */
            temp[3] = (short) imageIn[i - 1][j - 1] + (short) imageIn[i - 1][j] + (short) imageIn[i - 1][j + 1]
                      + (short) imageIn[i][j - 1] + (short) imageIn[i][j] + (short) imageIn[i][j + 1]
                      + (short) imageIn[i + 1][j - 1] + (short) imageIn[i + 1][j] + (short) imageIn[i + 1][j + 1];

            if (temp[0] > temp[3] / 12.0f)
            {
                imageOut[i][j] = 1;
            }
            else
            {
                imageOut[i][j] = 0;
            }

        }
    }
}

/*---------------------------------------------------------------
 ����    ����Bin_Image_Filter
 ����    �ܡ��������
 ����    ������
 ���� �� ֵ����
 ��ע�����
 ----------------------------------------------------------------*/
void Bin_Image_Filter(void)
{
    int16 nr; //��
    int16 nc; //��

    for (nr = 1; nr < IPS114_H - 1; nr++)
    {
        for (nc = 1; nc < IPS114_W - 1; nc = nc + 1)
        {
            if ((Bin_Image[nr][nc] == 0)
                    && (Bin_Image[nr - 1][nc] + Bin_Image[nr + 1][nc] + Bin_Image[nr][nc + 1] + Bin_Image[nr][nc - 1] > 2))
            {
                Bin_Image[nr][nc] = 1;
            }
            else if ((Bin_Image[nr][nc] == 1)
                     && (Bin_Image[nr - 1][nc] + Bin_Image[nr + 1][nc] + Bin_Image[nr][nc + 1] + Bin_Image[nr][nc - 1] < 2))
            {
                Bin_Image[nr][nc] = 0;
            }
        }
    }
}

/***************************************************************************
 *                                                                          *
 *  �������ƣ�Seek_Road(void)                                           *
 *  ����˵����Ѱ�Ұ�ɫ����ƫ��ֵ                                            *
 *  ����˵������                                                            *
 *  �������أ�ֵ�Ĵ�С                                                      *
 *  �޸�ʱ�䣺2017-07-16                                                    *
 *  ��    ע�����м�Ϊ0������һ���Ҳ��һ����ֵ����1�����                *
 *            ��������ӵ�һ�п�ʼ�������ڶ��н�����                        *
 *            ������Ϊ��������ֵԽ��˵��Խƫ��ߣ�                        *
 *            ������Ϊ��������ֵԽ��˵��Խƫ�ұߡ�                        *
 ***************************************************************************/
void Seek_Road(void)
{
    int16 nr; //��
    int16 nc; //��
    int16 temp = 0; //��ʱ��ֵ
    //for(nr=1; nr<MAX_ROW-1; nr++)
    temp = 0;
    for (nr = 8; nr < 24; nr++)
    {
        for (nc = MAX_COL / 2; nc < MAX_COL; nc = nc + 1)
        {
            if (Bin_Image[nr][nc])
            {
                ++temp;
            }
        }
        for (nc = 0; nc < MAX_COL / 2; nc = nc + 1)
        {
            if (Bin_Image[nr][nc])
            {
                --temp;
            }
        }
    }
    OFFSET0 = temp;
    temp = 0;
    for (nr = 24; nr < 40; nr++)
    {
        for (nc = MAX_COL / 2; nc < MAX_COL; nc = nc + 1)
        {
            if (Bin_Image[nr][nc])
            {
                ++temp;
            }
        }
        for (nc = 0; nc < MAX_COL / 2; nc = nc + 1)
        {
            if (Bin_Image[nr][nc])
            {
                --temp;
            }
        }
    }
    OFFSET1 = temp;
    temp = 0;
    for (nr = 40; nr < 56; nr++)
    {
        for (nc = MAX_COL / 2; nc < MAX_COL; nc = nc + 1)
        {
            if (Bin_Image[nr][nc])
            {
                ++temp;
            }
        }
        for (nc = 0; nc < MAX_COL / 2; nc = nc + 1)
        {
            if (Bin_Image[nr][nc])
            {
                --temp;
            }
        }
    }
    OFFSET2 = temp;
    return;
}
