#ifndef PICTUREDEAL_H
#define PICTUREDEAL_H

#include "headfile.h"

#define nWidth  MT9V03X_CSI_W
#define nHeight  MT9V03X_CSI_H

typedef enum
{
    NORMAL,
    LOSE,
    ERROR,
    L_LOSE,//只给mid_point用
    R_LOSE,//只给mid_point用
    cross,
    fill,
    HUANDAO,            //进环岛补线标志
    SHIZI,
    shizi_before,
}point_case_e;

typedef enum
{
  left_normal_right_lose,
  left_lose_right_nomal,
  all_normal,
  all_lose
}line_case_e;


typedef struct
{
    uint16 x;
    uint16 y;
    uint16 chuli;
    uint8 repiar_flag;
    line_case_e line_case_mode;
    uint16 road_wide;
    int16 mid_line_new;
    point_case_e state ;


}Point_info_t;
extern uint8  circle_point;
extern uint8  circle_flag ;
extern uint16 circle_crossroad ;
extern uint16 circle_count ;
extern int angel_count;
extern uint8 circle_ready;//环岛预标志位
extern uint8 circle_ready_l;
extern uint8 circle_ready_r;
extern uint8 circle_chack;
extern uint8 circle_out_flag2;
extern uint8 circle_out_flag;
extern uint16 circle_out_count;
extern int8  length;
extern Point_info_t left_point[nHeight];           //左线点情况
extern Point_info_t right_point[nHeight];          //右线点情况
extern Point_info_t mid_point[nHeight];            //中线点情况
extern int first_lose;

extern void edge_line_extract(uint8 *src);

#define WHITEFORCAM 255
#define BLACKFORCAM 0
extern void huandao_clear();
extern void forkroad_deal(void);

extern uint8 hd_zuo;
extern uint8 hd_you;
extern int16 left_lose_count;
extern int16 right_lose_count;
#endif
