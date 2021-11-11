#include "display.h"

#include "LQ_CAMERA.h"
/**************  总的显示界面  *******************/
uint8      HMI_DISP_FLAG = 'T';

//调整第x个参数
uint8 ParamsAdjustFloat(float *p, uint16 pos_x, uint16 pos_y, float step)
{
    //读取
    float now = *p;
    //变色
    IPS114_PENCOLOR = PURPLE;
    while (KeyPageDown != onepress)
    {
        if (KeyLeft == onepress)
        {
            KeyLeft = nopress;
            now += step * 10;
        }
        if (KeyRight == onepress)
        {
            KeyRight = nopress;
            now -= step * 10;
        }
        if (KeyUp == onepress)
        {
            KeyUp = nopress;
            now += step;
        }
        if (KeyDown == onepress)
        {
            KeyDown = nopress;
            now -= step;
        }
        ips114_showfloat(pos_x, pos_y, now, 3, 3);
    }
    KeyPageDown = nopress;
    IPS114_PENCOLOR = RED;
    *p = now;
    return 1;
}


//调整第x个参数
uint8 ParamsAdjustShort(unsigned short *p, uint16 pos_x, uint16 pos_y, short step)
{
    //读取
    short now = *p;
    IPS114_PENCOLOR = PURPLE;
    while (KeyPageDown != onepress)
    {
        if (KeyLeft == onepress)
        {
            KeyLeft = nopress;
            now += step * 10;
        }
        if (KeyRight == onepress)
        {
            KeyRight = nopress;
            now -= step * 10;
        }
        if (KeyUp == onepress)
        {
            KeyUp = nopress;
            now += step;
        }
        if (KeyDown == onepress)
        {
            KeyDown = nopress;
            now -= step;
        }
        ips114_showint16(pos_x, pos_y, now);
    }
    KeyPageDown = nopress;
    IPS114_PENCOLOR = RED;
    *p = now;

    return 1;
}


//调整第x个参数
uint8 ParamsAdjustUint32(uint32 *p, uint16 pos_x, uint16 pos_y, short step)
{
    //读取
    uint32 now = *p;
    IPS114_PENCOLOR = PURPLE;
    while (KeyPageDown != onepress)
    {
        if (KeyLeft == onepress)
        {
            KeyLeft = nopress;
            now += step * 10;
        }
        if (KeyRight == onepress)
        {
            KeyRight = nopress;
            now -= step * 10;
        }
        if (KeyUp == onepress)
        {
            KeyUp = nopress;
            now += step;
        }
        if (KeyDown == onepress)
        {
            KeyDown = nopress;
            now -= step;
        }
        ips114_showint16(pos_x, pos_y, now);
    }
    KeyPageDown = nopress;
    IPS114_PENCOLOR = RED;
    *p = now;

    return 1;
}

//调整第x个参数的状态 为是或者否
uint8 StateAdjust(uint8 *p, uint16 pos_x, uint16 pos_y)
{
    //读取
    uint8 now = *p;
    IPS114_PENCOLOR = PURPLE;
    while (KeyPageDown != onepress)
    {
        if (KeyLeft == onepress)
        {
            KeyLeft = nopress;
            if (now == 'T')
                now = 'F';
            else
                now = 'T';
        }
        if (KeyRight == onepress)
        {
            KeyRight = nopress;
            if (now == 'T')
                now = 'F';
            else
                now = 'T';
        }
        if (now == 'T')
            ips114_showstr(pos_x, pos_y, "T");
        else
            ips114_showstr(pos_x, pos_y, "F");
    }
    KeyPageDown = nopress;
    IPS114_PENCOLOR = RED;
    *p = now;

    return 1;
}


//调整第x个参数的状态 为左或者右
uint8 DirAdjust(uint8 *p, uint16 pos_x, uint16 pos_y)
{
    //读取
    uint8 now = *p;
    IPS114_PENCOLOR = PURPLE;
    while (KeyPageDown != onepress)
    {
        if (KeyLeft == onepress)
        {
            KeyLeft = nopress;
            if (now == 'L')
                now = 'R';
            else
                now = 'L';
        }
        if (KeyRight == onepress)
        {
            KeyRight = nopress;
            if (now == 'L')
                now = 'R';
            else
                now = 'L';
        }
        if (now == 'L')
            ips114_showstr(pos_x, pos_y, "L");
        else
            ips114_showstr(pos_x, pos_y, "R");
    }
    KeyPageDown = nopress;
    IPS114_PENCOLOR = RED;
    *p = now;

    return 1;
}


//按键控制选择菜单
void Menu_Choose(int8 *menu, uint8 maxnum)
{
    int8 numnow = *menu;
    //扫描按键
    if (KeyDown == onepress)
    {
        KeyDown = nopress;
        numnow ++;
        if (numnow > maxnum) numnow = maxnum;
        ips114_clear(YELLOW);
        ips114_showint8(208, 7, numnow);
    }
    if (KeyUp == onepress)
    {
        KeyUp = nopress;
        numnow --;
        if (numnow < 0) numnow = 0;
        ips114_clear(YELLOW);
        ips114_showint8(208, 7, numnow);
    }
    if (KeyRight == onepress)
    {
        KeyRight = nopress;
        numnow += 8;
        if (numnow > maxnum) numnow = maxnum;
        ips114_clear(YELLOW);
        ips114_showint8(208, 7, numnow);
    }
    if (KeyLeft == onepress)
    {
        KeyLeft = nopress;
        numnow -= 8;
        if (numnow < 0) numnow = 0;
        ips114_clear(YELLOW);
        ips114_showint8(208, 7, numnow);
    }
    *menu = numnow;
    //设置确认光标
    IPS114_PENCOLOR = BLUE;
    if (numnow < 8)
        ips114_showstr(0, numnow, ">>");
    else
        ips114_showstr(0, numnow - 8, ">>");
    IPS114_PENCOLOR = RED;
}


const uint8 Item0[] = "Camera";
const uint8 Item1[] = "Connect";//含有无线发射的控制
const uint8 Item2[] = "ELC";
const uint8 Item3[] = "ICM20602";
const uint8 Item4[] = "Motor";
const uint8 Item5[] = "Encoder";
const uint8 Item6[] = "CarInfo";
static int8 NowChooseItem = 0;

#define MAIN_MENU_NUM  6

//HMI选择目标目录 第一级菜单
uint8 HMI_Display(int8 page)
{
    static int8 flash_ok = 0;

    if (!flash_ok)
    {
        flash_ok = 1;
        ips114_clear(YELLOW);

    }
    Menu_Choose(&NowChooseItem, MAIN_MENU_NUM);
    //显示条目
    ips114_showstr(16, 0, Item0);
    ips114_showstr(16, 1, Item1);
    ips114_showstr(16, 2, Item2);
    ips114_showstr(16, 3, Item3);
    ips114_showstr(16, 4, Item4);
    ips114_showstr(16, 5, Item5);
    ips114_showstr(16, 6, Item6);
    //绘制间隔线
    ips114_DrawXLine(118, PINK, 3);
    //扫描
    if (KeyPageDown == onepress || KeyCenter == onepress)
    {
        KeyPageDown = nopress;
        KeyCenter = nopress;

        ips114_clear(YELLOW);
        switch (NowChooseItem)
        {
        case (0):
            Cam_Show();
            break;
        case (1):
            MeetCar_Show();
            break;
        case (2):
            ELC_Show();
            break;
        case (3):
            ICM20602_Show();
            break;
        case (4):
            Motor_Show();
            break;
        case (5):
            Encoder_Show();
            break;
        case (6):
            CarInfo_Show();
            break;
        }
        ips114_clear(YELLOW);
    }
    //按下S1保存参数
    if (KeyPageUp == onepress)
    {
        KeyPageUp = nopress;
        FlashParamsWrite();
    }

    //按下中键打开图像
    /*if(KeyCenter == onepress)
    {
        KeyCenter = nopress;
        ips114_clear(YELLOW);
        Cam_Processing();
    }*/
    //长按S1发车
    if (KeyPageUp == alwayspress)
    {
        KeyPageUp = nopress;
        return 1;

    }
    return 0;
}


/***************************摄像头参数调整******************************/


//基于图像坐标 非屏幕坐标
void Image_WritePoint(uint16 x, uint16 y, uint16 color)
{
    x >= COL ? x = (COL - 1) : x;
    y >= ROW ? y = (ROW - 1) : y;
    //写点
    ips114_drawpoint(x, ROW - 1 - y, color);
}


//绘制垂直于X轴的直线 size是按照中间扩充出去的距离
void Image_DrawXLine(int16 x, uint16 color, uint16 size)
{
    int16 _y = 0;
    for (_y = 0; _y < ROW; _y ++)
    {
        if (size == 1)
            Image_WritePoint(x, _y, color);
        else if (size == 3)
        {
            Image_WritePoint(x - 1, _y, color);
            Image_WritePoint(x, _y, color);
            Image_WritePoint(x + 1, _y, color);
        }
        else if (size == 5)
        {
            Image_WritePoint(x - 2, _y, color);
            Image_WritePoint(x - 1, _y, color);
            Image_WritePoint(x, _y, color);
            Image_WritePoint(x + 1, _y, color);
            Image_WritePoint(x + 2, _y, color);
        }
    }
}



//绘制一条垂直于Y轴的直线
void Image_DrawYLine(int16 y, uint16 color, uint16 size)
{
    int16 _x = 0;
    for (_x = 0; _x < COL; _x ++)
    {
        if (size == 1)
            Image_WritePoint(_x, y, color);
        else if (size == 3)
        {
            Image_WritePoint(_x, y - 1, color);
            Image_WritePoint(_x, y, color);
            Image_WritePoint(_x, y + 1, color);
        }
        else if (size == 5)
        {
            Image_WritePoint(_x, y - 2, color);
            Image_WritePoint(_x, y - 1, color);
            Image_WritePoint(_x, y, color);
            Image_WritePoint(_x, y + 1, color);
            Image_WritePoint(_x, y + 2, color);
        }
    }
}



//在特定位置显示一个X
void Image_Show_X(uint16 x, uint16 y, uint16 color, uint16 size)
{
    unsigned short i;
    if (size % 2 == 0) size++;
    //中点
    Image_WritePoint(x, y, color);
    //边缘
    for (i = 1; i <= ((size - 1) / 2); i ++)
    {
        Image_WritePoint(x - i, y - i, color);
        Image_WritePoint(x + i, y - i, color);
        Image_WritePoint(x - i, y + i, color);
        Image_WritePoint(x + i, y + i, color);
    }
}



//给图像添加网格
//图像128 * 60   C:64   C:30
void Img_ShowGird(void)
{
    //竖线
    Image_DrawYLine(5, PINK, 1);
    Image_DrawYLine(15, PINK, 1);
    Image_DrawYLine(30, PINK, 1);
    Image_DrawYLine(45, PINK, 1);
    //横线
    Image_DrawXLine(14, PINK, 1);
    Image_DrawXLine(34, PINK, 1);
    Image_DrawXLine(64, GREEN, 3);
    Image_DrawXLine(94, PINK, 1);
    Image_DrawXLine(114, PINK, 1);
    //绘制坐标
    //X
    IPS114_PENCOLOR = BLUE;
    ips114_showstr(6, 4, "14");
    IPS114_PENCOLOR = RED;
    ips114_showstr(26, 4, "34");
    IPS114_PENCOLOR = BLUE;
    ips114_showstr(56, 4, "64");
    IPS114_PENCOLOR = RED;
    ips114_showstr(86, 4, "94");
    IPS114_PENCOLOR = BLUE;
    ips114_showstr(106, 4, "114");
    //X
    ips114_showstr(130, 3, "5");
    ips114_showstr(130, 2, "15");
    ips114_showstr(130, 1, "30");
    ips114_showstr(130, 0, "45");
    IPS114_PENCOLOR = RED;
}


//绘制四邻域边缘
void ShowLeftAndRightBorder0(void)
{
    //绘制起始搜索列坐标
    for (Ysite = 0; Ysite <= 10; Ysite ++)
    {
        Image_WritePoint(Base.StartSearchColumn_1st, Ysite, CYAN);
        Image_WritePoint(Base.StartSearchColumn_1st + 1, Ysite, CYAN);
        Image_WritePoint(Base.StartSearchColumn_1st + 2, Ysite, CYAN);
        Image_WritePoint(Base.StartSearchColumn_1st - 1, Ysite, CYAN);
        Image_WritePoint(Base.StartSearchColumn_1st - 2, Ysite, CYAN);
    }
    //绘制
    //绘制三边
    for (Ysite = 5; Ysite < 59; Ysite ++)
    {
        // 修复后的中线和边线限幅
        //      LIMIT_COL(Img.Center[Ysite],127,0);
        LIMIT_COL(Img.LeftBorder[Ysite], 127, 0);
        LIMIT_COL(Img.RightBorder[Ysite], 127, 0);
        Image_WritePoint(Img.LeftBorder[Ysite] + 1, Ysite, BLUE);
        Image_WritePoint(Img.LeftBorder[Ysite] + 2, Ysite, BLUE);
        Image_WritePoint(Img.LeftBorder[Ysite] + 3, Ysite, BLUE);
        Image_WritePoint(Img.LeftBorder[Ysite] + 4, Ysite, BLUE);
        Image_WritePoint(Img.RightBorder[Ysite] - 1, Ysite, BLUE);
        Image_WritePoint(Img.RightBorder[Ysite] - 2, Ysite, BLUE);
        Image_WritePoint(Img.RightBorder[Ysite] - 3, Ysite, BLUE);
        Image_WritePoint(Img.RightBorder[Ysite] - 4, Ysite, BLUE);
        Image_WritePoint(Img.Center[Ysite], Ysite, RED);
        Image_WritePoint(Img.Center[Ysite] - 1, Ysite, RED);
        Image_WritePoint(Img.Center[Ysite] + 1, Ysite, RED);
    }
}



//显示未修复的边线
void ShowNoRepaiorLine(void)
{
    //显示修复后的中线和边线
    for (Ysite = 5; Ysite < Base.Topline; Ysite ++)
    {
        // 修复后的中线和边线限幅
        LIMIT_COL(Img.Center[Ysite], 127, 0);
        LIMIT_COL(Img.LeftBorder[Ysite], 127, 0);
        LIMIT_COL(Img.RightBorder[Ysite], 127, 0);
        Image_WritePoint(Img.LeftBorder[Ysite] + 1, Ysite, BLUE);
        Image_WritePoint(Img.LeftBorder[Ysite] + 2, Ysite, BLUE);
        Image_WritePoint(Img.LeftBorder[Ysite] + 3, Ysite, BLUE);
        Image_WritePoint(Img.LeftBorder[Ysite] + 4, Ysite, BLUE);
        Image_WritePoint(Img.RightBorder[Ysite] - 1, Ysite, BLUE);
        Image_WritePoint(Img.RightBorder[Ysite] - 2, Ysite, BLUE);
        Image_WritePoint(Img.RightBorder[Ysite] - 3, Ysite, BLUE);
        Image_WritePoint(Img.RightBorder[Ysite] - 4, Ysite, BLUE);
        Image_WritePoint(Img.Center[Ysite], Ysite, RED);
        Image_WritePoint(Img.Center[Ysite] - 1, Ysite, RED);
        Image_WritePoint(Img.Center[Ysite] + 1, Ysite, RED);
    }
}


//显示修复的中线 边线
void ShowRepairLine(void)
{
    //显示修复后的中线和边线
    for (Ysite = 5; Ysite < Base.Topline; Ysite ++)
    {
        Image_WritePoint(Img.LeftBorder_Repair[Ysite] - 1, Ysite, BLUE);
        Image_WritePoint(Img.LeftBorder_Repair[Ysite], Ysite, BLUE);
        Image_WritePoint(Img.LeftBorder_Repair[Ysite] + 1, Ysite, BLUE);
        Image_WritePoint(Img.RightBorder_Repair[Ysite] - 1, Ysite, BLUE);
        Image_WritePoint(Img.RightBorder_Repair[Ysite], Ysite, BLUE);
        Image_WritePoint(Img.RightBorder_Repair[Ysite] + 1, Ysite, BLUE);
        Image_WritePoint(Img.Center_Repair[Ysite], Ysite, RED);
        Image_WritePoint(Img.Center_Repair[Ysite] - 1, Ysite, RED);
        Image_WritePoint(Img.Center_Repair[Ysite] + 1, Ysite, RED);
    }
}

//三岔路口元素显示
void ShowForkFlag(void)
{
    Image_Show_X(Fork.Peak.x, Fork.Peak.y, RED, 12);
}


//显示理想边线
void ShowExBorder(void)
{
    //显示修复后的中线和边线
    for (Ysite = 5; Ysite < 60; Ysite ++)
    {
        Image_WritePoint((64 - RoadWide0[Ysite] / 2), Ysite, RED);
        Image_WritePoint((64 - RoadWide0[Ysite] / 2) + 1, Ysite, RED);
        Image_WritePoint((64 - RoadWide0[Ysite] / 2) + 2, Ysite, RED);
        Image_WritePoint((64 + RoadWide0[Ysite] / 2), Ysite, RED);
        Image_WritePoint((64 + RoadWide0[Ysite] / 2) - 1, Ysite, RED);
        Image_WritePoint((64 + RoadWide0[Ysite] / 2) - 2, Ysite, RED);
    }
}

//调整阈值
uint8 Adjust_Threshold(void)
{
    while (KeyPageUp != onepress)
    {
        if (ov7725_finish_flag == 1)
        {
            CameraExtract0();
            ips114_displayimage032_zoom(&ImageUsed[0][0], 128, 60, 128, 60);
            ov7725_finish_flag = 0;
        }
        if (KeyUp == onepress)
        {
            KeyUp = nopress;
            ov7725_threshold ++;
            ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_CNST, ov7725_threshold);         //阈值
            delay_ms(100);
        }
        if (KeyDown == onepress)
        {
            KeyDown = nopress;
            ov7725_threshold --;
            ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_CNST, ov7725_threshold);         //阈值
            delay_ms(100);
        }
        if (KeyLeft == onepress)
        {
            KeyLeft = nopress;
            ov7725_threshold += 10;
            ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_CNST, ov7725_threshold);         //阈值
            delay_ms(100);
        }
        if (KeyRight == onepress)
        {
            KeyRight = nopress;
            ov7725_threshold -= 10;
            ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_CNST, ov7725_threshold);         //阈值
            delay_ms(100);
        }
        ips114_showstr(180, 0, "Threshold:");
        ips114_showuint8(180, 1, ov7725_threshold);
    }
    KeyPageUp = nopress;
    return 0;
}




//LCD显示 图像总共处理后显示 包括扫线等等
uint8 Cam_Processing(void)
{
    while (KeyPageUp != onepress)
    {
        Inductor_Read();
        ELC_Normalized();
        ELC_Judge();
        ELC_Center_Calculate();
        CameraExtract0();
        GetBorder();
        ips114_displayimage032(&Bin_Image[0][0], 128, 60);
        Img_ShowGird();
        //ShowLeftAndRightBorder0();
        //ShowNoRepaiorLine();
        ShowForkFlag();
        Waveform_Write();
        ShowRepairLine();
        Center_Cal();
        ips114_showstr(170, 0, "Center:");
        ips114_showint16(170, 1, point_center);
        ips114_showstr(170, 2, "Topline:");
        ips114_showint8(170, 3, Base.Topline);
        ips114_showstr(170, 4, "ServoAdd:");
        ips114_showint16(170, 5, Servo_add);
        delay_ms(30);
    }
    ips114_clear(YELLOW);
    KeyPageUp = nopress;
    return 0;
}


//摄像头对角度
uint8 Cam_Focus(void)
{
    while (KeyPageUp != onepress)
    {
        CameraExtract0();
        ips114_displayimage032(&Bin_Image[0][0], 128, 60);
        //显示边缘
        ShowExBorder();
        delay_ms(30);

    }
    KeyPageUp = nopress;
    return 0;
}


//从flash中读取图像处理后显示 包括扫线等等
uint8 Photo_Processing(void)
{
    while (KeyPageUp != onepress)
    {
        Inductor_Read();
        ELC_Normalized();
        ELC_Judge();
        ELC_Center_Calculate();
        FLASHTOIMAGE;
        GetBorder();
        ips114_displayimage032(&Bin_Image[0][0], 128, 60);
        Img_ShowGird();
        //ShowLeftAndRightBorder0();
        //ShowNoRepaiorLine();
        ShowForkFlag();
        Waveform_Write();
        ShowRepairLine();
        Center_Cal();
        ips114_showstr(180, 0, "Center:");
        ips114_showint16(180, 1, point_center);
        delay_ms(30);
    }
    KeyPageUp = nopress;
    return 0;
}


const uint8 CamItem0[] = "All";
const uint8 CamItem1[] = "Threshold";//含有无线发射的控制
const uint8 CamItem2[] = "Photo";
const uint8 CamItem3[] = "Ramp Set";
const uint8 CamItem4[] = "Barn Set";
const uint8 CamItem5[] = "Fork Set";
const uint8 CamItem6[] = "Circle Set";
const uint8 CamItem7[] = "Normal Set";

#define CAM_MENU_NUM  7

//HMI选择目标目录 第一级菜单
uint8 Cam_Show(void)
{
    static int8 CamChooseItem = 0;
    while (KeyPageUp != onepress)
    {
        Menu_Choose(&CamChooseItem, CAM_MENU_NUM);
        //显示条目
        ips114_showstr(16, 0, CamItem0);
        ips114_showstr(16, 1, CamItem1);
        ips114_showstr(16, 2, CamItem2);
        ips114_showstr(16, 3, CamItem3);
        ips114_showstr(16, 4, CamItem4);
        ips114_showstr(16, 5, CamItem5);
        ips114_showstr(16, 6, CamItem6);
        ips114_showstr(16, 7, CamItem7);
        //绘制间隔线
        ips114_DrawXLine(110, PINK, 3);
        //
        if (KeyPageDown == onepress)
        {
            KeyPageDown = nopress;
            ips114_clear(YELLOW);
            switch (CamChooseItem)
            {
            case (0):
                Cam_Processing();
                break;
            case (1):
                Adjust_Threshold();
                break;
            case (2):
                Photo_Processing();
                break;
            case (3):; //Ramp_Show();
                break;
            case (4):
                Barn_Show();
                break;
            case (5):
                Fork_Show();
                break;
            case (6):
                ;
                Circle_Show();
                break;
            case (7): //Normal_Show();
                break;
            }
            ips114_clear(YELLOW);
        }
    }
    KeyPageUp = nopress;
    //
    return 1;
}



//图像处理参数

//车库设置
const uint8 BarnItem0[] = "OutDir";
const uint8 BarnItem1[] = "InNum";
const uint8 BarnItem2[] = "StartLineY";
const uint8 BarnItem3[] = "NoTurnRunDis";
const uint8 BarnItem4[] = "TurnCenter";
const uint8 BarnItem5[] = "TurnSpeed";
const uint8 BarnItem6[] = "InRunDis";


#define BARN_MENU_NUM  5

//HMI选择目标目录 第一级菜单
uint8 Barn_Show(void)
{
    static int8 BarnChooseItem = 0;
    while (KeyPageUp != onepress)
    {
        Menu_Choose(&BarnChooseItem, BARN_MENU_NUM);
        //显示条目
        ips114_showstr(16, 0, BarnItem0);
        ips114_showstr(16, 1, BarnItem1);
        ips114_showstr(16, 2, BarnItem2);
        ips114_showstr(16, 3, BarnItem3);
        ips114_showstr(16, 4, BarnItem4);
        ips114_showstr(16, 5, BarnItem5);
        ips114_showstr(16, 6, BarnItem6);
        //显示参数
        IPS114_PENCOLOR = BLUE;
        if (Barn.OutDir == 'L')
            ips114_showstr(120, 0, "L");
        else
            ips114_showstr(120, 0, "R");
        ips114_showuint16(120, 1, Barn.BranInNum);
        if (Barn.OutDir == 'L')
        {
            ips114_showuint16(120, 2, BarnInInfo[0][1]);
            ips114_showuint16(120, 3, BarnInInfo[0][2]);
            ips114_showuint16(120, 4, BarnInInfo[0][3]);
            ips114_showuint16(120, 5, BarnInInfo[0][4]);
            ips114_showuint16(120, 6, BarnInInfo[0][5]);
        }
        else
        {
            ips114_showuint16(120, 2, BarnInInfo[1][1]);
            ips114_showuint16(120, 3, BarnInInfo[1][2]);
            ips114_showuint16(120, 4, BarnInInfo[1][3]);
            ips114_showuint16(120, 5, BarnInInfo[1][4]);
            ips114_showuint16(120, 6, BarnInInfo[1][5]);
        }

        //调整参数
        if (KeyPageDown == onepress)
        {
            KeyPageDown = nopress;
            switch (BarnChooseItem)
            {
            case (0):
                DirAdjust(&Barn.OutDir, 120, 0);
                break;
            case (1):
                ParamsAdjustShort(&Barn.BranInNum, 120, 1, 1);
                break;
            case (2):
                ParamsAdjustShort(&Barn.StartLineFoundY, 120, 2, 1);
                break;
            case (3):
                ParamsAdjustShort(&Barn.BarnTurnCenter, 120, 3, 1);
                break;
            case (4):
                ParamsAdjustFloat(&Barn.BarnInRunDis, 120, 4, 0.01);
                break;
            case (5):
                ParamsAdjustShort(&Barn.BarnInStraightSpeed, 120, 5, 1);
                break;
            }
            ips114_clear(YELLOW);
        }
    }
    KeyPageUp = nopress;
    //
    return 1;
}



//三岔设置

//设置三岔参数
const uint8 ForkxItem0[] = "Dir";
const uint8 ForkxItem1[] = "InTurn K";
const uint8 ForkxItem2[] = "OutTurn K";

#define FORKx_MENU_NUM  2

//三岔二级菜单
uint8 Fork_ParamsAdjust(uint32 num)
{
    static int8 ForkxChooseItem = 0;
    uint8 x0dir;
    x0dir = (uint8)ForkInfo[num][0];
    ips114_clear(YELLOW);
    while (KeyPageUp != onepress)
    {
        Menu_Choose(&ForkxChooseItem, FORKx_MENU_NUM);
        //显示条目
        IPS114_PENCOLOR = RED;
        ips114_showstr(16, 0, ForkxItem0);
        ips114_showstr(16, 1, ForkxItem1);
        ips114_showstr(16, 2, ForkxItem2);
        //显示参数
        IPS114_PENCOLOR = BLUE;
        if (x0dir == 'L')
            ips114_showstr(120, 0, "L");
        else
            ips114_showstr(120, 0, "R");
        ips114_showuint16(120, 1, ForkInfo[num][1]);
        ips114_showuint16(120, 2, ForkInfo[num][2]);
        //调整参数
        if (KeyPageDown == onepress)
        {
            KeyPageDown = nopress;
            switch (ForkxChooseItem)
            {
            case (0):
                DirAdjust(&x0dir, 120, 0);
                break;
            case (1):
                ParamsAdjustUint32(&ForkInfo[num][1], 120, 1, 1);
                break;
            case (2):
                ParamsAdjustUint32(&ForkInfo[num][2], 120, 2, 1);
                break;
            }
            ips114_clear(YELLOW);
            ForkInfo[num][0] = x0dir;
        }
    }
    ips114_clear(YELLOW);
    KeyPageUp = nopress;
    //
    return 1;
}

//三岔设置
const uint8 ForkItem0[] = "Amount";
const uint8 ForkItem1[] = "In Speed";
const uint8 ForkItem2[] = "Out Speed";
const uint8 ForkItem3[] = "Fork 1";
const uint8 ForkItem4[] = "Fork 2";
const uint8 ForkItem5[] = "Fork 3";
const uint8 ForkItem6[] = "Fork 4";


#define FORK_MENU_NUM  6

//三岔菜单
uint8 Fork_Show(void)
{
    static int8 ForkChooseItem = 0;
    while (KeyPageUp != onepress)
    {
        Menu_Choose(&ForkChooseItem, FORK_MENU_NUM);
        //显示条目
        IPS114_PENCOLOR = RED;
        ips114_showstr(16, 0, ForkItem0);
        ips114_showstr(16, 1, ForkItem1);
        ips114_showstr(16, 2, ForkItem2);
        ips114_showstr(16, 3, ForkItem3);
        ips114_showstr(16, 4, ForkItem4);
        ips114_showstr(16, 5, ForkItem5);
        ips114_showstr(16, 6, ForkItem6);
        //显示参数
        IPS114_PENCOLOR = BLUE;
        ips114_showuint16(120, 0, Fork.ForkAmount);
        ips114_showuint16(120, 1, Fork.InSpeed);
        ips114_showuint16(120, 2, Fork.OutSpeed);
        //调整参数
        if (KeyPageDown == onepress)
        {
            KeyPageDown = nopress;
            switch (ForkChooseItem)
            {
            case (0):
                ParamsAdjustShort(&Fork.ForkAmount, 120, 0, 1);
                break;
            case (1):
                ParamsAdjustShort(&Fork.InSpeed, 120, 0, 1);
                break;
            case (2):
                ParamsAdjustShort(&Fork.OutSpeed, 120, 0, 1);
                break;
            case (3):
                Fork_ParamsAdjust(0);
                break;
            case (4):
                Fork_ParamsAdjust(1);
                break;
            case (5):
                Fork_ParamsAdjust(2);
                break;
            }
            ips114_clear(YELLOW);
        }
    }
    KeyPageUp = nopress;
    //
    return 1;
}


//环岛部分
//设置环岛参数
const uint8 CirclexItem0[] = "Dir";
const uint8 CirclexItem1[] = "Circle R";
const uint8 CirclexItem2[] = "Center ELC";
const uint8 CirclexItem3[] = "Turn Dis";
const uint8 CirclexItem4[] = "InTurnUP_Y";
const uint8 CirclexItem5[] = "InTurn K";
const uint8 CirclexItem6[] = "TurnSpeed";
const uint8 CirclexItem7[] = "InSpeed";
const uint8 CirclexItem8[] = "OutTurnK";


#define Circlex_MENU_NUM  8


//环岛二级菜单
uint8 Circle_ParamsAdjust(uint32 num)
{
    static int8 CirclexChooseItem = 0;
    uint8 x1dir;
    x1dir = (uint8)CircleInfo[num][0];
    ips114_clear(YELLOW);
    while (KeyPageUp != onepress)
    {
        Menu_Choose(&CirclexChooseItem, Circlex_MENU_NUM);
        //显示条目
        IPS114_PENCOLOR = RED;
        if (CirclexChooseItem <= 7)
        {
            ips114_showstr(16, 0, CirclexItem0);
            ips114_showstr(16, 1, CirclexItem1);
            ips114_showstr(16, 2, CirclexItem2);
            ips114_showstr(16, 3, CirclexItem3);
            ips114_showstr(16, 4, CirclexItem4);
            ips114_showstr(16, 5, CirclexItem5);
            ips114_showstr(16, 6, CirclexItem6);
            ips114_showstr(16, 7, CirclexItem7);
        }
        else
        {
            ips114_showstr(16, 0, CirclexItem8);
        }

        //显示参数
        IPS114_PENCOLOR = BLUE;
        if (CirclexChooseItem <= 7)
        {
            if (x1dir == 'L')
                ips114_showstr(120, 0, "L");
            else
                ips114_showstr(120, 0, "R");
            ips114_showuint16(120, 1, CircleInfo[num][1]);
            ips114_showuint16(120, 2, CircleInfo[num][2]);
            ips114_showuint16(120, 3, CircleInfo[num][3]);
            ips114_showuint16(120, 4, CircleInfo[num][4]);
            ips114_showuint16(120, 5, CircleInfo[num][5]);
            ips114_showuint16(120, 6, CircleInfo[num][6]);
            ips114_showuint16(120, 7, CircleInfo[num][7]);
        }
        else
        {
            ips114_showuint16(120, 0, CircleInfo[num][8]);
        }
        //调整参数
        if (KeyPageDown == onepress)
        {
            KeyPageDown = nopress;
            switch (CirclexChooseItem)
            {
            case (0):
                DirAdjust(&x1dir, 120, 0);
                break;
            case (1):
                ParamsAdjustUint32(&CircleInfo[num][1], 120, 1, 1);
                break;
            case (2):
                ParamsAdjustUint32(&CircleInfo[num][2], 120, 2, 1);
                break;
            case (3):
                ParamsAdjustUint32(&CircleInfo[num][3], 120, 3, 1);
                break;
            case (4):
                ParamsAdjustUint32(&CircleInfo[num][4], 120, 4, 1);
                break;
            case (5):
                ParamsAdjustUint32(&CircleInfo[num][5], 120, 5, 1);
                break;
            case (6):
                ParamsAdjustUint32(&CircleInfo[num][6], 120, 6, 1);
                break;
            case (7):
                ParamsAdjustUint32(&CircleInfo[num][7], 120, 7, 1);
                break;
            case (8):
                ParamsAdjustUint32(&CircleInfo[num][8], 120, 0, 1);
                break;
            }
            ips114_clear(YELLOW);
            CircleInfo[num][0] = x1dir;
        }
    }
    ips114_clear(YELLOW);
    KeyPageUp = nopress;
    //
    return 1;
}

//调参部分




//环岛主菜单
const uint8 CircleItem0[] = "CircleAmount";
const uint8 CircleItem1[] = "Judge ELC14";
const uint8 CircleItem2[] = "Judge ELC23";
const uint8 CircleItem3[] = "FindOutSpeed";
const uint8 CircleItem4[] = "Circle 1";
const uint8 CircleItem5[] = "Circle 2";
const uint8 CircleItem6[] = "Circle 3";
const uint8 CircleItem7[] = "Circle 4";

#define CIRCLE_MENU_NUM  7

//环岛菜单
uint8 Circle_Show(void)
{
    static int8 CircleChooseItem = 0;
    while (KeyPageUp != onepress)
    {
        Menu_Choose(&CircleChooseItem, CIRCLE_MENU_NUM);
        //显示条目
        IPS114_PENCOLOR = RED;
        ips114_showstr(16, 0, CircleItem0);
        ips114_showstr(16, 1, CircleItem1);
        ips114_showstr(16, 2, CircleItem2);
        ips114_showstr(16, 3, CircleItem3);
        ips114_showstr(16, 4, CircleItem4);
        ips114_showstr(16, 5, CircleItem5);
        ips114_showstr(16, 6, CircleItem6);
        ips114_showstr(16, 7, CircleItem7);
        //显示参数
        IPS114_PENCOLOR = BLUE;
        ips114_showuint16(120, 0, Circle.CircleAmount);
        ips114_showuint16(120, 1, Circle.CircleELC14);
        ips114_showuint16(120, 2, Circle.CircleELC23);
        ips114_showuint16(120, 3, Circle.FoundOutSpeed);
        //调整参数
        if (KeyPageDown == onepress)
        {
            KeyPageDown = nopress;
            switch (CircleChooseItem)
            {
            case (0):
                ParamsAdjustShort(&Circle.CircleAmount, 120, 0, 1);
                break;
            case (1):
                ParamsAdjustShort(&Circle.CircleELC14, 120, 1, 1);
                break;
            case (2):
                ParamsAdjustShort(&Circle.CircleELC23, 120, 2, 1);
                break;
            case (3):
                ParamsAdjustShort(&Circle.FoundOutSpeed, 120, 2, 1);
                break;
            case (4):
                Circle_ParamsAdjust(0);
                break;
            case (5):
                Circle_ParamsAdjust(1);
                break;
            case (6):
                ;
                break;
            case (7):
                ;
                break;
            }
            ips114_clear(YELLOW);
        }
    }
    KeyPageUp = nopress;
    //
    return 1;
}


////坡道设置
//const uint8 RampItem0[] = "GyroYMax";
//const uint8 RampItem1[] = "UpSpeed";
//const uint8 RampItem2[] = "DownSpeed";
//const uint8 RampItem3[] = "Fork 1";
//const uint8 RampItem4[] = "Fork 2";
//const uint8 RampItem5[] = "Fork 3";
//const uint8 RampItem6[] = "Fork 4";


//#define RAMP_MENU_NUM  6

////三岔菜单
//uint8 Ramp_Show(void)
//{
//  static int8 RampChooseItem = 0;
//  while(KeyPageUp != onepress)
//  {
//      Menu_Choose(&RampChooseItem,Ramp_MENU_NUM);
//      //显示条目
//      IPS114_PENCOLOR = RED;
//      ips114_showstr(16,0,RampItem0);
//      ips114_showstr(16,1,RampItem1);
//      ips114_showstr(16,2,RampItem2);
//      ips114_showstr(16,3,RampItem3);
//      ips114_showstr(16,4,RampItem4);
//      ips114_showstr(16,5,RampItem5);
//      ips114_showstr(16,6,RampItem6);
//      //显示参数
//      IPS114_PENCOLOR = BLUE;
//      ips114_showuint16(120,0,Ramp.RampAmount);
//      ips114_showuint16(120,1,Ramp.InSpeed);
//      ips114_showuint16(120,2,Ramp.OutSpeed);
//      //调整参数
//      if(KeyPageDown == onepress)
//      {
//          KeyPageDown = nopress;
//          switch(RampChooseItem)
//          {
//              case(0):ParamsAdjustShort(&Ramp.RampAmount,120,0,1);1`` 4
//                  break;
//              case(1):ParamsAdjustShort(&Ramp.InSpeed,120,0,1);
//                  break;
//              case(2):ParamsAdjustShort(&Ramp.OutSpeed,120,0,1);
//                  break;
//              case(3):Ramp_ParamsAdjust(0);
//                  break;
//              case(4):Ramp_ParamsAdjust(1);
//                  break;
//              case(5):Ramp_ParamsAdjust(2);
//                  break;
//          }
//          ips114_clear(YELLOW);
//      }
//  }
//  KeyPageUp = nopress;
//  //
//  return 1;
//}
