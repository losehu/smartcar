 //�Զ�ֵ��֮���ͼ����д���Ѱ������
 #include "pictureDeal.h"
 
 
 
 /********�������(1.3mǰհ)**********/
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
 
 /********���߲���б��**********/
uint8 k_tiaojie=7;//(��������)
uint8 k_tiaojie_huandao=6;//(Ԥ��������)
uint8 k_tiaojie_huandao1=6;//(��������)
uint8 k_tiaojie_huandao2=7;//(��������)
uint8 k_forkroad = 7;
 /********ɨ�߱���(�ṹ��)**********/
Point_info_t left_point[nHeight];           //���ߵ����
Point_info_t right_point[nHeight];          //���ߵ����
Point_info_t mid_point[nHeight];            //���ߵ����
static int16 last_mid_point;

/********��Ч��**********/
int8  length=nHeight;
uint8 length_old_end;
int   length_old;
uint8 length_flag;

 
/********��������**********/
float k_right[nHeight]={0};  //  ���ֶ�ֵ80��Ŀǰ��������Ϊ����nHeight����80�����˸ĳɺ궨����
float k_left[nHeight]={0};   //  ���ֶ�ֵ80��Ŀǰ��������Ϊ����nHeight����80�����˸ĳɺ궨����
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
int first_left_point=nHeight;//����ҵ��ĵ�һ����        //nHeight80
int first_right_point=nHeight;//�ұ��ҵ��ĵ�һ����
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

/********����**********/
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

/********ʮ��**********/
int HOW;
uint8 crossroad=0;
uint8 crossroad_flag = 0 ;
uint8 crossroad_ready=0;
int16 crossroad_count  =0;
int buzzer_flag =0 ;

/********����·��(����)**********/
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



/**�߽���Ϣ��ȡ----ͼ��������**/
void edge_line_extract(uint8 *src)
{

    int16 i,j;
    uint8 length_flag=0;
    int first_lose_flag = 0;


    /**ͼ�������Ϣ**/
    all_lose_count=0;                   //�����������
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
    
        //����·��ʹ�ò���
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
    
     /*****����*****/
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


   
     /**ͼ�����ʮ������ȥ��Ѱ�ҵ�ʮ�����е�**/
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

    last_mid_first=first_line_sum/first_line_num;  //������������ֵ �ۼư׵����꼰����
    first_line_num=0;
    first_line_sum=0;
    last_mid_point=last_mid_first;

    if(last_mid_point==BLACKFORCAM)               //�����һ��ȫΪ�ھͽ���ֵ��Ϊ80
    {
        last_mid_point=nWidth/2;//���Դ���������ͼ��ʧ
    }
    
    
    //����·��ʹ�ò���
    for(int i = 75;i>0 ; i-- )
    {
        if(imageAdaptiveThreshold.data[i*nWidth+nWidth/2] == 0)
        {
            white_first = i;//�м��һ���ڵ����ڵ�����
            break;
        }
     }

    for(int i = 80;i<159;i++)
    {
        if(imageAdaptiveThreshold.data[white_first*nWidth+i]==BLACKFORCAM)
        white_num_you++;//ɨ�߽���
    }

    for(int i = 80;i>0;i--)
    {
        if(imageAdaptiveThreshold.data[white_first*nWidth+i]==BLACKFORCAM)
            white_num_zuo++;
        if(imageAdaptiveThreshold.data[i*nWidth+124]==WHITEFORCAM)
            white_num_130++;
        if(imageAdaptiveThreshold.data[i*nWidth+64]==WHITEFORCAM)
            white_num_30++;
    }//ɨ�߽���
    

    mid_point[14].x = last_mid_point;
    
    int K ;
    /**�߽���������**/  //nHeight = 120  Data:2021.8.30
    int limit[nHeight] = {0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,20,20,20,20,20,
                     20,20,20,20,20,25,25,25,25,25,
                     25,25,25,25,25,30,30,30,30,30,
                     30,30,30,30,30,30,30,30,30,30,
                     30,30,30,30,30,30,30,30,30,30,
                    };

                    
         
    /*****����ɨ��forѭ����ʼ*****/
    for(K=(nHeight-15);K>0;K--)                          //nHeight=80�߶�
    {

       pimg=src+(K)*nWidth;                           //nWidth=160���
       i=nHeight-K;

        
        

       /**ɨ����**/
       for(j= last_mid_point;j>=0;j--)
       {
           if(BLACKFORCAM==*(pimg+j) && j!=limit[i])                  //pimg+j������Ǻ�ɫ�ͼ�¼����     //��һ���������!!!!!������������
           {                                             //��¼���꣬x��ʾ�߶�
               left_point[i].x=j+1;
               left_point[i].chuli=j+1;
               left_point[i].state=NORMAL;
               left_case=1;

               //���Լ���б��
               if(i>10&&left_point[i-1].state==NORMAL&&left_point[i-2].state==NORMAL)
               {k_left[i]=((float)(left_point[i].x-left_point[i-1].x)+(float)(left_point[i-1].x-left_point[i-2].x)+(float)(left_point[i-2].x-left_point[i-3].x))/3;}
               break;

           }
           else if(j==limit[i])                       //����
           {
               left_point[i].x=0;
               left_point[i].chuli=0;
               left_point[i].state=LOSE;
               left_case=3;

           }
       }
       /**ɨ����**/
       for(j=last_mid_point;j<nWidth;j++)         //��¼right_point    first_right_point   first_right_lose
       {
           if(BLACKFORCAM==*(pimg+j) && j!=(159-limit[i]))                                     //&&*(pimg+j-1)+*(pimg+j-2)+*(pimg+j-3)==0)
           {
               right_point[i].x=j-1;
               right_point[i].chuli=j-1;
               right_case=1;
               right_point[i].state=NORMAL;

               //���Լ���б��
               if(i>10&&right_point[i-1].state==NORMAL&&right_point[i-2].state==NORMAL)
               {k_right[i]=((float)(right_point[i-1].x-right_point[i].x)+(float)(right_point[i-2].x-right_point[i-1].x)+(float)(right_point[i-3].x-right_point[i-2].x))/3;}  

               break;
           }

           else if(j==nWidth-1-limit[i])                  //��ʾfirst_right_lose��û�иı��ǵ�һ�ζ�ʧ
           {
               right_point[i].x=nWidth-1;
               right_point[i].chuli=nWidth-1;
               right_case=0;
               right_point[i].state=LOSE;
           }
         }
       
          /**����������Ϊ����Ԥ��־λ**/
//       if(i<length&&k_left[i]>0&&k_left[i]<=1&&(k_right[i]>=2||k_right[i]==0))  {rightcircle++;}

//       if(i<length&&k_right[i]>0&&k_right[i]<=1&&(k_left[i]>=2||k_left[i]==0))  {leftcircle++;}

//       if(k_right[i]==0)   {left_circlezero++;}
//       if(k_left[i]==0)    {right_circlezero++;}

         
         
         
       //���������ж��Ƿ�Ϊ����ֱ��
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
       
        //3.ȷ���������           ��¼������Ϣ
             if(length_flag==0)
             {
                switch(left_case-right_case)      //�����Ƕ�ÿһ�ж�����һ��
                 {
                 case 0:             //���ұ��߶�ɨ��
                     all_normal_count++;
                     right_normal_count++;
                     left_normal_count++;

                     if(i>35)
                     {
                         all_normal_35up++;  //��ʹ��
                         right_normal_35up++;
                         left_normal_35up++;
                     }
                     if(i>45)
                     {
                         all_normal_45up++;  //��ʹ��
                         right_normal_45up++;
                         left_normal_45up++;
                     }
                     break;
                 case 1:            //ɨ������ߣ���ʧ�ұ���
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
                 case 2:           //��ʧ����ߣ�ɨ���ұ���
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
                 case 3:          //���ұ��߾���ʧ
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
                     switch(left_case-right_case)      //�����Ƕ�ÿһ�ж�����һ��
                     {
                     case 0:             //���ұ��߶�ɨ��
                         if(i<=45&&i>15)
                         {
                             all_normal_15_30++;
                             left_normal_15_30++;
                             right_normal_15_30++;
                         }
                         break;
                     case 1:            //ɨ������ߣ���ʧ�ұ���
                         if(i<=45&&i>15)
                         {
                             left_normal_15_30++;
                             right_lose_15_30++;
                         }
                         break;
                     case 2:           //��ʧ����ߣ�ɨ���ұ���
                         if(i<=45&&i>15)
                         {
                             left_lose_15_30++;
                             right_normal_15_30++;
                         }
                         break;
                     case 3:          //���ұ��߾���ʧ
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

     //����������ж϶�����
     if((right_point[i].x-left_point[i].x)>(distance3[i]+30))
     {
         lose_count++;
         if(first_lose_flag == 0)
         {
             first_lose = i;
             first_lose_flag = 1;
         }
     }
     
       //����Ԥ�ж�
     if((//Զ������Ԥ�жϣ�1��
         (i>75&&length>=70&&crossroad_ready == 0
         &&leftline_45up<10&&left_lose_15_30>=15&&left_lose_count>30&&left_lose_count<=50
         &&rightline>40&&left_lose_35up>7&&right_lose_35up<=5&&right_lose_count<=20)
         ||//����ǰǿ��Ԥ�жϣ�2��
         (((adc.l2.value+adc.l1.value+adc.r1.value+adc.r2.value)>255)
         &&((adc.l2.value+adc.l1.value+adc.r1.value+adc.r2.value)<400)
         &&left_lose_count>right_lose_count))
//         &&ELC_Circle_flag==0&&ELC_Circle_turn==0&&ramp_flag==0&&TFmin_Dist>30  ��Ϊ������� 2021.9.6
        &&ELC_Circle_flag==0&&ELC_Circle_turn==0
     
         &&circle_flag == 0&&circle_ready_r==0&&circle_flag == 0)
       {
//           crossroad_ready = 0;
           crossroad = 0;
           circle_ready_l=1;
           BeepDiDi(2,150);
		   state.element = circle;
       }
     else if((//Զ������Ԥ�жϣ�1��
         (i>75&&length>=70&&crossroad_ready == 0
         &&rightline_45up<10&&leftline>38&&right_lose_count<=50
         &&right_lose_15_30>15&&right_lose_count>30&&right_lose_35up>7&&left_lose_count<=20
         &&left_lose_35up<=5&&left_lose_count<=20)
         ||//����ǰǿ��Ԥ�жϣ�2��
         (((adc.l2.value+adc.l1.value+adc.r1.value+adc.r2.value)>255)
         &&((adc.l2.value+adc.l1.value+adc.r1.value+adc.r2.value)<400)
         &&right_lose_count>left_lose_count))
         &&ELC_Circle_turn==0&&ELC_Circle_flag==0
//         &&ramp_flag==0&&TFmin_Dist>30&&circle_flag == 0  ��Ϊ������� 2021.9.6
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
      (length<70//�������
      ||hd_zuo||hd_you//ʶ�𻷵�����
      ||(left_normal_count>=60&&right_normal_count>=60&&((adc.l2.value+adc.l1.value+adc.r1.value+adc.r2.value)<250))//��ֱ������
      ||(left_lose_count>=55&&right_lose_count>=55&&((adc.l2.value+adc.l1.value+adc.r1.value+adc.r2.value)<250))
//      ||TFmin_Dist<=30)   ��TFmini ע�͵�  ������������� 2021.9.6
     )
      //�µ�����
       )//����������������Ҳ�����ֹ���г���.
     {
         circle_ready_l=0;
         circle_ready_r=0;
		 state.element = normal;
     }
     
     
     
     
     
     
    //����ɨ�߽���
             
         /********������ʮ��*********/
     //ʮ��
//     if(ELC_Circle_flag==0&&ELC_Circle_turn==0&&garage_out_finish_flag==1&&ramp_flag==0)
//     {
//        if(length>=70&&(white_first<=14||white_num_30>=76||white_num_130>=76)&&ramp_lowspeed==0&&  ��������ɾȥ��ramp_lowspeed==0    2021.9.5
            if(length>=70&&(white_first<=14||white_num_30>=76||white_num_130>=76)&&
                
        ((left_lose_15_30>=28&&right_lose_15_30>=28)||(left_lose_15_30>=28&&right_lose_15_30>=22)||(left_lose_15_30>=22&&right_lose_15_30>=28))//�� �� ��
//        &&forkroad_flag_right ==0&& forkroad_flag_right_in ==0&&forkroad_flag_right_out==0
//        &&forkroad_flag_left ==0&& forkroad_flag_left_in ==0&&forkroad_flag_left_out==0
        &&(right_lose_count>=30&&left_lose_count>=30)&& circle_ready_l==0&&HOW != 3&&HOW!=4)  
            
//        &&forkroad_flag_right==0&&ramp_flag==0&&ELC_Circle_flag==0&&ELC_Circle_turn==0)
        {
            crossroad   = 1;    //ʮ��·�ڱ�־λ
            crossroad_ready = 1; //Q���־λ
        }

     if(i>=45&&(left_lose_15_30<=9||right_lose_15_30<=9)&&crossroad   == 1)
         {crossroad = 0 ;}

//     if(crossroad_ready==1&&right_lose_count>=20&&left_lose_count>=20& HOW = 3;)
//         {crossroad = 1;}

     if(i>70&&length>=70&&crossroad==0/*&&crossroad_count>=200*/
           /*&&right_normal_count>=40&&left_normal_count>=40*/&&white_first<=8)
         {crossroad_ready = 0;}

     //������������ ʶ�������   ���׺�ʮ��·�ڷ�������
     //�������棡
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
     
          /*****��������*****/

     //�����ǽ���
     if(ELC_Circle_flag==1||ELC_Circle_turn==1)
     angel_count = ABS(Circle_angle - huandao_angel);


      if(ELC_Circle_flag==1&&ELC_Circle_turn==1)//�ȼ�⻷���ڣ����жϻ�������*/
      {
          crossroad = 0;
          crossroad_ready = 0 ;

//          /****����������****/
          if(circle_chack==0&&hd_zuo==0&&hd_you==0)//ȷ��ֻ���һ��
          {
//              if((left_lose_35up>5||right_lose_35up>5))
//              {
                  if((left_lose_count>right_lose_count)&&left_lose_35up>=15)//��
                  {
//                      BEEP_NO;
                      huandao_angel = Circle_angle ;   
                      hd_zuo=1;
                      hd_you=0;
                      circle_chack=1;
                  }
                  else if((right_lose_count>left_lose_count)&&right_lose_35up>=15)//�һ�
                  {
//                      BEEP_NO;
                      huandao_angel = Circle_angle ;   
                      hd_you=1;
                      hd_zuo=0;
                      circle_chack=1;
                  }

//              }
            }


          /****������ڲ���****/

          if(hd_zuo==1)
          {
              //�����ǽǶȴ���35�� ��������ֹͣ
            if(ELC_Circle_at==0&&ELC_Circle_flag==1&&ELC_Circle_turn==1
               &&angel_count>=30&&ELC_circle_count>=100/*&&length<=50*/)
            {
                servo_sum_flag=1;
                ELC_Circle_turn=0;
                ELC_circle_count=0;
            }
            /****���󻷵�����****/
            if(i<17)
               mid_point[i].x=(left_point[i].x+right_point[i].x)/2;//�ײ�����ֵ
            else if(left_point[i].state==LOSE&&right_point[i].state==LOSE)      //������ʱȫ��
            {mid_point[i].x=mid_point[i-1].x-(all_lose_count+i)/2/4;}
            else if(left_point[i].state==NORMAL && right_point[i].state==NORMAL)  //����������
            {
                if(mid_point[i-1].x<25)
                {
                    mid_point[i].x=0;
//                    length_flag=1;
                }
                else   mid_point[i].x=left_point[i].x+distance3[i]/2;
            }
            else if(right_point[i].state==LOSE)         //�����Ҷ�
            {
                if(mid_point[i-1].x<25)
                {
                    mid_point[i].x=0;
//                    length_flag=1;
                }
                else   mid_point[i].x=left_point[i].x+distance3[i]/2;
            }
            else if(left_point[i].state==LOSE)           //������  ���·��
            {mid_point[i].x=mid_point[i-1].x-(left_lose_count+i)/2/4;}

          }

          else if(hd_you==1)
          {
              //�����ǽ���
              if(ELC_Circle_at==0&&ELC_Circle_flag==1&&ELC_Circle_turn==1
                 &&angel_count>=60&&ELC_circle_count>=100)//��������ʱ���������������������ȶ�����
              {
                  servo_sum_flag=1;
                  ELC_Circle_turn=0;
                  ELC_circle_count=0;
              }
              /****���һ�������****/
              if(i<17)
              {
                  mid_point[i].x=(left_point[i].x+right_point[i].x)/2;
              }//�ײ�����ֵ
              else if(left_point[i].state==LOSE&&right_point[i].state==LOSE)     //�һ���     ������ʱȫ��
              {
                  mid_point[i].x=mid_point[i-1].x+(all_lose_count+i)/2/3;
              }
              else if(left_point[i].state==NORMAL&&right_point[i].state==NORMAL)  //�һ���   ����������
              {
                  if(mid_point[i-1].x>135)
                  {mid_point[i].x=159;
//                  length_flag=1;
                  }
                  else     mid_point[i].x=right_point[i].x-distance3[i]/2;
              }
              else if(left_point[i].state==LOSE)           //�һ���  ������
              {
                  if(mid_point[i-1].x>135)
                  {mid_point[i].x=159;
//                  length_flag=1;
                  }
                  else     mid_point[i].x=right_point[i].x-distance3[i]/2;
              }
              else if(right_point[i].state==LOSE)         //�һ���   �����Ҷ����·��
              {mid_point[i].x=mid_point[i-1].x+(right_lose_count+i)/2/3;}

          }

          else  //��ʱ��δ�жϳ�����,������ǰ��.
          {

              if(i<17)
              {mid_point[i].x=(left_point[i].x+right_point[i].x)/2;}//�ײ�����ֵ
              else if(right_lose_count>left_lose_count)
              {mid_point[i].x=left_point[i].x+distance3[i]/2;}
              else if(right_lose_count<left_lose_count)
              {mid_point[i].x=right_point[i].x-distance3[i]/2;}
              else
              {mid_point[i].x=left_point[i].x+right_point[i].x/2;}

          }
      }

      /********���ڲ�����********/
      else if(ELC_Circle_flag==1&&ELC_Circle_turn==0&&hd_zuo==1)
      {
          //�������жϳ�����־λ
          //С����
          if(angel_count>=190/*&&right_lose_count>=6*/)  {circle_out_flag2=1;}
          //�󻷵�
//          else if(angel_count>=252&&journey>300)  {circle_out_flag2=1;}
          //�����ڲ���
          if(left_point[i].state==LOSE&&right_point[i].state==LOSE)//�ڲ����Ҷ���
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
              else if(right_point[i].chuli-right_point[i-1].chuli>0)//֤�����������������
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
      /********�һ��ڲ�����********/
      else if(ELC_Circle_flag==1&&ELC_Circle_turn==0&&hd_you==1)
      {
          //���������־λ
          if(angel_count>=190/*&&left_lose_count>=6*/)     {circle_out_flag2=1;}//С����
//          else if(angel_count>=252&&journey>300) {circle_out_flag2=1;}//�󻷵�

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
              else if(left_point[i].chuli-left_point[i-1].chuli<0)//֤�����������������
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
      /********�󻷳���********/
      else if(ELC_Circle_flag==0&&ELC_Circle_turn==1&&hd_zuo==1)
      {
          circle_chack=0;//��ֹ���ܶ��ν��뻷��
          if(left_point[i].state==LOSE&&right_point[i].state==LOSE)
          {mid_point[i].x=MT9V03X_CSI_W/2;}
          else
          {mid_point[i].x=right_point[i].x-distance3[i]/2;}
      }
      /********�һ�����********/
      else if(ELC_Circle_flag==0&&ELC_Circle_turn==1&&hd_you==1)//�һ�����
      {
          circle_chack=0;
          if(left_point[i].state==LOSE&&right_point[i].state==LOSE)
          {mid_point[i].x=MT9V03X_CSI_W/2;}
          else
          {mid_point[i].x=left_point[i].x+distance3[i]/2;}
      }
     
     
    /********��������********/

          if(i<17)  
          {
            mid_point[i].x=(left_point[i].x+right_point[i].x)/2;
          }//�ײ�����ֵ��֤ƽ��
          else if(left_point[i].state==LOSE && right_point[i].state==LOSE)
          {
            mid_point[i].x=(left_point[i].x+right_point[i].x)/2;
          }
          else if(left_point[i].state==LOSE)//��ඪʧ
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
          else if(right_point[i].state==LOSE)//�Ҳඪʧ
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
              if(circle_ready_r==1)      // ȥ����else
              {mid_point[i].x=left_point[i].x+distance3[i]/2;}
              else if(circle_ready_l==1)
              {mid_point[i].x=right_point[i].x-distance3[i]/2;}
              else
              {
                mid_point[i].x=(left_point[i].x+right_point[i].x)/2;
              }
          }
      
       //�����޷�
     if(mid_point[i].x<0)     {mid_point[i].x=0;}
     if(mid_point[i].x>159)   {mid_point[i].x=159;}
     if(right_point[i].x>159) {right_point[i].x=159;}
     if(left_point[i].x<0)    {left_point[i].x=0;}
          
     
     
////////////******�����Ч��******///////////////////////
    //��������Ч�����⴦��
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
    //��������Ч������
//    else if(garage_pass == 1 )
//    {
//        length=80;
//    }
    else if(right_point[i].x<(15+left_point[i].x)&&length_flag==0)
        if(right_point[i].x<(15+left_point[i].x)&&length_flag==0)  //ȥ����������һ�е�else
    {
        length=i;
        length_flag=1;
    }
    if(i==79&&length_flag==0)
    {
        length=80;
    }
    
    
}
    /*********************����ɨ��forѭ����ֹ**************************************************************/
    
          /****������־λ����****/
    if(ELC_Circle_flag==0&&ELC_Circle_turn==0)
    {huandao_clear();}
      
      /****ʮ��������·�ڲ���****/
//      forkroad_deal();
}

//���������־λ
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

//����·�ڴ���
void forkroad_deal(void)
{

    //��������·�ڼ��
    //Ѱ������㲹�߷�

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
        HOW = 4;//б��ʮ�ֿ�ʼ���߱�־��������һ������ ��ֻ������һ��
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
