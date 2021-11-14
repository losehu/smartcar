#ifndef _lcd_h_
#define _lcd_h_

#include "headfile.h"

#define LINEWIDTH   (16)
#define ARROWWIDTH  (LINEWIDTH)
#define ARROWLENGTH (ARROWWIDTH)

typedef enum
{
    Select,
    Change,
}LcdStateEnum;

//·½Ïò
typedef enum
{
    Null,
    Up,
    Down,
    Left,
    Right,
}DirectionEnum;

typedef struct
{
    uint8 page;
    uint8 cursor;
}Lcd;
extern Lcd lcd;

extern void LcdInit();
extern void LcdCloseInterrupt();
extern void LcdDisplay();


#endif
