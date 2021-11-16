#include "command.h"
#include <rtthread.h>
#include "zf_tim.h"
extern float Angle[4];      // 角度环PID{0.2, 0, 0.03, 1000};
extern float Ang_Vel[4];   // 角速度环PID {0.2, 0.007, 0, 500};
extern float Vel[4];  // 速度环PID {0.2, 0.007, 0, 500};
extern float rubo_Vel[4];//转向角度环
extern float Direct[4];	// 转向角速度环PID 位置{0.018, 0.00215, 0.023, 55};
extern int speed_set;
extern  rt_thread_t tid_main;
extern	int start_line,end_line;///////////////
extern float setted_angle;

void set_speed (int argc, char **argv)
	{
		 		if(argc!=2)return;
		 		 printf("speed: %d\n",speed_set);
		 	sscanf(argv[1],"%d",&speed_set); // char to float 
		 printf("set: %d\n",speed_set);

	 }
void read_speed()
{
	 	 printf("speed: %d\n",speed_set);
}
void read_view()
{
			 printf("view: %d %d\n",start_line,end_line);
}
void read_angle()
{
		 		 printf("angle: %f\n",setted_angle);
}
void set_angle (int argc, char **argv)
	{
		 		if(argc!=2)return;

		 		 printf("angle: %f\n",setted_angle);

		 	sscanf(argv[1],"%d",&setted_angle); // char to float 
		 		 printf("angle: %f\n",setted_angle);

	 }
void set_view (int argc, char **argv)
	{
		 		if(argc!=3)return;

			 printf("view: %d %d\n",start_line,end_line);

		 	sscanf(argv[1],"%d",&start_line); // char to float 
				 	sscanf(argv[2],"%d",&end_line); // char to float 
			 printf("view: %d %d\n",start_line,end_line);
	 }
void read_pid(int argc, char **argv)
{
  	printf("1\tangle pid:\n%f\t%f\t%f\n",Angle[0],Angle[1],Angle[2]);
		printf("2\tangle_v pid:\n%f\t%f\t%f\n",Ang_Vel[0],Ang_Vel[1],Ang_Vel[2]);
		printf("3\tspeed pid:\n%f\t%f\t%f\n",Vel[0],Vel[1],Vel[2]);
		printf("4\tturn_angle pid:\n%f\t%f\t%f\n",rubo_Vel[0],rubo_Vel[1],rubo_Vel[2]);
    printf("5\tturn_angle_v pid:\n%f\t%f\t%f\n",Direct[0],Direct[1],Direct[2]);

	}
void set_pid(int argc, char **argv)
{
		if(argc!=5)return;

	float num[3];
	sscanf(argv[2],"%f",&num[0]); // char to float 
	sscanf(argv[3],"%f",&num[1]); // char to float 
	sscanf(argv[4],"%f",&num[2]); // char to float 
switch(argv[1][0]-48)	
{
	case 1:
	printf("1\tangle pid:\n%f\t%f\t%f\n",Angle[0],Angle[1],Angle[2]);
	Angle[0]=num[0],Angle[1]=num[1],Angle[2]=num[2];
	printf("setting pid:\n%f\t%f\t%f\n",Angle[0],Angle[1],Angle[2]);


		break;
	case 2:
		printf("2\tangle_v pid:\n%f\t%f\t%f\n",Ang_Vel[0],Ang_Vel[1],Ang_Vel[2]);
		Ang_Vel[0]=num[0],Ang_Vel[1]=num[1],Ang_Vel[2]=num[2];
		printf("setting pid:\n%f\t%f\t%f\n",Ang_Vel[0],Ang_Vel[1],Ang_Vel[2]);

		break;
	case 3:
		printf("3\tspeed pid:\n%f\t%f\t%f\n",Vel[0],Vel[1],Vel[2]);
		Vel[0]=num[0],Vel[1]=num[1],Vel[2]=num[2];
		printf("setting pid:\n%f\t%f\t%f\n",Vel[0],Vel[1],Vel[2]);

		break;
	case 4:
		printf("4\tturn_angle pid:\n%f\t%f\t%f\n",rubo_Vel[0],rubo_Vel[1],rubo_Vel[2]);
		rubo_Vel[0]=num[0],rubo_Vel[1]=num[1],rubo_Vel[2]=num[2];
		printf("setting pid:\n%f\t%f\t%f\n",rubo_Vel[0],rubo_Vel[1],rubo_Vel[2]);
		break;
	case 5:
			printf("5\tturn_angle_v pid:\n%f\t%f\t%f\n",Direct[0],Direct[1],Direct[2]);
		Direct[0]=num[0],Direct[1]=num[1],Direct[2]=num[2];
			printf("setting pid:\n%f\t%f\t%f\n",Direct[0],Direct[1],Direct[2]);
		break;
}

   return;

}
