#include "MyFlash.h"


//����ͷ���� �ѵ�ǰͼ��ѹ��Ϊuint32������ ÿ��λ�д���һ������
uint32 ImgZip0[60][128 / 8 / 4];
uint8 ImageZipToFlash(uint8 *imgsrc0, uint32 *imgzipFlash0)
{
    //ת��
    int16 i = 0, j = 0, k = 0;
    uint32 *temp2;
    uint8 temp1;
    for (i = 0; i < 60; i ++)
    {
        for (j = 0 ; j < (128 / 32); j ++)
        {
            temp2 = imgzipFlash0;
            imgzipFlash0 ++;
            for (k = 31; k >= 0; k --) //ͼ����д��λ
            {
                temp1 = *imgsrc0;
                imgsrc0 ++;
                if (temp1 == 255)
                    *temp2 |= (0x80000000 >> k);
                else if (temp1 == 0)
                    *temp2 &= (0x7FFFFFFF >> k);
            }
        }
    }
    //дflash
    //128*60/32 = 240 < 256
    flash_erase_page(FLASH_SECTION_127, FLASH_PAGE_3);
    if (flash_page_program(FLASH_SECTION_127, FLASH_PAGE_3, &ImgZip0[0][0], 240))
        return 1;
    else
        return 0;
}


//����ͷͼ���ѹ
uint8 ImageUzipToSram(uint8 *imgsrc1)
{
    uint32 *imgzipFlash1;
    int16 i, j, k;
    uint32 temp = 0;
    uint8 *temp0;
    //flash -> sram
    flash_page_read(FLASH_SECTION_127, FLASH_PAGE_3, &ImgZip0[0][0], 240);
    //��ѹͼ��
    imgzipFlash1 = &ImgZip0[0][0];
    for (i = 0; i < 60; i++)
    {
        for (j = 0; j < (128 / 32); j ++)
        {
            temp = *imgzipFlash1;
            imgzipFlash1++;
            for (k = 0; k < 32; k++)
            {
                temp0 = imgsrc1;
                imgsrc1 ++;
                if ((temp >> k) & 0x00000001) //ͼ���ȶ�ȡ��λ
                    *temp0 = 255;
                else
                    *temp0 = 0;
            }
        }
    }
    return 1;
}


/******************************************  ����д���Լ���ȡ  ******************************************/

//��������u32�͵Ĳ���
uint32 ParamsNow[255];


//����д��
uint8 FlashParamsWrite(void)
{
    //��ʾ����
    ips114_clear(YELLOW);
    IPS114_PENCOLOR = RED;
    ips114_showstr(40, 3, "Start Write Params !");
    delay_ms(300);
    //��ȡ��������׼��дflash
    //////////ϵͳ����////////////
    ParamsNow[0] = CarInfo.ReadFlashParams;
    ParamsNow[1] = CarInfo.CAR_PROTECT_MODE;
    ParamsNow[2] = CarInfo.ClosedLoopMode;
    ////////////���ӿ���////////////
    ParamsNow[3] = MeetCar.MeetEnable;
    ParamsNow[4] = MeetCar.ForkNum;
    ParamsNow[5] = (uint32)(MeetCar.ReduceSpeedDis * 1000);
    ParamsNow[6] = MeetCar.ReduceSpeed;
    ParamsNow[7] = (uint32)(MeetCar.MeetForwardDis * 1000);
    ////////////ͼ����ֵ///////////
    ParamsNow[8] = ov7725_threshold;
    ////////////�������///////////
    ParamsNow[9] = Barn.OutDir;
    ParamsNow[10] = Barn.BranInNum;
    ParamsNow[11] = Barn.StartLineFoundY;
    ParamsNow[12] = Barn.BarnInRunDis * 100;
    ParamsNow[13] = Barn.BarnInStraightSpeed;
    ParamsNow[14] = Barn.BarnTurnCenter;
    ////////////�������///////////
    ParamsNow[15] = Fork.ForkAmount;
    ParamsNow[16] = Fork.InSpeed;
    ParamsNow[17] = Fork.OutSpeed;
    ParamsNow[18] = ForkInfo[0][0];
    ParamsNow[19] = ForkInfo[0][1];
    ParamsNow[20] = ForkInfo[0][2];
    ParamsNow[21] = ForkInfo[1][0];
    ParamsNow[22] = ForkInfo[1][1];
    ParamsNow[23] = ForkInfo[1][2];
    ParamsNow[24] = ForkInfo[2][0];
    ParamsNow[25] = ForkInfo[2][1];
    ParamsNow[26] = ForkInfo[2][2];
    ParamsNow[27] = ForkInfo[3][0];
    ParamsNow[28] = ForkInfo[3][1];
    ParamsNow[29] = ForkInfo[3][2];




    //дflash
    //128*60/32 = 240 < 256
    flash_erase_page(FLASH_SECTION_127, FLASH_PAGE_2);
    if (!flash_page_program(FLASH_SECTION_127, FLASH_PAGE_2, &ParamsNow[0], 255))
    {
        ips114_clear(GREEN);
        IPS114_BGCOLOR = GREEN;
        ips114_showstr(112, 3, "OK!");
        IPS114_BGCOLOR = YELLOW;
        delay_ms(300);
        ips114_clear(YELLOW);
        return 1;
    }
    else
    {
        ips114_clear(BLACK);
        IPS114_BGCOLOR = BLACK;
        ips114_showstr(112, 3, "Fail!");
        IPS114_BGCOLOR = YELLOW;
        delay_ms(300);
        ips114_clear(YELLOW);
        return 1;
    }
}


//������ȡ
uint8 FlashParamsRead(void)
{
    //��flash
    flash_page_read(FLASH_SECTION_127, FLASH_PAGE_2, &ParamsNow[0], 255);
    //д��������
    CarInfo.ReadFlashParams = ParamsNow[0];
    if (CarInfo.ReadFlashParams != 'T')
        return 0;//��Ϊ���Ʋ������ȡ
    /////////ϵͳ����/////////
    CarInfo.CAR_PROTECT_MODE = ParamsNow[1];
    CarInfo.ClosedLoopMode = ParamsNow[2];
    ////////////���ӿ���////////////
    MeetCar.MeetEnable = ParamsNow[3];
    MeetCar.ForkNum = ParamsNow[4];
    MeetCar.ReduceSpeedDis = ParamsNow[5];
    MeetCar.ReduceSpeedDis /= 1000;
    MeetCar.ReduceSpeed = ParamsNow[6];
    MeetCar.MeetForwardDis = ParamsNow[7];
    MeetCar.MeetForwardDis /= 1000;
    //����ͷ��ֵ
    ov7725_threshold = ParamsNow[8];
    ////////////�������///////////
    Barn.OutDir =  ParamsNow[9];
    Barn.BranInNum =  ParamsNow[10];
    Barn.StartLineFoundY =  ParamsNow[11];
    Barn.BarnInRunDis =  ParamsNow[12];
    Barn.BarnInRunDis /= 100;
    Barn.BarnInStraightSpeed =  ParamsNow[13];
    Barn.BarnTurnCenter = ParamsNow[14];
    ///////////�������///////////
    Fork.ForkAmount = ParamsNow[15];
    Fork.InSpeed = ParamsNow[16];
    Fork.OutSpeed = ParamsNow[17];
    ForkInfo[0][0] = ParamsNow[18];
    ForkInfo[0][1] = ParamsNow[19];
    ForkInfo[0][2] = ParamsNow[20];
    ForkInfo[1][0] = ParamsNow[21];
    ForkInfo[1][1] = ParamsNow[22];
    ForkInfo[1][2] = ParamsNow[23];
    ForkInfo[2][0] = ParamsNow[24];
    ForkInfo[2][1] = ParamsNow[25];
    ForkInfo[2][2] = ParamsNow[26];
    ForkInfo[3][0] = ParamsNow[27];
    ForkInfo[3][1] = ParamsNow[28];
    ForkInfo[3][2] = ParamsNow[29];

    return 1;
}




























