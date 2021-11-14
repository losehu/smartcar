#ifndef ANO_DT_H
#define ANO_DT_H

#include "headfile.h"

#define BYTE0(dwTemp)         (*(char *)(&dwTemp))
#define BYTE1(dwTemp)         (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)         (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)         (*((char *)(&dwTemp) + 3))

extern void ANODTSend();
#endif