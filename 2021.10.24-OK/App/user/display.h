#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "headfile.h"


//statement
extern uint8      HMI_DISP_FLAG;



//fun
void Image_WritePoint(uint16 x, uint16 y, uint16 color); //Í¼Ïñ×ø±ê
void Image_DrawXLine(int16 x, uint16 color, uint16 size);
void Image_DrawYLine(int16 y, uint16 color, uint16 size);
void Image_Show_X(uint16 x, uint16 y, uint16 color, uint16 size);
void Img_ShowGird(void);
void ShowLeftAndRightBorder0(void);
void ShowNoRepaiorLine(void);
void ShowRepairLine(void);
void Menu_Choose(int8 *menu, uint8 maxnum);
uint8 HMI_Display(int8 page);
void  ShowForkFlag(void);
uint8 ParamsAdjustFloat(float *p, uint16 pos_x, uint16 pos_y, float step);
uint8 ParamsAdjustShort(unsigned short *p, uint16 pos_x, uint16 pos_y, short step);
uint8 StateAdjust(uint8 *p, uint16 pos_x, uint16 pos_y);
uint8 Cam_Show(void);
uint8 Cam_Processing(void);
uint8 Barn_Show(void);
uint8 Fork_Show(void);
uint8 Circle_Show(void);
#endif

