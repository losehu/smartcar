#include "cam.h"


uint16 Threshold;
image_t imageRaw = DEF_IMAGE(NULL, MT9V03X_CSI_W, MT9V03X_CSI_H);//80,128

AT_DTCM_SECTION_ALIGN(uint8_t img_thres_data[MT9V03X_CSI_H][MT9V03X_CSI_W], 64);
image_t imageAdaptiveThreshold = DEF_IMAGE((uint8_t *) img_thres_data, MT9V03X_CSI_W, MT9V03X_CSI_H);//80,128

AT_DTCM_SECTION_ALIGN(uint8_t img_line_data[MT9V03X_CSI_H][MT9V03X_CSI_W], 64);
image_t imageLine = DEF_IMAGE((uint8_t *) img_line_data, MT9V03X_CSI_W, MT9V03X_CSI_H);


uint8 otsuThreshold(uint8 *image, uint16 col, uint16 row)
{
    #define GrayScale 256
    uint16 width = col;
    uint16 height = row;
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j, pixelSum = width * height;
    uint8 threshold = 0;
    uint8* data = image;  //指向像素数据的指针
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }

    //统计灰度级中每个像素在整幅图像中的个数  
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            pixelCount[(int)data[i * width + j]]++;  //将像素值作为计数数组的下标
        }
    }

    //计算每个像素在整幅图像中的比例  
    float maxPro = 0.0;
    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
        if (pixelPro[i] > maxPro)
        {
            maxPro = pixelPro[i];
        }
    }

    //遍历灰度级[0,255]  
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
    for (i = 0; i < GrayScale; i++)     // i作为阈值
    {
        w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
        for (j = 0; j < GrayScale; j++)
        {
            if (j <= i)   //背景部分  
            {
                w0 += pixelPro[j];
                u0tmp += j * pixelPro[j];
            }
            else   //前景部分  
            {
                w1 += pixelPro[j];
                u1tmp += j * pixelPro[j];
            }
        }
        u0 = u0tmp / w0;
        u1 = u1tmp / w1;
        u = u0tmp + u1tmp;
        deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = i;
        }
    }
    return threshold;
}

void CamPictureGet()
{
    
    imageRaw.data = mt9v03x_csi_image[0];
    imageRaw.height = MT9V03X_CSI_H;
    imageRaw.width = MT9V03X_CSI_W;
    Threshold =  otsuThreshold(imageRaw.data, imageRaw.height, imageRaw.width);
    Threshold = range_protect(Threshold, 0, 255);
    threshold( &imageRaw, &imageAdaptiveThreshold,Threshold,0,255);
}

void Display_Pitcure(uint16 startX,uint16 startY,float times)  //显示二值化图像和中线
{
    int i,j ;

    ips200_displayimage032_zoom1(imageAdaptiveThreshold.data,imageAdaptiveThreshold.width,imageAdaptiveThreshold.height, startX, startY, imageAdaptiveThreshold.width * times, imageAdaptiveThreshold.height * times);

    for(i=length;i>15;i--)//画左边线点（蓝） 右边线点（绿） 中线（红）
    {

        ips200_drawpoint(left_point[i].x * times+2,(nWidth * times)/2-i,BLUE);
        ips200_drawpoint(left_point[i].x * times+3,(nWidth * times)/2-i,BLUE);
//        ips200_drawpoint(left_point[79-i].x+4,i,BLUE);

        ips200_drawpoint(right_point[i].x * times-2, (nWidth * times)/2-i,GREEN);
        ips200_drawpoint(right_point[i].x * times-3, (nWidth * times)/2-i,GREEN);
//        ips200_drawpoint(right_point[79-i].x-4,i,GREEN);

        ips200_drawpoint(mid_point[i].x * times,   (nWidth * times)/2-i,RED);
        ips200_drawpoint(mid_point[i].x * times-1, (nWidth * times)/2-i,RED);
        ips200_drawpoint(mid_point[i].x * times+1, (nWidth * times)/2-i,RED);

        ips200_drawpoint(nWidth/2,i,BLACK);

    }

    for(i=0;i<160;i++)
    {
        ips200_drawpoint(i,(80-first_lose) * times, RED);
    }

//
//    for(i=0;i<(80-first_lose);i++)
//    {
//        ips200_drawpoint(80+i,80-first_lose-i,RED);
//    }
}



void Camer()
{
	if(mt9v03x_csi_finish_flag)
	{
		CamPictureGet();	//获取二值化图像
		edge_line_extract(imageAdaptiveThreshold.data);	//边界获取
		mt9v03x_csi_finish_flag = 0;	//清除采集完成标志位
	}
	
}

//舵机控制偏差
float angle;



// 原图左右边线
AT_DTCM_SECTION_ALIGN(int ipts0[POINTS_MAX_LEN][2], 8);
AT_DTCM_SECTION_ALIGN(int ipts1[POINTS_MAX_LEN][2], 8);
int ipts0_num, ipts1_num;
// 变换后左右边线
AT_DTCM_SECTION_ALIGN(float rpts0[POINTS_MAX_LEN][2], 8);
AT_DTCM_SECTION_ALIGN(float rpts1[POINTS_MAX_LEN][2], 8);
int rpts0_num, rpts1_num;
// 变换后左右边线+滤波
AT_DTCM_SECTION_ALIGN(float rpts0b[POINTS_MAX_LEN][2], 8);
AT_DTCM_SECTION_ALIGN(float rpts1b[POINTS_MAX_LEN][2], 8);
int rpts0b_num, rpts1b_num;
// 变换后左右边线+等距采样
AT_DTCM_SECTION_ALIGN(float rpts0s[POINTS_MAX_LEN][2], 8);
AT_DTCM_SECTION_ALIGN(float rpts1s[POINTS_MAX_LEN][2], 8);
int rpts0s_num, rpts1s_num;
// 左右边线局部角度变化率
AT_DTCM_SECTION_ALIGN(float rpts0a[POINTS_MAX_LEN], 8);
AT_DTCM_SECTION_ALIGN(float rpts1a[POINTS_MAX_LEN], 8);
int rpts0a_num, rpts1a_num;
// 左右边线局部角度变化率+非极大抑制
AT_DTCM_SECTION_ALIGN(float rpts0an[POINTS_MAX_LEN], 8);
AT_DTCM_SECTION_ALIGN(float rpts1an[POINTS_MAX_LEN], 8);
int rpts0an_num, rpts1an_num;
// 左/右中线
AT_DTCM_SECTION_ALIGN(float rptsc0[POINTS_MAX_LEN][2], 8);
AT_DTCM_SECTION_ALIGN(float rptsc1[POINTS_MAX_LEN][2], 8);
int rptsc0_num, rptsc1_num;
// 中线
float (*rpts)[2];
int rpts_num;
// 归一化中线
AT_DTCM_SECTION_ALIGN(float rptsn[POINTS_MAX_LEN][2], 8);
int rptsn_num;


float begin_x = 32;
float begin_y = 167;
float block_size = 7;
float clip_value = 2;
float thres = 140;
float angle_dist = 0.2;
float sample_dist = 0.02;
float pixel_per_meter = 102;
float line_blur_kernel = 7;
float aim_distance = 0.68;




//void process_image() {
//	
//    // 原图找左右边线
//    int x1 = imageRaw.width / 2 - begin_x, y1 = begin_y;
//    ipts0_num = sizeof(ipts0) / sizeof(ipts0[0]);
//    for (; x1 > 0; x1--) if (AT_IMAGE(&imageRaw, x1 - 1, y1) < thres) break;
//    if (AT_IMAGE(&imageRaw, x1, y1) >= thres)
//        findline_lefthand_adaptive(&imageRaw, block_size, clip_value, x1, y1, ipts0, &ipts0_num);
//    else ipts0_num = 0;
//    int x2 = imageRaw.width / 2 + begin_x, y2 = begin_y;
//    ipts1_num = sizeof(ipts1) / sizeof(ipts1[0]);
//    for (; x2 < imageRaw.width - 1; x2++) if (AT_IMAGE(&imageRaw, x2 + 1, y2) < thres) break;
//    if (AT_IMAGE(&imageRaw, x2, y2) >= thres)
//        findline_righthand_adaptive(&imageRaw, block_size, clip_value, x2, y2, ipts1, &ipts1_num);
//    else ipts1_num = 0;

//	// 边线等距采样
//    rpts0s_num = sizeof(rpts0s) / sizeof(rpts0s[0]);
//    resample_points(rpts0b, rpts0b_num, rpts0s, &rpts0s_num, sample_dist * pixel_per_meter);
//    rpts1s_num = sizeof(rpts1s) / sizeof(rpts1s[0]);
//    resample_points(rpts1b, rpts1b_num, rpts1s, &rpts1s_num, sample_dist * pixel_per_meter);
//	
//	
//    // 左右中线跟踪
//    track_leftline(rpts0s, rpts0s_num, rptsc0, (int) round(angle_dist / sample_dist), pixel_per_meter * ROAD_WIDTH / 2);
//    rptsc0_num = rpts0s_num;
//    track_rightline(rpts1s, rpts1s_num, rptsc1, (int) round(angle_dist / sample_dist), pixel_per_meter * ROAD_WIDTH / 2);
//    rptsc1_num = rpts1s_num;
//    
//    
//}

// 当前巡线模式
enum track_type_e track_type = TRACK_RIGHT;

//void CamSJTU()
//{
//	
//	imageRaw.data = mt9v03x_csi_image[0];
//	
//	// 开始处理摄像头图像
//    process_image();    // 边线提取&处理
//	
//	// 预瞄距离,动态效果更佳
//    aim_distance = 0.58;
//	
//	// 单侧线少，切换巡线方向  切外向圆
//    if (rpts0s_num < rpts1s_num / 2 && rpts0s_num < 60) {
//        track_type = TRACK_RIGHT;
//    } else if (rpts1s_num < rpts0s_num / 2 && rpts1s_num < 60) {
//        track_type = TRACK_LEFT;
//    } else if (rpts0s_num < 20 && rpts1s_num > rpts0s_num) {
//        track_type = TRACK_RIGHT;
//    } else if (rpts1s_num < 20 && rpts0s_num > rpts1s_num) {
//        track_type = TRACK_LEFT;
//    }
//    
//	// 中线跟踪
//     if (track_type == TRACK_LEFT) {
//         rpts = rptsc0;
//         rpts_num = rptsc0_num;
//     } else {
//         rpts = rptsc1;
//         rpts_num = rptsc1_num;
//     }
//	 
//	 float cx = 0;
//	 float cy = 0;
//	 // 找最近点(起始点中线归一化)
//        int begin_id = -1;
//	 
//	 
//	 // 中线有点，同时最近点不是最后几个点
//        if (begin_id >= 0 && rpts_num - begin_id >= 3) {
//            // 归一化中线
//            rpts[begin_id][0] = cx;
//            rpts[begin_id][1] = cy;
//            rptsn_num = sizeof(rptsn) / sizeof(rptsn[0]);
//            resample_points(rpts + begin_id, rpts_num - begin_id, rptsn, &rptsn_num, sample_dist * pixel_per_meter);

//            // 远预锚点位置
//            int aim_idx = clip(round(aim_distance / sample_dist), 0, rptsn_num - 1);
//            // 近预锚点位置
//            int aim_idx_near = clip(round(0.25 / sample_dist), 0, rptsn_num - 1);

//            // 计算远锚点偏差值
//            float dx = rptsn[aim_idx][0] - cx;
//            float dy = cy - rptsn[aim_idx][1] + 0.2 * pixel_per_meter;
//            float dn = sqrt(dx * dx + dy * dy);
//            float error = -atan2f(dx, dy) * 180 / PI;
//            assert(!isnan(error));

//            // 若考虑近点远点,可近似构造Stanley算法,避免撞路肩
//            // 计算近锚点偏差值
//            float dx_near = rptsn[aim_idx_near][0] - cx;
//            float dy_near = cy - rptsn[aim_idx_near][1] + 0.2 * pixel_per_meter;
//            float dn_near = sqrt(dx_near * dx_near + dy_near * dy_near);
//            float error_near = -atan2f(dx_near, dy_near) * 180 / PI;
//            assert(!isnan(error_near));

//            // 远近锚点综合考虑
//            //angle = pid_solve(&servo_pid, error * far_rate + error_near * (1 - far_rate));
//            // 根据偏差进行PD计算
//            //float angle = pid_solve(&servo_pid, error);

//            // 纯跟踪算法(只考虑远点)
//            float pure_angle = -atanf(pixel_per_meter * 2 * 0.2 * dx / dn / dn) / PI * 180 / SMOTOR_RATE;
////            angle = pid_solve(&servo_pid, pure_angle);
////            angle = MINMAX(angle, -14.5, 14.5);

//        } else {
//            // 中线点过少(出现问题)，则不控制舵机
//            rptsn_num = 0;
//        }
//	 
//}


// 常规PID
float pid_solve(pid_param_t *pid, float error) {
    pid->out_d = (error - pid->out_p) * pid->low_pass + pid->out_d * (1 - pid->low_pass);
    pid->out_p = error;
    pid->out_i += error;

//    if (pid->ki != 0) pid->out_i = MINMAX(pid->out_i, -pid->i_max / pid->ki, pid->i_max / pid->ki);

    return pid->kp * pid->out_p + pid->ki * pid->out_i + pid->kd * pid->out_d;
}



extern int clip(int x, int low, int up);

AT_ITCM_SECTION_INIT(void clone_image(image_t *img0, image_t *img1)) {
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img0->width == img1->width && img0->height == img1->height);
    assert(img0 != img1 && img0->data != img1->data);

    if (img0->width == img0->step && img1->width == img1->step) {
        memcpy(img1->data, img0->data, img0->width * img0->height);
    } else {
        for (int y = 0; y < img0->height; y++) {
            memcpy(&AT(img1, 0, y), &AT(img0, 0, y), img0->width);
        }
    }
}

AT_ITCM_SECTION_INIT(void clear_image(image_t *img)) {
    assert(img && img->data);
    if (img->width == img->step) {
        memset(img->data, 0, img->width * img->height);
    } else {
        for (int y = 0; y < img->height; y++) {
            memset(&AT(img, 0, y), 0, img->width);
        }
    }
}

// 固定阈值二值化
AT_ITCM_SECTION_INIT(void threshold(image_t *img0, image_t *img1, uint8_t thres, uint8_t low_value, uint8_t high_value)) {
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img0->width == img1->width && img0->height == img1->height);

    // 先遍历y后遍历x比较cache-friendly
    for (int y = 0; y < img0->height; y++) {
        for (int x = 0; x < img0->width; x++) {
                    AT(img1, x, y) = AT(img0, x, y) < thres ? low_value : high_value;
        }
    }
}

// 自适应阈值二值化
AT_ITCM_SECTION_INIT(void adaptive_threshold(image_t *img0, image_t *img1, int block_size, int down_value, uint8_t low_value, uint8_t high_value)) {
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img0->data != img1->data);
    assert(img0->width == img1->width && img0->height == img1->height);
    assert(block_size > 1 && block_size % 2 == 1);

    int half = block_size / 2;
    // 先遍历y后遍历x比较cache-friendly
    for (int y = 0; y < img0->height; y++) {
        for (int x = 0; x < img0->width; x++) {
            int thres_value = 0;
            for (int dy = -half; dy <= half; dy++) {
                for (int dx = -half; dx <= half; dx++) {
                    thres_value += AT_CLIP(img0, x + dx, y + dy);
                }
            }
            thres_value /= block_size * block_size;
            thres_value -= down_value;
                    AT(img1, x, y) = AT(img0, x, y) < thres_value ? low_value : high_value;
        }
    }
}

// 图像逻辑与
AT_ITCM_SECTION_INIT(void image_and(image_t *img0, image_t *img1, image_t *img2)) {
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img2 && img2->data);
    assert(img0->width == img1->width && img0->height == img1->height);
    assert(img0->width == img2->width && img0->height == img2->height);

    // 先遍历y后遍历x比较cache-friendly
    for (int y = 0; y < img0->height; y++) {
        for (int x = 0; x < img0->width; x++) {
                    AT(img2, x, y) = (AT(img0, x, y) == 0 || AT(img1, x, y) == 0) ? 0 : 255;
        }
    }
}

// 图像逻辑或
AT_ITCM_SECTION_INIT(void image_or(image_t *img0, image_t *img1, image_t *img2)) {
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img2 && img2->data);
    assert(img0->width == img1->width && img0->height == img1->height);
    assert(img0->width == img2->width && img0->height == img2->height);

    // 先遍历y后遍历x比较cache-friendly
    for (int y = 0; y < img0->height; y++) {
        for (int x = 0; x < img0->width; x++) {
                    AT(img2, x, y) = (AT(img0, x, y) == 0 && AT(img1, x, y) == 0) ? 0 : 255;
        }
    }
}

// 2x2最小池化(赛道边界是黑色，最小池化可以较好保留赛道边界)
AT_ITCM_SECTION_INIT(void minpool2(image_t *img0, image_t *img1)) {
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img0->width / 2 == img1->width && img0->height / 2 == img1->height);
    assert(img0 != img1 && img0->data != img1->data);

    uint8_t min_value;
    // 先遍历y后遍历x比较cache-friendly
    for (int y = 1; y < img0->height; y += 2) {
        for (int x = 1; x < img0->width; x += 2) {
            min_value = 255;
            if (AT(img0, x, y) < min_value) min_value = AT(img0, x, y);
            if (AT(img0, x - 1, y) < min_value) min_value = AT(img0, x - 1, y);
            if (AT(img0, x, y - 1) < min_value) min_value = AT(img0, x, y - 1);
            if (AT(img0, x - 1, y - 1) < min_value) min_value = AT(img0, x - 1, y - 1);
                    AT(img1, x / 2, y / 2) = min_value;
        }
    }
}

// 图像滤波降噪
AT_ITCM_SECTION_INIT(void blur(image_t *img0, image_t *img1, uint32_t kernel)) {
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img0->width == img1->width && img0->height == img1->height);
    assert(img0 != img1 && img0->data != img1->data);

    // 先遍历y后遍历x比较cache-friendly
    for (int y = 1; y < img0->height - 1; y++) {
        for (int x = 1; x < img0->width - 1; x++) {
                    AT(img1, x, y) = (1 * AT(img0, x - 1, y - 1) + 2 * AT(img0, x, y - 1) + 1 * AT(img0, x + 1, y - 1) +
                                      2 * AT(img0, x - 1, y) + 4 * AT(img0, x, y) + 2 * AT(img0, x + 1, y) +
                                      1 * AT(img0, x - 1, y + 1) + 2 * AT(img0, x, y + 1) + 1 * AT(img0, x + 1, y + 1)) / 16;
        }
    }
}

// 3x3sobel边缘提取
AT_ITCM_SECTION_INIT(void sobel3(image_t *img0, image_t *img1)) {
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img0->width == img1->width && img0->height == img1->height);
    assert(img0 != img1 && img0->data != img1->data);

    int gx, gy;
    // 先遍历y后遍历x比较cache-friendly    
    for (int y = 1; y < img0->height - 1; y++) {
        for (int x = 1; x < img0->width - 1; x++) {
            gx = (-1 * AT(img0, x - 1, y - 1) + 1 * AT(img0, x + 1, y - 1) +
                  -2 * AT(img0, x - 1, y) + 2 * AT(img0, x + 1, y) +
                  -1 * AT(img0, x - 1, y + 1) + 1 * AT(img0, x + 1, y + 1)) / 4;
            gy = (1 * AT(img0, x - 1, y - 1) + 2 * AT(img0, x, y - 1) + 1 * AT(img0, x + 1, y - 1) +
                  -1 * AT(img0, x - 1, y + 1) - 2 * AT(img0, x, y + 1) - 1 * AT(img0, x + 1, y + 1)) / 4;
                    AT(img1, x, y) = (abs(gx) + abs(gy)) / 2;
        }
    }
}

// 3x3腐蚀
AT_ITCM_SECTION_INIT(void erode3(image_t *img0, image_t *img1)) {
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img0->width == img1->width && img0->height == img1->height);
    assert(img0 != img1 && img0->data != img1->data);

    int min_value;
    // 先遍历y后遍历x比较cache-friendly    
    for (int y = 1; y < img0->height - 1; y++) {
        for (int x = 1; x < img0->width - 1; x++) {
            min_value = 255;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (AT(img0, x + dx, y + dy) < min_value) min_value = AT(img0, x + dx, y + dy);
                }
            }
                    AT(img1, x, y) = min_value;
        }
    }
}

// 3x3膨胀
AT_ITCM_SECTION_INIT(void dilate3(image_t *img0, image_t *img1)) {
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img0->width == img1->width && img0->height == img1->height);
    assert(img0 != img1 && img0->data != img1->data);

    int max_value;
    // 先遍历y后遍历x比较cache-friendly    
    for (int y = 1; y < img0->height - 1; y++) {
        for (int x = 1; x < img0->width - 1; x++) {
            max_value = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (AT(img0, x + dx, y + dy) > max_value) max_value = AT(img0, x + dx, y + dy);
                }
            }
                    AT(img1, x, y) = max_value;
        }
    }
}

//
AT_ITCM_SECTION_INIT(void remap(image_t *img0, image_t *img1, fimage_t *mapx, fimage_t *mapy)) {
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(mapx && mapx->data);
    assert(mapy && mapy->data);
    assert(img0 != img1 && img0->data != img1->data);
    assert(img0->width == img1->width && img0->height == img1->height);
    assert(mapx->width == mapy->width && mapx->height == mapy->height);
    assert(img0->width == mapx->width && img0->height == mapx->height);

    // 先遍历y后遍历x比较cache-friendly    
    for (int y = 1; y < img0->height - 1; y++) {
        for (int x = 1; x < img0->width - 1; x++) {
                    AT(img1, x, y) = AT(img0, (int) (AT(mapx, x, y) + 0.5), (int) (AT(mapy, x, y) + 0.5));
        }
    }
}

/* 前进方向定义：
 *   0
 * 3   1
 *   2
 */
AT_DTCM_SECTION_ALIGN_INIT(const int dir_front[4][2], 8) = {{0,  -1},
                                                            {1,  0},
                                                            {0,  1},
                                                            {-1, 0}};
AT_DTCM_SECTION_ALIGN_INIT(const int dir_frontleft[4][2], 8) = {{-1, -1},
                                                                {1,  -1},
                                                                {1,  1},
                                                                {-1, 1}};
AT_DTCM_SECTION_ALIGN_INIT(const int dir_frontright[4][2], 8) = {{1,  -1},
                                                                 {1,  1},
                                                                 {-1, 1},
                                                                 {-1, -1}};

// 左手迷宫巡线
AT_ITCM_SECTION_INIT(void findline_lefthand_adaptive(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num)) {
    assert(img && img->data);
    assert(num && *num >= 0);
    assert(block_size > 1 && block_size % 2 == 1);
    int half = block_size / 2;
    int step = 0, dir = 0, turn = 0;
    while (step < *num && half < x && x < img->width - half - 1 && half < y && y < img->height - half - 1 && turn < 4) {
        int local_thres = 0;
        for (int dy = -half; dy <= half; dy++) {
            for (int dx = -half; dx <= half; dx++) {
                local_thres += AT(img, x + dx, y + dy);
            }
        }
        local_thres /= block_size * block_size;
        local_thres -= clip_value;

        int current_value = AT(img, x, y);
        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
        int frontleft_value = AT(img, x + dir_frontleft[dir][0], y + dir_frontleft[dir][1]);
        if (front_value < local_thres) {
            dir = (dir + 1) % 4;
            turn++;
        } else if (frontleft_value < local_thres) {
            x += dir_front[dir][0];
            y += dir_front[dir][1];
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
        } else {
            x += dir_frontleft[dir][0];
            y += dir_frontleft[dir][1];
            dir = (dir + 3) % 4;
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
        }
    }
    *num = step;
}

// 右手迷宫巡线
AT_ITCM_SECTION_INIT(void findline_righthand_adaptive(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num)) {
    assert(img && img->data);
    assert(num && *num >= 0);
    assert(block_size > 1 && block_size % 2 == 1);
    int half = block_size / 2;
    int step = 0, dir = 0, turn = 0;
    while (step < *num && 0 < x && x < img->width - 1 && 0 < y && y < img->height - 1 && turn < 4) {
        int local_thres = 0;
        for (int dy = -half; dy <= half; dy++) {
            for (int dx = -half; dx <= half; dx++) {
                local_thres += AT(img, x + dx, y + dy);
            }
        }
        local_thres /= block_size * block_size;
        local_thres -= clip_value;

        int current_value = AT(img, x, y);
        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
        int frontright_value = AT(img, x + dir_frontright[dir][0], y + dir_frontright[dir][1]);
        if (front_value < local_thres) {
            dir = (dir + 3) % 4;
            turn++;
        } else if (frontright_value < local_thres) {
            x += dir_front[dir][0];
            y += dir_front[dir][1];
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
        } else {
            x += dir_frontright[dir][0];
            y += dir_frontright[dir][1];
            dir = (dir + 1) % 4;
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
        }
    }
    *num = step;
}

// 折线段近似
AT_ITCM_SECTION_INIT(void approx_lines(int pts[][2], int pts_num, float epsilon, int lines[][2], int *lines_num)) {
    assert(pts);
    assert(epsilon > 0);

    int dx = pts[pts_num - 1][0] - pts[0][0];
    int dy = pts[pts_num - 1][1] - pts[0][1];
    float nx = -dy / sqrtf(dx * dx + dy * dy);
    float ny = dx / sqrtf(dx * dx + dy * dy);
    float max_dist = 0, dist;
    int idx = -1;
    for (int i = 1; i < pts_num - 1; i++) {
        dist = fabs((pts[i][0] - pts[0][0]) * nx + (pts[i][1] - pts[0][1]) * ny);
        if (dist > max_dist) {
            max_dist = dist;
            idx = i;
        }
    }
    if (max_dist >= epsilon) {
        int num1 = *lines_num;
        approx_lines(pts, idx + 1, epsilon, lines, &num1);
        int num2 = *lines_num - num1 - 1;
        approx_lines(pts + idx, pts_num - idx, epsilon, lines + num1 - 1, &num2);
        *lines_num = num1 + num2 - 1;
    } else {
        lines[0][0] = pts[0][0];
        lines[0][1] = pts[0][1];
        lines[1][0] = pts[pts_num - 1][0];
        lines[1][1] = pts[pts_num - 1][1];
        *lines_num = 2;
    }
}

// float类型的折线段近似
AT_ITCM_SECTION_INIT(void approx_lines_f(float pts[][2], int pts_num, float epsilon, float lines[][2], int *lines_num)) {
    assert(pts);
    assert(epsilon > 0);

    int dx = pts[pts_num - 1][0] - pts[0][0];
    int dy = pts[pts_num - 1][1] - pts[0][1];
    float nx = -dy / sqrtf(dx * dx + dy * dy);
    float ny = dx / sqrtf(dx * dx + dy * dy);
    float max_dist = 0, dist;
    int idx = -1;
    for (int i = 1; i < pts_num - 1; i++) {
        dist = fabs((pts[i][0] - pts[0][0]) * nx + (pts[i][1] - pts[0][1]) * ny);
        if (dist > max_dist) {
            max_dist = dist;
            idx = i;
        }
    }
    if (max_dist >= epsilon && *lines_num > 2) {
        int num1 = *lines_num;
        approx_lines_f(pts, idx + 1, epsilon, lines, &num1);
        int num2 = *lines_num - num1 - 1;
        approx_lines_f(pts + idx, pts_num - idx, epsilon, lines + num1 - 1, &num2);
        *lines_num = num1 + num2 - 1;
    } else {
        lines[0][0] = pts[0][0];
        lines[0][1] = pts[0][1];
        lines[1][0] = pts[pts_num - 1][0];
        lines[1][1] = pts[pts_num - 1][1];
        *lines_num = 2;
    }
}

AT_ITCM_SECTION_INIT(void draw_line(image_t *img, int pt0[2], int pt1[2], uint8_t value)) {
    int dx = pt1[0] - pt0[0];
    int dy = pt1[1] - pt0[1];
    if (abs(dx) > abs(dy)) {
        for (int x = pt0[0]; x != pt1[0]; x += (dx > 0 ? 1 : -1)) {
            int y = pt0[1] + (x - pt0[0]) * dy / dx;
                    AT(img, clip(x, 0, img->width - 1), clip(y, 0, img->height - 1)) = value;
        }
    } else {
        for (int y = pt0[1]; y != pt1[1]; y += (dy > 0 ? 1 : -1)) {
            int x = pt0[0] + (y - pt0[1]) * dx / dy;
                    AT(img, clip(x, 0, img->width - 1), clip(y, 0, img->height - 1)) = value;
        }
    }
}

// 计算大津阈值
AT_ITCM_SECTION_INIT(uint16_t getOSTUThreshold(image_t *img, uint8_t MinThreshold, uint8_t MaxThreshold)) {
    uint8_t Histogram[256];
    uint16_t OUSTThreshold = 0;
    uint32_t PixelAmount = 0, Value_Sum = 0;
    uint64_t sigma = 0, maxSigma = 0;
    float w1 = 0, w2 = 0;
    int32_t u1 = 0, u2 = 0;
    uint8_t MinValue = 0, MaxValue = 255;

    //各像素点个数
    uint8_t *ptr = img->data;
    uint8_t *ptrEnd = img->data + img->width * img->height;
    while (ptr != ptrEnd) {
        ++Histogram[*ptr++];
    }

    for (uint8_t m = 0; m < 100; m++) {

        Histogram[m] = 0;
    }

    for (MinValue = 0; Histogram[MinValue] == 0 && MinValue < 255; ++MinValue);
    for (MaxValue = 255; Histogram[MaxValue] == 0 && MaxValue > 0; --MaxValue);

    if (MaxValue == MinValue) return MaxValue;         // 只有一个颜色
    if (MinValue + 1 == MaxValue) return MinValue;        // 只有二个颜色

    if (MinValue < MinThreshold) {
        MinValue = MinThreshold;
    }
    if (MaxValue > MaxThreshold) {
        MaxValue = MaxThreshold;
    }

    uint32_t Pixel_Integral[256] = {0};   //像素积分 
    uint32_t Value_Integral[256] = {0};    //灰度积分
    for (uint8_t i = MinValue; i <= MaxValue; ++i) {
        PixelAmount += Histogram[i];      //像素总数
        Value_Sum += Histogram[i] * i;     //灰度总和
        Pixel_Integral[i] = PixelAmount;
        Value_Integral[i] = Value_Sum;
    }
    for (uint8_t i = MinValue; i < MaxValue + 1; ++i) {
        w1 = (float) Pixel_Integral[i] / PixelAmount;  //前景像素点比例
        w2 = 1 - w1;                               //背景比例
        u1 = (int32_t) (Value_Integral[i] / w1);                   //前景平均灰度
        u2 = (int32_t) ((Value_Sum - Value_Integral[i]) / w2);      //背景平均灰度
        sigma = (uint64_t) (w1 * w2 * (u1 - u2) * (u1 - u2));
        if (sigma >= maxSigma) {
            maxSigma = sigma;
            OUSTThreshold = i;
        } else {
            break;
        }
    }
    return OUSTThreshold;
}

// 点集三角滤波
AT_ITCM_SECTION_INIT(void blur_points(float pts_in[][2], int num, float pts_out[][2], int kernel)) {
    assert(kernel % 2 == 1);
    int half = kernel / 2;
    for (int i = 0; i < num; i++) {
        pts_out[i][0] = pts_out[i][1] = 0;
        for (int j = -half; j <= half; j++) {
            pts_out[i][0] += pts_in[clip(i + j, 0, num - 1)][0] * (half + 1 - abs(j));
            pts_out[i][1] += pts_in[clip(i + j, 0, num - 1)][1] * (half + 1 - abs(j));
        }
        pts_out[i][0] /= (2 * half + 2) * (half + 1) / 2;
        pts_out[i][1] /= (2 * half + 2) * (half + 1) / 2;
    }
}

// 点集等距采样
AT_ITCM_SECTION_INIT(void resample_points(float pts_in[][2], int num1, float pts_out[][2], int *num2, float dist)) {
    if (num1 < 0) {
        *num2 = 0;
        return;
    }
    pts_out[0][0] = pts_in[0][0];
    pts_out[0][1] = pts_in[0][1];
    int len = 1;
    for (int i = 0; i < num1 - 1 && len < *num2; i++) {
        float x0 = pts_in[i][0];
        float y0 = pts_in[i][1];
        float x1 = pts_in[i + 1][0];
        float y1 = pts_in[i + 1][1];

        do {
            float x = pts_out[len - 1][0];
            float y = pts_out[len - 1][1];

            float dx0 = x0 - x;
            float dy0 = y0 - y;
            float dx1 = x1 - x;
            float dy1 = y1 - y;

            float dist0 = sqrt(dx0 * dx0 + dy0 * dy0);
            float dist1 = sqrt(dx1 * dx1 + dy1 * dy1);

            float r0 = (dist1 - dist) / (dist1 - dist0);
            float r1 = 1 - r0;

            if (r0 < 0 || r1 < 0) break;
            x0 = x0 * r0 + x1 * r1;
            y0 = y0 * r0 + y1 * r1;
            pts_out[len][0] = x0;
            pts_out[len][1] = y0;
            len++;
        } while (len < *num2);

    }
    *num2 = len;
}

// 点集局部角度变化率
AT_ITCM_SECTION_INIT(void local_angle_points(float pts_in[][2], int num, float angle_out[], int dist)) {
    for (int i = 0; i < num; i++) {
        if (i <= 0 || i >= num - 1) {
            angle_out[i] = 0;
            continue;
        }
        float dx1 = pts_in[i][0] - pts_in[clip(i - dist, 0, num - 1)][0];
        float dy1 = pts_in[i][1] - pts_in[clip(i - dist, 0, num - 1)][1];
        float dn1 = sqrtf(dx1 * dx1 + dy1 * dy1);
        float dx2 = pts_in[clip(i + dist, 0, num - 1)][0] - pts_in[i][0];
        float dy2 = pts_in[clip(i + dist, 0, num - 1)][1] - pts_in[i][1];
        float dn2 = sqrtf(dx2 * dx2 + dy2 * dy2);
        float c1 = dx1 / dn1;
        float s1 = dy1 / dn1;
        float c2 = dx2 / dn2;
        float s2 = dy2 / dn2;
        angle_out[i] = atan2f(c1 * s2 - c2 * s1, c2 * c1 + s2 * s1);
    }
}

// 角度变化率非极大抑制
AT_ITCM_SECTION_INIT(void nms_angle(float angle_in[], int num, float angle_out[], int kernel)) {
    assert(kernel % 2 == 1);
    int half = kernel / 2;
    for (int i = 0; i < num; i++) {
        angle_out[i] = angle_in[i];
        for (int j = -half; j <= half; j++) {
            if (fabs(angle_in[clip(i + j, 0, num - 1)]) > fabs(angle_out[i])) {
                angle_out[i] = 0;
                break;
            }
        }
    }
}

// 左边线跟踪中线
AT_ITCM_SECTION_INIT(void track_leftline(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist)) {
    for (int i = 0; i < num; i++) {
        float dx = pts_in[clip(i + approx_num, 0, num - 1)][0] - pts_in[clip(i - approx_num, 0, num - 1)][0];
        float dy = pts_in[clip(i + approx_num, 0, num - 1)][1] - pts_in[clip(i - approx_num, 0, num - 1)][1];
        float dn = sqrt(dx * dx + dy * dy);
        dx /= dn;
        dy /= dn;
        pts_out[i][0] = pts_in[i][0] - dy * dist;
        pts_out[i][1] = pts_in[i][1] + dx * dist;
    }
}

// 右边线跟踪中线
AT_ITCM_SECTION_INIT(void track_rightline(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist)) {
    for (int i = 0; i < num; i++) {
        float dx = pts_in[clip(i + approx_num, 0, num - 1)][0] - pts_in[clip(i - approx_num, 0, num - 1)][0];
        float dy = pts_in[clip(i + approx_num, 0, num - 1)][1] - pts_in[clip(i - approx_num, 0, num - 1)][1];
        float dn = sqrt(dx * dx + dy * dy);
        dx /= dn;
        dy /= dn;
        pts_out[i][0] = pts_in[i][0] + dy * dist;
        pts_out[i][1] = pts_in[i][1] - dx * dist;
    }
}

void draw_x(image_t *img, int x, int y, int len, uint8_t value) {
    for (int i = -len; i <= len; i++) {
                AT(img, clip(x + i, 0, img->width - 1), clip(y + i, 0, img->height - 1)) = value;
                AT(img, clip(x - i, 0, img->width - 1), clip(y + i, 0, img->height - 1)) = value;
    }
}

void draw_o(image_t *img, int x, int y, int radius, uint8_t value) {
    for (float i = -PI; i <= PI; i += PI / 10) {
                AT(img, clip(x + radius * cosf(i), 0, img->width - 1), clip(y + radius * sinf(i), 0, img->height - 1)) = value;
    }
}


            

