#ifndef _MYFLASH_H_
#define _MYFLASH_H_

#include "headfile.h"

#define IMAGETOFLASH    ImageZipToFlash(&ImageUsed[0][0],&ImgZip0[0][0])
#define FLASHTOIMAGE	ImageUzipToSram(&ImageUsed[0][0])


extern uint32 ImgZip0[60][128/8/4];

uint8 ImageZipToFlash(uint8 *imgsrc,uint32 *imgzipFlash0);
uint8 ImageUzipToSram(uint8 *imgsrc1) ;

extern uint32 ParamsNow[255];
uint8 FlashParamsWrite(void);
uint8 FlashParamsRead(void);








#endif

