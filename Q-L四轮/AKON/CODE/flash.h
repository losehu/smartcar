#ifndef _flash_h_
#define _flash_h_

#include "headfile.h"


//����flash���һ������������������ͻ
//�洢������ô����һ��������ʼʹ��
#define EXAMPLE_FLASH_SECTOR        (FLASH_SECTOR_NUM-1)

//��������������ҳ���
#define EXAMPLE_FLASH_SECTOR_PAGE   (0)
#define FLASH_SAVE_NUM  64//�궨����Ҫ�洢�����ĸ���    һ��ҳ���Ϊ256�ֽڣ�һ��������Ҫռ4���ֽڣ�����������Ϊ256/4=64




typedef struct
{
    int32 temp_write[15][3];
    uint32 write_buf[FLASH_SAVE_NUM];
    uint32 read_buf[FLASH_SAVE_NUM];
    uint8 status;
}FlashInformation;
extern void FlashInit();
extern void FlashParamInit();
extern void FlashWrite();
extern void FlashRead();

#endif
