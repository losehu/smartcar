 //对二值化之后的图像进行处理，寻找中线
 #include "pictureDeal.h"
 
 
 
 /********赛道宽度(1.3m前瞻)**********/
 uint8 distance3[80]=
 {
   180,180,180,180,180,159,159,158,156,156,
   155,151,150,148,150,148,146,146,145,144,
   142,140,140,138,136,134,133,132,130,129,
   128,127,125,124,123,121,120,119,117,116,
   113,112,110,108,106,105,104,101,99,98,
   97,95,93,92,90,90,88,86,84,82,
   81,79,78,76,75,73,71,69,67,65,
   63,61,60,59,57,55,53,52,51,50,
 };
 
 /********中线补线斜率**********/
uint8 k_tiaojie=7;//(进环补线)
uint8 k_tiaojie_huandao=6;//(预出环补线)
uint8 k_tiaojie_huandao1=6;//(出环补线)
uint8 k_tiaojie_huandao2=7;//(出环补线)
uint8 k_forkroad = 7;
 /********扫线变量(结构体)**********/
Point_info_t left_point[nHeight];           //左线点情况
Point_info_t right_point[nHeight];          //右线点情况
Point_info_t mid_point[nHeight];            //中线点情况
static int16 last_mid_point;

/********有效行**********/
int8  length=nHeight;
uint8 length_old_end;
int   length_old;
uint8 length_flag;

 
/********赛道环境**********/
float k_right[nHeight]={0};  //  四轮定值80，目前估计是因为他们nHeight就是80，忘了改成宏定义了
float k_left[nHeight]={0};   //  四轮定值80，目前估计是因为他们nHeight就是80，忘了改成宏定义了
int first_lose_flag = 0;
int first_lose = 0;
int16 all_lose_count;
int16 left_lose_count;
int16 right_lose_count;
int16 all_lose_count_old;
int16 all_normal_count;
int16 right_normal_count;
int16 left_normal_count;
int16 left_normal_right_lose_count;
int16 left_lose_right_nomal_count;
int first_left_point=nHeight;//左边找到的第一个点        //nHeight80
int first_right_point=nHeight;//右边找到的第一个点
int left_lose_15;
int right_lose_15;
int left_nomal_15;
int right_nomal_15;
int all_lose_15;
int all_nomal_15;
int left_lose_35up;
int right_lose_35up;
int all_lose_35up;
int all_normal_35up;
int left_normal_35up;
int right_normal_35up;
int left_lose_45up;
int right_lose_45up;
int all_lose_45up;
int all_normal_45up;
int left_normal_45up;
int right_normal_45up;
uint8 all_lose_15_30;
uint8 right_lose_15_30;
uint8 left_lose_15_30;
uint8 all_normal_15_30;
uint8 right_normal_15_30;
uint8 left_normal_15_30;
uint8 leftline;
uint8 rightline;
int8  rightcircle;
int8  leftcircle;
uint8 leftline_45up;
uint8 rightline_45up;
int16 lose_count;

/********环岛**********/
uint8  circle_point;
uint8  circle_flag = 0;
uint16 circle_crossroad = 0;
uint8 circle_ready=0;
uint8 circle_out_flag=0;
uint8 circle_ready_l=0;
uint8 circle_ready_r=0;
int left_circlezero;
int right_circlezero;
int angel_count = 0;
int huandao_angel = 0;
uint8 hd_zuo=0;
uint8 hd_you=0;
uint8 circle_out_flag2=0;
uint8 circle_chack=0;
uint16 circle_out_count=0;
uint16 circle_count = 0;

/********十字**********/
int HOW;
uint8 crossroad=0;
uint8 crossroad_flag = 0 ;
uint8 crossroad_ready=0;
int16 crossroad_count  =0;
int buzzer_flag =0 ;

/********三岔路口(部分)**********/
int white_first;
int white_num_you;
int white_num_zuo;
int white_num_130;
int white_num_135;
int white_num_125;
int white_num_25;
int white_num_30;
int white_num_35;

int white_xiezuo;
int white_xieyou;


static int16 last_mid_first;
static int16 first_line_num,first_line_sum;



/**边界信息提取----图像主函数**/
void edge_line_extract(uint8 *src)
{

    int16 i,j;
    uint8 length_flag=0;
    int first_lose_flag = 0;


    /**图像基础信息**/
    all_lose_count=0;                   //清零计数变量
    left_lose_count=0;
    right_lose_count=0;
    all_normal_count=0;
    right_normal_count=0;
    left_normal_count=0;
    left_normal_right_lose_count=0;
    left_lose_right_nomal_count=0;
    right_lose_15=0;
    left_nomal_15=0;
    right_nomal_15=0;
    all_lose_15=0;
    all_normal_45up=0;
    left_normal_45up=0;
    right_normal_45up=0;
    all_lose_45up=0;
    left_lose_45up=0;
    right_lose_45up=0;
    all_lose_15_30=0;
    right_lose_15_30=0;
    left_lose_15_30=0;
    all_normal_15_30=0;
    right_normal_15_30=0;
    left_normal_15_30=0;
    left_lose_35up=0;
    right_lose_35up=0;
    all_lose_35up=0;
    all_normal_35up=0;
    left_normal_35up=0;
    right_normal_35up=0;
    length_old_end=0;
    lose_count = 0;
    
        //三岔路口使用参数
    white_num_you= 0;
    white_num_zuo= 0;
    white_first   = 0;
    white_num_130 = 0;
    white_num_135 = 0;
    white_num_125 = 0;
    white_num_25  = 0;
    white_num_30  = 0;
    white_num_35  = 0;

    white_xiezuo = 0;
    white_xieyou = 0;
    
     /*****加入*****/
   uint8  *pimg;
   uint8  *pimg1;
   uint8   tmpsrc;

   uint8 left_case,right_case;

   leftline=0;
   rightline=0;
   rightcircle=0;
   leftcircle=0;
   leftline_45up=0;
   rightline_45up=0;
//   left_circlezero = 0;
//   right_circlezero = 0;

//   white_num_80 = 0 ;
//   white_first = 0;


   
     /**图像底下十五行舍去，寻找第十六行中点**/
   pimg1=src+((nHeight-15)*nWidth);
   for(j=0;j<nWidth;j++)
   {
       tmpsrc=*(pimg1+j);
       if(tmpsrc==WHITEFORCAM)
       {
           first_line_num++;
           first_line_sum+=j;
       }
   }

    last_mid_first=first_line_sum/first_line_num;  //求中线样本均值 累计白点坐标及个数
    first_line_num=0;
    first_line_sum=0;
    last_mid_point=last_mid_first;

    if(last_mid_point==BLACKFORCAM)               //如果第一行全为黑就将中值设为80
    {
        last_mid_point=nWidth/2;//可以处理车过急弯图像丢失
    }
    
    
    //三岔路口使用参数
    for(int i = 75;i>0 ; i-- )
    {
        if(imageAdaptiveThreshold.data[i*nWidth+nWidth/2] == 0)
        {
            white_first = i;//中间第一个黑点所在的行数
            break;
        }
     }

    for(int i = 80;i<159;i++)
    {
        if(imageAdaptiveThreshold.data[white_first*nWidth+i]==BLACKFORCAM)
        white_num_you++;//扫线结束
    }

    for(int i = 80;i>0;i--)
    {
        if(imageAdaptiveThreshold.data[white_first*nWidth+i]==BLACKFORCAM)
            white_num_zuo++;
        if(imageAdaptiveThreshold.data[i*nWidth+124]==WHITEFORCAM)
            white_num_130++;
        if(imageAdaptiveThreshold.data[i*nWidth+64]==WHITEFORCAM)
            white_num_30++;
    }//扫线结束
    

    mid_point[14].x = last_mid_point;
    
    int K ;
    /**边界限制条件**/  //nHeight = 120  Data:2021.8.30
    int limit[nHeight] = {0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,20,20,20,20,20,
                     20,20,20,20,20,25,25,25,25,25,
                     25,25,25,25,25,30,30,30,30,30,
                     30,30,30,30,30,30,30,30,30,30,
                     30,30,30,30,30,30,30,30,30,30,
                    };

                    
         
    /*****整体扫线for循环开始*****/
    for(K=(nHeight-15);K>0;K--)                          //nHeight=80高度
    {

       pimg=src+(K)*nWidth;                           //nWidth=160宽度
       i=nHeight-K;

        
        

       /**扫左线**/
       for(j= last_mid_point;j>=0;j--)
       {
           if(BLACKFORCAM==*(pimg+j) && j!=limit[i])                  //pimg+j里面的是黑色就记录下来     //这一行数组溢出!!!!!！！！！！！
           {                                             //记录坐标，x表示高度
               left_point[i].x=j+1;
               left_point[i].chuli=j+1;
               left_point[i].state=NORMAL;
               left_case=1;

               //粗略计算斜率
               if(i>10&&left_point[i-1].state==NORMAL&&left_point[i-2].state==NORMAL)
               {k_left[i]=((float)(left_point[i].x-left_point[i-1].x)+(float)(left_point[i-1].x-left_point[i-2].x)+(float)(left_point[i-2].x-left_point[i-3].x))/3;}
               break;

           }
           else if(j==limit[i])                       //丢线
           {
               left_point[i].x=0;
               left_point[i].chuli=0;
               left_point[i].state=LOSE;
               left_case=3;

           }
       }
       /**扫右线**/
       for(j=last_mid_point;j<nWidth;j++)         //记录right_point    first_right_point   first_right_lose
       {
           if(BLACKFORCAM==*(pimg+j) && j!=(159-limit[i]))                                     //&&*(pimg+j-1)+*(pimg+j-2)+*(pimg+j-3)==0)
           {
               right_point[i].x=j-1;
               right_point[i].chuli=j-1;
               right_case=1;
               right_point[i].state=NORMAL;

               //粗略计算斜率
               if(i>10&&right_point[i-1].state==NORMAL&&right_point[i-2].state==NORMAL)
               {k_right[i]=((float)(right_point[i-1].x-right_point[i].x)+(float)(right_point[i-2].x-right_point[i-1].x)+(float)(right_point[i-3].x-right_point[i-2].x))/3;}  

               break;
           }

           else if(j==nWidth-1-limit[i])                  //表示first_right_lose还没有改变是第一次丢失
           {
               right_point[i].x=nWidth-1;
               right_point[i].chuli=nWidth-1;
               right_case=0;
               right_point[i].state=LOSE;
           }
         }
       
          /**这里用来作为环岛预标志位**/
//       if(i<length&&k_left[i]>0&&k_left[i]<=1&&(k_right[i]>=2||k_right[i]==0))  {rightcircle++;}

//       if(i<length&&k_right[i]>0&&k_right[i]<=1&&(k_left[i]>=2||k_left[i]==0))  {leftcircle++;}

//       if(k_right[i]==0)   {left_circlezero++;}
//       if(k_left[i]==0)    {right_circlezero++;}

         
         
         
       //这里用来判断是否为正常直道
       if(i<length&&k_left[i]>0.3&&k_left[i]<1.4)
       {
         leftline++;
         if(i>45)  {leftline_45up++;}

       }

       if(i<length&&k_right[i]>0.3&&k_right[i]<1.4)
       {
         rightline++;
         if(i>45)  {rightline_45up++;}

       }
       
        //3.确定该行情况           记录赛道信息
             if(length_flag==0)
             {
                switch(left_case-right_case)      //这里是对每一行都处理一次
                 {
                 case 0:             //左右边线都扫到
                     all_normal_count++;
                     right_normal_count++;
                     left_normal_count++;

                     if(i>35)
                     {
                         all_normal_35up++;  //有使用
                         right_normal_35up++;
                         left_normal_35up++;
                     }
                     if(i>45)
                     {
                         all_normal_45up++;  //有使用
                         right_normal_45up++;
                         left_normal_45up++;
                     }
                     break;
                 case 1:            //扫到左边线，丢失右边线
                     right_lose_count++;
                     left_normal_count++;
                     left_normal_right_lose_count++;
                     if(i>35)
                     {
                         right_lose_35up++;
                         left_normal_35up++;
                     }
                     if(i>45)
                     {
                         right_lose_45up++;
                         left_normal_45up++;
                     }
                     break;
                 case 2:           //丢失左边线，扫到右边线
                     left_lose_count++;
                     right_normal_count++;
                     left_lose_right_nomal_count++;
                     if(i>35)
                     {
                         left_lose_35up++;
                         right_normal_35up++;
                     }
                     if(i>45)
                     {
                         left_lose_45up++;
                         right_normal_45up++;
                     }
                     break;
                 case 3:          //左右边线均丢失
                     left_lose_count++;
                     right_lose_count++;
                     all_lose_count++;

                     if(i>35)
                     {
                         left_lose_35up++;
                         right_lose_35up++;
                         all_lose_35up++;
                     }
                     if(i>45)
                     {
                         left_lose_45up++;
                         right_lose_45up++;
                         all_lose_45up++;
                     }
                     break;
                 }

                 if(i<55&&i>5)
                 {
                     switch(left_case-right_case)      //这里是对每一行都处理一次
                     {
                     case 0:             //左右边线都扫到
                         if(i<=45&&i>15)
                         {
                             all_normal_15_30++;
                             left_normal_15_30++;
                             right_normal_15_30++;
                         }
                         break;
                     case 1:            //扫到左边线，丢失右边线
                         if(i<=45&&i>15)
                         {
                             left_normal_15_30++;
                             right_lose_15_30++;
                         }
                         break;
                     case 2:           //丢失左边线，扫到右边线
                         if(i<=45&&i>15)
                         {
                             left_lose_15_30++;
                             right_normal_15_30++;
                         }
                         break;
                     case 3:          //左右边线均丢失
                         if(i<=45&&i>15)
                         {
                             all_lose_15_30++;
                             left_lose_15_30++;
                             right_lose_15_30++;
                         }
                         break;
                     }
                 }
                 
            }

     //用赛道宽度判断丢线数
     if((right_point[i].x-left_point[i].x)>(distance3[i]+30))
     {
         lose_count++;
         if(first_lose_flag == 0)
         {
             first_lose = i;
             first_lose_flag = 1;
         }
     }
     
       //环岛预判断
     if((//远处环岛预判断（1）
         (i>75&&length>=70&&crossroad_ready == 0
         &&leftline_45up<10&&left_lose_15_30>=15&&left_lose_count>30&&left_lose_count<=50
         &&rightline>40&&left_lose_35up>7&&right_lose_35up<=5&&right_lose_count<=20)
         ||//环岛前强行预判断（2）
         (((adc.l2.value+adc.l1.value+adc.r1.value+adc.r2.value)>255)
         &&((adc.l2.value+adc.l1.value+adc.r1.value+adc.r2.value)<400)
         &&left_lose_count>right_lose_count))
//         &&ELC_Circle_flag==0&&ELC_Circle_turn==0&&ramp_flag==0&&TFmin_Dist>30  改为下面这个 2021.9.6
        &&ELC_Circle_flag==0&&ELC_Circle_turn==0
     
         &&circle_flag == 0&&circle_ready_r==0&&circle_flag == 0)
       {
//           crossroad_ready = 0;
           crossroad = 0;
           circle_ready_l=1;
           BeepDiDi(2,150);
		   state.element = circle;
       }
     else if((//远处环岛预判断（1）
         (i>75&&length>=70&&crossroad_ready == 0
         &&rightline_45up<10&&leftline>38&&right_lose_count<=50
         &&right_lose_15_30>15&&right_lose_count>30&&right_lose_35up>7&&left_lose_count<=20
         &&left_lose_35up<=5&&left_lose_count<=20)
         ||//环岛前强行预判断（2）
         (((adc.l2.value+adc.l1.value+adc.r1.value+adc.r2.value)>255)
         &&((adc.l2.value+adc.l1.value+adc.r1.value+adc.r2.value)<400)
         &&right_lose_count>left_lose_count))
         &&ELC_Circle_turn==0&&ELC_Circle_flag==0
//         &&ramp_flag==0&&TFmin_Dist>30&&circle_flag == 0  改为下面这个 2021.9.6
        &&circle_flag == 0
         &&circle_ready_l==0&&circle_flag == 0)
       {
//           crossroad_ready = 0;
           crossroad = 0;
           circle_ready_r=1;
           BeepDiDi(2,150);
		   state.element = circle;
       }


     if((circle_ready_l==1||circle_ready_r==1)&&
      (length<70//弯道清零
      ||hd_zuo||hd_you//识别环岛清零
      ||(left_normal_count>=60&&right_normal_count>=60&&((adc.l2.value+adc.l1.value+adc.r1.value+adc.r2.value)<250))//大直道清零
      ||(left_lose_count>=55&&right_lose_count>=55&&((adc.l2.value+adc.l1.value+adc.r1.value+adc.r2.value)<250))
//      ||TFmin_Dist<=30)   无TFmini 注释掉  用下面这个括号 2021.9.6
     )
      //坡道清零
       )//进环清零或遇到弯道也清零防止误判出错.
     {
         circle_ready_l=0;
         circle_ready_r=0;
		 state.element = normal;
     }
     
     
     
     
     
     
    //整体扫线结束
             
         /********三叉与十字*********/
     //十字
//     if(ELC_Circle_flag==0&&ELC_Circle_turn==0&&garage_out_finish_flag==1&&ramp_flag==0)
//     {
//        if(length>=70&&(white_first<=14||white_num_30>=76||white_num_130>=76)&&ramp_lowspeed==0&&  下面这行删去了ramp_lowspeed==0    2021.9.5
            if(length>=70&&(white_first<=14||white_num_30>=76||white_num_130>=76)&&
                
        ((left_lose_15_30>=28&&right_lose_15_30>=28)||(left_lose_15_30>=28&&right_lose_15_30>=22)||(left_lose_15_30>=22&&right_lose_15_30>=28))//中 左 右
//        &&forkroad_flag_right ==0&& forkroad_flag_right_in ==0&&forkroad_flag_right_out==0
//        &&forkroad_flag_left ==0&& forkroad_flag_left_in ==0&&forkroad_flag_left_out==0
        &&(right_lose_count>=30&&left_lose_count>=30)&& circle_ready_l==0&&HOW != 3&&HOW!=4)  
            
//        &&forkroad_flag_right==0&&ramp_flag==0&&ELC_Circle_flag==0&&ELC_Circle_turn==0)
        {
            crossroad   = 1;    //十字路口标志位
            crossroad_ready = 1; //Q弯标志位
        }

     if(i>=45&&(left_lose_15_30<=9||right_lose_15_30<=9)&&crossroad   == 1)
         {crossroad = 0 ;}

//     if(crossroad_ready==1&&right_lose_count>=20&&left_lose_count>=20& HOW = 3;)
//         {crossroad = 1;}

     if(i>70&&length>=70&&crossroad==0/*&&crossroad_count>=200*/
           /*&&right_normal_count>=40&&left_normal_count>=40*/&&white_first<=8)
         {crossroad_ready = 0;}

     //满足所有条件 识别到三岔口   容易和十字路口发生误判
     //两个三叉！
//     if(white_num_130<=70&&white_num_30<=70&&length<=65&&length>=30&&crossroad_ready==0&&crossroad==0
//        &&(white_num_you<=10||white_num_zuo<=10)&&white_first<42&&white_first>=20
//        &&((right_lose_count>=35&&left_lose_count>=35)||(right_lose_count>=36&&left_lose_count>=15)||(right_lose_count>=15&&left_lose_count>=36))
//        && ((left_lose_15_30>=28&&right_lose_15_30>=28)||(left_lose_15_30>=28&&right_lose_15_30<=2)||(left_lose_15_30<2&&right_lose_15_30>=28))
//        &&((right_circlezero>=10&&left_circlezero>=10)||(right_circlezero>=18&&left_circlezero>=5)||(right_circlezero>=5&&left_circlezero>=18))
//        &&forkroad_far ==0&&forkroad_no==0)
//     {

//         if(((forkroad_flag_first ==0&&forkroad_flag_right_finish==0)
//            ||(forkroad_flag_first ==1&&forkroad_flag_right_finish_1==0&&forkroad_flag_right_finish==1))
//            &&forkroad_flag_right_in==0
//            &&forkroad_flag_right_out==0)
//         {
//             forkroad_flag_right = 1;
//             forkroad_far = 0;
//             forkroad_near = 1;

//             if(forkroad_flag_first ==0)
//            forkroad_flag_first = 1;
//            else if(forkroad_flag_first ==1)
//            forkroad_flag_first = 2;

//             if(forkroad_angel==0)
//             forkroad_angel = Circle_angle;
//         }
//         else if(((forkroad_flag_first == 2 && forkroad_flag_right_finish_1==1)
//                       ||(forkroad_flag_first == 3 && forkroad_flag_left_finish ==1&& forkroad_flag_left_finish_1 ==0))
//                       &&forkroad_flag_left_in==0&&forkroad_flag_left_out==0
//                       &&forkroad_flag_right_in==0&&forkroad_flag_right_out==0)

//         {
//             forkroad_flag_left = 1;
//             forkroad_far = 0;
//             forkroad_near = 1;

//             if(forkroad_flag_first == 2)
//             forkroad_flag_first = 3;
//             else if(forkroad_flag_first == 3)
//             forkroad_flag_first = 4;

//             if(forkroad_angel==0)
//             forkroad_angel = Circle_angle;
//         }
//     }
     
          /*****环岛补线*****/

     //陀螺仪进环
     if(ELC_Circle_flag==1||ELC_Circle_turn==1)
     angel_count = ABS(Circle_angle - huandao_angel);


      if(ELC_Circle_flag==1&&ELC_Circle_turn==1)//先检测环岛口，再判断环岛方向*/
      {
          crossroad = 0;
          crossroad_ready = 0 ;

//          /****环岛方向检测****/
          if(circle_chack==0&&hd_zuo==0&&hd_you==0)//确保只检查一次
          {
//              if((left_lose_35up>5||right_lose_35up>5))
//              {
                  if((left_lose_count>right_lose_count)&&left_lose_35up>=15)//左环
                  {
//                      BEEP_NO;
                      huandao_angel = Circle_angle ;   
                      hd_zuo=1;
                      hd_you=0;
                      circle_chack=1;
                  }
                  else if((right_lose_count>left_lose_count)&&right_lose_35up>=15)//右环
                  {
//                      BEEP_NO;
                      huandao_angel = Circle_angle ;   
                      hd_you=1;
                      hd_zuo=0;
                      circle_chack=1;
                  }

//              }
            }


          /****环岛入口补线****/

          if(hd_zuo==1)
          {
              //陀螺仪角度大于35° 进环补线停止
            if(ELC_Circle_at==0&&ELC_Circle_flag==1&&ELC_Circle_turn==1
               &&angel_count>=30&&ELC_circle_count>=100/*&&length<=50*/)
            {
                servo_sum_flag=1;
                ELC_Circle_turn=0;
                ELC_circle_count=0;
            }
            /****进左环岛补线****/
            if(i<17)
               mid_point[i].x=(left_point[i].x+right_point[i].x)/2;//底部赋中值
            else if(left_point[i].state==LOSE&&right_point[i].state==LOSE)      //进环岛时全丢
            {mid_point[i].x=mid_point[i-1].x-(all_lose_count+i)/2/4;}
            else if(left_point[i].state==NORMAL && right_point[i].state==NORMAL)  //进环都正常
            {
                if(mid_point[i-1].x<25)
                {
                    mid_point[i].x=0;
//                    length_flag=1;
                }
                else   mid_point[i].x=left_point[i].x+distance3[i]/2;
            }
            else if(right_point[i].state==LOSE)         //进环右丢
            {
                if(mid_point[i-1].x<25)
                {
                    mid_point[i].x=0;
//                    length_flag=1;
                }
                else   mid_point[i].x=left_point[i].x+distance3[i]/2;
            }
            else if(left_point[i].state==LOSE)           //进环左丢  拟合路线
            {mid_point[i].x=mid_point[i-1].x-(left_lose_count+i)/2/4;}

          }

          else if(hd_you==1)
          {
              //陀螺仪进环
              if(ELC_Circle_at==0&&ELC_Circle_flag==1&&ELC_Circle_turn==1
                 &&angel_count>=60&&ELC_circle_count>=100)//陀螺仪有时会产生波动，加入计数，稳定进环
              {
                  servo_sum_flag=1;
                  ELC_Circle_turn=0;
                  ELC_circle_count=0;
              }
              /****进右环岛补线****/
              if(i<17)
              {
                  mid_point[i].x=(left_point[i].x+right_point[i].x)/2;
              }//底部赋中值
              else if(left_point[i].state==LOSE&&right_point[i].state==LOSE)     //右环岛     进环岛时全丢
              {
                  mid_point[i].x=mid_point[i-1].x+(all_lose_count+i)/2/3;
              }
              else if(left_point[i].state==NORMAL&&right_point[i].state==NORMAL)  //右环岛   进环都正常
              {
                  if(mid_point[i-1].x>135)
                  {mid_point[i].x=159;
//                  length_flag=1;
                  }
                  else     mid_point[i].x=right_point[i].x-distance3[i]/2;
              }
              else if(left_point[i].state==LOSE)           //右环岛  进环左丢
              {
                  if(mid_point[i-1].x>135)
                  {mid_point[i].x=159;
//                  length_flag=1;
                  }
                  else     mid_point[i].x=right_point[i].x-distance3[i]/2;
              }
              else if(right_point[i].state==LOSE)         //右环岛   进环右丢拟合路线
              {mid_point[i].x=mid_point[i-1].x+(right_lose_count+i)/2/3;}

          }

          else  //此时还未判断出方向,继续往前跑.
          {

              if(i<17)
              {mid_point[i].x=(left_point[i].x+right_point[i].x)/2;}//底部赋中值
              else if(right_lose_count>left_lose_count)
              {mid_point[i].x=left_point[i].x+distance3[i]/2;}
              else if(right_lose_count<left_lose_count)
              {mid_point[i].x=right_point[i].x-distance3[i]/2;}
              else
              {mid_point[i].x=left_point[i].x+right_point[i].x/2;}

          }
      }

      /********左环内部补线********/
      else if(ELC_Circle_flag==1&&ELC_Circle_turn==0&&hd_zuo==1)
      {
          //陀螺仪判断出环标志位
          //小环岛
          if(angel_count>=190/*&&right_lose_count>=6*/)  {circle_out_flag2=1;}
          //大环岛
//          else if(angel_count>=252&&journey>300)  {circle_out_flag2=1;}
          //环岛内补线
          if(left_point[i].state==LOSE&&right_point[i].state==LOSE)//内部左右都丢
          {
              if(i<18)  mid_point[i].x=nWidth/2;
//              else if(circle_out_flag2==1)
//                    mid_point[i].x=mid_point[i-1].x-(all_lose_count+i)/2/4;
          }
          else if(left_point[i].state==NORMAL&&right_point[i].state==LOSE)
          {
               if(left_point[i].x-left_point[i-1].x>0)
                  mid_point[i].x=mid_point[i-1].x-(all_lose_count+i)/2/4;
               else
               mid_point[i].x=left_point[i].x+distance3[i]/2;
          }
          else if(left_point[i].state==LOSE&&right_point[i].state==NORMAL)
          {
              if(i<17)
              {mid_point[i].x=right_point[i].x-distance3[i]/2;}
//              else if(circle_out_flag2==1)
//              mid_point[i].x=mid_point[i-1].x-(all_lose_count+i)/2/4;
              else if(right_point[i].chuli-right_point[i-1].chuli>0)//证明补到出环口外侧了
              {mid_point[i].x=right_point[i].x-distance3[i]/2;}
              else if(right_point[i].chuli==right_point[i-1].chuli)
              {mid_point[i].x=mid_point[i-1].x;}
              else
              {mid_point[i].x=right_point[i].x-distance3[i]/2;}
          }
          else if(left_point[i].state==NORMAL&&right_point[i].state==NORMAL)
          {
              if(i<17)
              {mid_point[i].x=(left_point[i].x+right_point[i].x)/2;}
//              else if(circle_out_flag2==1)
//              mid_point[i].x=mid_point[i-1].x-(all_lose_count+i)/2/4;
              else
              {mid_point[i].x=(left_point[i].x+right_point[i].x)/2;}
          }
      }
      /********右环内部补线********/
      else if(ELC_Circle_flag==1&&ELC_Circle_turn==0&&hd_you==1)
      {
          //出环岛大标志位
          if(angel_count>=190/*&&left_lose_count>=6*/)     {circle_out_flag2=1;}//小环岛
//          else if(angel_count>=252&&journey>300) {circle_out_flag2=1;}//大环岛

          if(left_point[i].state==LOSE&&right_point[i].state==LOSE)
          {
//             if(circle_out_flag2==1)
//                  mid_point[i].x=mid_point[i-1].x+(all_lose_count+i)/2/4;
//             else
              mid_point[i].x=nWidth/2;
          }
          else if(left_point[i].state==LOSE&&right_point[i].state==NORMAL)
          {
             if(right_point[i].x-right_point[i-1].x<0)
             mid_point[i].x=mid_point[i-1].x+(all_lose_count+i)/2/4;
             else
              mid_point[i].x=right_point[i].x-distance3[i]/2;
          }
          else if(left_point[i].state==NORMAL&&right_point[i].state==LOSE)
          {
              if(i<17)
              {mid_point[i].x=left_point[i].x+distance3[i]/2;}
//              else if(circle_out_flag2==1)
//              mid_point[i].x=mid_point[i-1].x+(all_lose_count+i)/2/4;
              else if(left_point[i].chuli-left_point[i-1].chuli<0)//证明补到出环口外侧了
              { mid_point[i].x=left_point[i].x+distance3[i]/2;}
              else if(left_point[i].chuli==left_point[i-1].chuli)
              {mid_point[i].x=mid_point[i-1].x;}
              else
              {mid_point[i].x=left_point[i].x+distance3[i]/2;}
          }
          else if(left_point[i].state==NORMAL&&right_point[i].state==NORMAL)
          {
              if(i<17)
              {mid_point[i].x=(left_point[i].x+right_point[i].x)/2;}
//              else if(circle_out_flag2==1)
//              mid_point[i].x=mid_point[i-1].x+(all_lose_count+i)/2/4;
              else
              {mid_point[i].x=(left_point[i].x+right_point[i].x)/2;}
          }
      }
      /********左环出环********/
      else if(ELC_Circle_flag==0&&ELC_Circle_turn==1&&hd_zuo==1)
      {
          circle_chack=0;//防止不能二次进入环岛
          if(left_point[i].state==LOSE&&right_point[i].state==LOSE)
          {mid_point[i].x=MT9V03X_CSI_W/2;}
          else
          {mid_point[i].x=right_point[i].x-distance3[i]/2;}
      }
      /********右环出环********/
      else if(ELC_Circle_flag==0&&ELC_Circle_turn==1&&hd_you==1)//右环出环
      {
          circle_chack=0;
          if(left_point[i].state==LOSE&&right_point[i].state==LOSE)
          {mid_point[i].x=MT9V03X_CSI_W/2;}
          else
          {mid_point[i].x=left_point[i].x+distance3[i]/2;}
      }
     
     
    /********正常补线********/

          if(i<17)  
          {
            mid_point[i].x=(left_point[i].x+right_point[i].x)/2;
          }//底部赋中值保证平滑
          else if(left_point[i].state==LOSE && right_point[i].state==LOSE)
          {
            mid_point[i].x=(left_point[i].x+right_point[i].x)/2;
          }
          else if(left_point[i].state==LOSE)//左侧丢失
          {
              if(circle_ready_l==1/*||forkroad_flag_right_in==1*/)
              {mid_point[i].x=right_point[i].x-distance3[i]/2;}
              else if(right_point[i-1].x>=right_point[i].x)

              {
                mid_point[i].x=mid_point[i-1].x+right_point[i].x-right_point[i-1].x;
              }
              else
              { 
                mid_point[i].x=mid_point[i-1].x;
              }
          }
          else if(right_point[i].state==LOSE)//右侧丢失
          {
              if(circle_ready_r==1/*||forkroad_flag_left_in==1*/)
              {mid_point[i].x=left_point[i].x+distance3[i]/2;}
              else if(left_point[i].x>=left_point[i-1].x)

              {
                mid_point[i].x=mid_point[i-1].x+left_point[i].x-left_point[i-1].x;
              }
              else
              {
                mid_point[i].x=mid_point[i-1].x;
              }
          }
          else
          {
////              if(forkroad_flag_left_in==1)
////              {mid_point[i].x=(left_point[i].x+right_point[i].x)/2;}
////              else if(forkroad_flag_right_in==1)
////              {mid_point[i].x=(left_point[i].x+right_point[i].x)/2;}
              if(circle_ready_r==1)      // 去掉了else
              {mid_point[i].x=left_point[i].x+distance3[i]/2;}
              else if(circle_ready_l==1)
              {mid_point[i].x=right_point[i].x-distance3[i]/2;}
              else
              {
                mid_point[i].x=(left_point[i].x+right_point[i].x)/2;
              }
          }
      
       //边线限幅
     if(mid_point[i].x<0)     {mid_point[i].x=0;}
     if(mid_point[i].x>159)   {mid_point[i].x=159;}
     if(right_point[i].x>159) {right_point[i].x=159;}
     if(left_point[i].x<0)    {left_point[i].x=0;}
          
     
     
////////////******求出有效行******///////////////////////
    //进环岛有效行特殊处理
    if(ELC_Circle_flag==1&&ELC_Circle_turn==1)
    {
        if(hd_zuo==1)
        {
          if(left_point[i].x<=20)
          {
            length=i;
            length_flag=1;
          }
        }
        else if(hd_you==1)
        {
          if(right_point[i].x>=140)
          {
              length = i;
              length_flag=1;
          }
        }
    }
    //起跑线有效行拉满
//    else if(garage_pass == 1 )
//    {
//        length=80;
//    }
    else if(right_point[i].x<(15+left_point[i].x)&&length_flag==0)
        if(right_point[i].x<(15+left_point[i].x)&&length_flag==0)  //去掉了上面这一行的else
    {
        length=i;
        length_flag=1;
    }
    if(i==79&&length_flag==0)
    {
        length=80;
    }
    
    
}
    /*********************整体扫线for循环截止**************************************************************/
    
          /****环岛标志位清零****/
    if(ELC_Circle_flag==0&&ELC_Circle_turn==0)
    {huandao_clear();}
      
      /****十字与三岔路口补线****/
//      forkroad_deal();
}

//清楚环岛标志位
void huandao_clear()
{
      huandao_angel = 0;
      hd_zuo=0;
      hd_you=0;
      circle_chack=0;
      circle_out_flag2=0;
      circle_ready_flag=0;
      angel_count = 0;
}

//三岔路口处理
void forkroad_deal(void)
{

    //黄氏三岔路口检测
    //寻找跳变点补线法

    if(crossroad_ready == 1&&right_lose_15_30>=20&&left_lose_15_30>=20)
    {
//        BeepDiDi(3,300);
        if(right_point[75].x-left_point[75].x>=10)
        {
        lian_xian_zhi(right_point[75].x,75,160,0,1);
        lian_xian_zhi(left_point[75].x,75,0,0,2);
        HOW = 1;
        }
        else if(right_point[70].x-left_point[70].x>=10)
        {
        lian_xian_zhi(right_point[70].x,70,160,0,1);
        lian_xian_zhi(left_point[70].x,70,0,0,2);
        HOW = 1;
        }
        else if(right_point[65].x-left_point[65].x>=10)
        {
            lian_xian_zhi(right_point[65].x,65,160,0,1);
            lian_xian_zhi(left_point[65].x,65,0,0,2);
            HOW = 1;
        }

        if(HOW==1)
        {
        for(int i = length;i>=15;i--)
        {
            mid_point[i].x = (right_point[i].x+left_point[i].x)/2 ;
        }
        }

    }

    if(crossroad_ready==0)
    {
        HOW = 0;
    }

    else if(crossroad_ready==1&&right_lose_15_30>=24&&(left_lose_45up>=10)&&left_lose_15_30<=25
        &&crossroad==0)
    {
//        BeepDiDi(3,300);
        HOW = 4;//斜出十字开始补线标志，比赛朝一个方向 ，只考虑了一种
    }


   if(HOW==3&&white_first>=8)
    {

        lian_xian_zhi(0,80,160,0,1);
      for(int i = length;i>=15;i--)
      {
      mid_point[i].x = (right_point[i].x+left_point[i].x)/2 ;
      }
    }

   else  if(HOW==4&&white_first>=6)
   {

       lian_xian_zhi(160,80,0,0,2);
       for(int i = length;i>=15;i--)
       {
       mid_point[i].x = (right_point[i].x+left_point[i].x)/2 ;
       }
   }

   
}
