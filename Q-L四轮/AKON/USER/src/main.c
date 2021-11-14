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
		
/* ע�⣺
	1������������Ҫ������ħ����������C++������language C������������������Ϊgnu

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
 *                     ���汣��        ����BUG
 *            ��Ի:
 *                   д��¥��д�ּ䣬д�ּ������Ա��
 *                   ������Աд�������ó��򻻾�Ǯ��
 *                   ����ֻ���������������������ߣ�
 *                   ��������ո��գ����������긴�ꡣ
 *                   ��Ը�������Լ䣬��Ը�Ϲ��ϰ�ǰ��
 *                   ���۱������Ȥ���������г���Ա��
 *                   ����Ц��߯��񲣬��Ц�Լ���̫����
 *                   ��������Ư���ã��ĸ���ó���Ա��
*/
extern void CircleCount();
int main(void)
{
    DisableGlobalIRQ();
    board_init();   	//��ر��������������ڳ�ʼ��MPU ʱ�� ���Դ���
	systick_delay_ms(300);	//��ʱ300ms���ȴ��������������ϵ�ɹ�
    
	while(!GlobalInit())
    {
		
    };
   //��һ��NULL�ǿ�������Ļص��������ڶ���NULL�ǹر�����Ļص�����
    CreatTask(beepTask,      BeepInt,       NULL,               NULL,                1,   true);
    CreatTask(motorTask,     MotorModule,   NULL,               NULL,                2,   true);
    CreatTask(servoTask,     ServoModule,   NULL,               NULL,                2,   true);
    CreatTask(gyroscopeTask, GetGyroscope,  NULL,               NULL,                2,   true);
	CreatTask(dialTask,      DialSwitch,    NULL,               NULL,                300, true);  //upXX���Ǵ���Ļ��downXX���ǹر���Ļ
	CreatTask(adcTask,       ADC_calc,      NULL,               NULL,                20,  true); //ʱ�������̫������ô�����ͻ�٣�С�Ľ�
															    
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
		//�ж������Ƿ�ִ��
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
			    /**��������**/
    if(ELC_Circle_turn==1&&ELC_Circle_at==0)//������������
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

State state; //����״̬
void StateParamInit()
{
	state.element = normal;
	state.isRun = false;
}