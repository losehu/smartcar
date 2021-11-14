#ifndef _flash_h_
#define _flash_h_

#include "headfile.h"


//定义flash最后一个扇区，避免与程序冲突
//存储参数最好从最后一个扇区开始使用
#define EXAMPLE_FLASH_SECTOR        (FLASH_SECTOR_NUM-1)

//定义所在扇区的页编号
#define EXAMPLE_FLASH_SECTOR_PAGE   (0)
#define FLASH_SAVE_NUM  64//宏定义需要存储参数的个数    一个页最多为256字节，一个参数需要占4个字节，因此最多这里为256/4=64




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
