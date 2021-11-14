#include "headfile.h"

//struct s1
//{
//    int a;
//    int b;

//    double c;
//};

//struct s2
//{
//    long long a;
//    long long b;
//};
//struct s1 s11;
//struct s2 s22;
//void gcc_overload_s1(struct s1 s, int8 b)
//{
//    PRINTF("Got a struct s1: %d %d %f\n", s.a, s.b, s.c);
//}

//void gcc_overload_s2(struct s2 s, int8 b)
//{
//    PRINTF("Got a struct s2: %lld %lld\n", s.a, s.b);
//}
//void gcc_type_overload_aux(int typeval, ...)
//{
//    switch(typeval)
//    {
//        case 1:
//        {
//            va_list v;
//            va_start(v, typeval);

//            struct s1 s = va_arg(v, struct s1);

//            va_end(v);

//            gcc_overload_s1(s,10);

//            break;
//        }

//        case 2:
//        {
//            va_list v;
//            va_start(v, typeval);

//            struct s2 s = va_arg(v, struct s2);

//            va_end(v);

//            gcc_overload_s2(s,10);

//            break;
//        }

//        default:
//        {
//            PRINTF("Invalid type to 'gcc_type_overload()'\n");
//        }
//    }
//}


#define gcc_type_overload(A,b)\
    gcc_type_overload_aux(\
        __builtin_types_compatible_p(typeof(A), struct s1) * 1\
        + __builtin_types_compatible_p(typeof(A), struct s2) * 2\
        , A,b)
		
/* 注意：
	1：函数重载需要开启：魔法棒——》C++——》language C（两个）——》都改为gnu

*/

/**
 *                             _ooOoo_
 *                            o8888888o
 *                            88" . "88
 *                            (| -_- |)
 *                            O\  =  /O
 *                         ____/`---'\____
 *                       .'  \\|     |//  `.
 *                      /  \\|||  :  |||//  \
 *                     /  _||||| -:- |||||-  \
 *                     |   | \\\  -  /// |   |
 *                     | \_|  ''\---/''  |   |
 *                     \  .-\__  `-`  ___/-. /
 *                   ___`. .'  /--.--\  `. . __
 *                ."" '<  `.___\_<|>_/___.'  >'"".
 *               | | :  `- \`.;`\ _ /`;.`/ - ` : | |
 *               \  \ `-.   \_ __\ /__ _/   .-` /  /
 *          ======`-.____`-.___\_____/___.-`____.-'======
 *                             `=---='
 *          ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 *                     佛祖保佑        永无BUG
 *            佛曰:
 *                   写字楼里写字间，写字间里程序员；
 *                   程序人员写程序，又拿程序换酒钱。
 *                   酒醒只在网上坐，酒醉还来网下眠；
 *                   酒醉酒醒日复日，网上网下年复年。
 *                   但愿老死电脑间，不愿鞠躬老板前；
 *                   奔驰宝马贵者趣，公交自行程序员。
 *                   别人笑我忒疯癫，我笑自己命太贱；
 *                   不见满街漂亮妹，哪个归得程序员？
*/
extern void CircleCount();
int main(void)
{
    DisableGlobalIRQ();
    board_init();   	//务必保留，本函数用于初始化MPU 时钟 调试串口
	systick_delay_ms(300);	//延时300ms，等待主板其他外设上电成功
    
	while(!GlobalInit())
    {
		
    };
   //第一个NULL是开启任务的回调函数，第二个NULL是关闭任务的回调函数
    CreatTask(beepTask,      BeepInt,       NULL,               NULL,                1,   true);
    CreatTask(motorTask,     MotorModule,   NULL,               NULL,                2,   true);
    CreatTask(servoTask,     ServoModule,   NULL,               NULL,                2,   true);
    CreatTask(gyroscopeTask, GetGyroscope,  NULL,               NULL,                2,   true);
	CreatTask(dialTask,      DialSwitch,    NULL,               NULL,                300, true);  //upXX就是打开屏幕，downXX就是关闭屏幕
	CreatTask(adcTask,       ADC_calc,      NULL,               NULL,                20,  true); //时间检测如果太长，那么保护就会迟，小心脚
															    
	CreatTask(keyTask,       KeyScanInt,    NULL,               NULL,                10,  true);
    CreatTask(ANODTTask,     ANODTSend,     NULL,               NULL,                100, true);
	CreatTask(lcdTask,       LcdDisplay,    NULL,               LcdCloseInterrupt,   150, true);
	CreatTask(runControlTask,StartRunAndStopRun,NULL,           NULL,                100, true);
	CreatTask(judgetask,     ELC_Flag_Judge,NULL,               NULL,                100, true);
	CreatTask(circleCountTask,CircleCount,  NULL,               NULL,                2,   true);
    unsigned char i;   
	unsigned char j;
    systick_delay_ms(500);
    EnableGlobalIRQ(0);
	

	
    while(1)
    {
		//判断任务是否执行
        for(i = 0; i < taskSum; i++)
        {
            if(task[i].status == run)
            {
                task[i].status = sleep;
                (*(task[i].Function))();
            }
        }
		if(state.element == circle) BEEP_ON;
		else BEEP_OFF;
    }
}
int16 dis[80] = {0};
void CircleCount()
{
			    /**环岛计数**/
    if(ELC_Circle_turn==1&&ELC_Circle_at==0)//环岛计数清零
        {
        ELC_circle_count++;
        circle_count = 0;
        }

    if(circle_flag==1)
    {
        circle_crossroad++;
        if(circle_crossroad>=200)
        {
            circle_flag = 0;
        }
    }
	uint8 i = 0;
	for( i = 0; i<80; i++)
	dis[i] = left_point[i].x - right_point[i].x;
}

State state; //整车状态
void StateParamInit()
{
	state.element = normal;
	state.isRun = false;
}