//#include"command.h"
//extern float Angle[4];      // 角度环PID{0.2, 0, 0.03, 1000};
//extern float Ang_Vel[4];   // 角速度环PID {0.2, 0.007, 0, 500};
//extern float Vel[4];  // 速度环PID {0.2, 0.007, 0, 500};
//extern float rubo_Vel[4];//转向角度环
//extern float Direct[4];	// 转向角速度环PID 位置{0.018, 0.00215, 0.023, 55};
//void setting_pid()
//{
//    rt_kprintf("enter setting pid mode!\n");
//    rt_kprintf("enter setting pid mode!\n");
//    rt_kprintf("enter setting pid mode!\n");
//    rt_kprintf("enter setting pid mode!\n");
//    char cmd[100];int cnt=0;
//    while(1)
//    {   char ch;
//        ch=getchar();
//        if(ch)
//        {
//					if(ch!='\n')
//					{
//          cmd[cnt]=ch;
//					cnt++;
//					}
//					else
//					{
//						cmd[cnt]='\0';
//						 printf("cmd%s\n",cmd);
//						cnt=0;
//						cmd[0]='\0';
//					
//					}
//        }
//    }
//}
