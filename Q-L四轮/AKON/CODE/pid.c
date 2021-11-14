#include "pid.h"

Pid pid;  

		
void PidParamInit(void)
{
    //有可能改变
    pid.servo.cam.normal.kp1 = 5;
    pid.servo.cam.normal.kp2 = 7;
    pid.servo.cam.normal.kd  = 0;
	
	pid.servo.cam.circle.kp1 = 6;
	pid.servo.cam.circle.kp2 = 8;
	pid.servo.cam.circle.kd  = 0;
	
	pid.servo.elc.kp = 360;
	pid.servo.elc.ki = 0;
	pid.servo.elc.kd = 0;
	
    pid.leftMotor.kp  = 10;
    pid.leftMotor.ki  = 10;
    pid.leftMotor.kd  = 0;
	
    pid.rightMotor.kp = 10;
    pid.rightMotor.ki = 10;
    pid.rightMotor.kd = 0;
    //基本不变
	pid.servo.cam.use.kp1 = 0;	//实际使用的值在计算输出值进行赋值使用的PID
	pid.servo.cam.use.kp2 = 0;
	pid.servo.cam.use.kd = 0;
	
    pid.servo.cam.repeat.kp1Sum = 0;
    pid.servo.cam.repeat.kp2Sum = 0;
    pid.servo.cam.repeat.kpSum  = 0;
    pid.servo.cam.repeat.kdSum  = 0;
	
    pid.leftMotor.error  = 0;
    pid.leftMotor.errorPre  = 0;
    pid.leftMotor.errorPrePre  = 0;

    pid.rightMotor.error = 0;
    pid.rightMotor.errorPre = 0;
    pid.rightMotor.errorPrePre = 0; 

}

float FuzzyservoKp1(void)
{		
		int E[7] = {-450,-300,-150, 0, 150, 300, 450,};
		int Ec[7]= { -60, -40, -20, 0,  20,  40,  60,};
		int En[7]= {  -9,  -6,  -3, 0,   3,   6,   9,};
		int fuzzykp[7][7]={
			//   				 Kp模糊规则表
			//							 Ec
			// 		 -9,-6,-3, 0, 3, 6, 9
						{ 9, 9, 9, 9, 6, 0, 0,},//-9
						{ 9, 9, 9, 6, 6, 0, 0,},//-6
						{ 9, 6, 6, 3, 0,-3,-6,},//-3
						{ 6, 6, 3, 0,-3,-6,-6,},// 0		E
						{ 3, 3, 0,-6,-6,-6,-9,},// 3
						{ 0, 0, 0,-6,-9,-9,-9,},// 6
						{ 0,-3,-9,-9,-9,-9,-9,} // 9
				};
	if(camServoError<=E[6] && camServoError>=E[0]){
		if(camServoError<=E[1]){
			pid.fuzzy.Eas = (E[1] - camServoError)/(E[1] - E[0]);
			pid.fuzzy.lis = 1;
		}else if(camServoError<=E[2]){
			pid.fuzzy.Eas = (E[2] - camServoError)/(E[2] - E[1]);
			pid.fuzzy.lis = 2;
		}else if(camServoError<=E[3]){
			pid.fuzzy.Eas = (E[3] - camServoError)/(E[3] - E[2]);
			pid.fuzzy.lis = 3;
		}else if(camServoError<=E[4]){
			pid.fuzzy.Eas = (E[4] - camServoError)/(E[4] - E[3]);
			pid.fuzzy.lis= 4;
		}else if(camServoError<=E[5]){
			pid.fuzzy.Eas = (E[5] - camServoError)/(E[5] - E[4]);
			pid.fuzzy.lis = 5;
		}else if(camServoError<=E[6]){
			pid.fuzzy.Eas = (E[6] - camServoError)/(E[6] - E[5]);
			pid.fuzzy.lis = 6;
		};    
	};
	if(camServoErrorAdd<=Ec[6] && camServoErrorAdd>=Ec[0]){
		if(camServoErrorAdd<=Ec[1]){
			pid.fuzzy.Ebs = (Ec[1] - camServoErrorAdd)/(Ec[1] - Ec[0]);
			pid.fuzzy.js = 1;
		}else if(camServoErrorAdd<=Ec[2]){
			pid.fuzzy.Ebs = (Ec[2] - camServoErrorAdd)/(Ec[2] - Ec[1]);
			pid.fuzzy.js = 2;
		}else if(camServoErrorAdd<=Ec[3]){
			pid.fuzzy.Ebs = (Ec[3] - camServoErrorAdd)/(Ec[3] - Ec[2]);
			pid.fuzzy.js = 3;
		}else if(camServoErrorAdd<=Ec[4]){
			pid.fuzzy.Ebs= (Ec[4] - camServoErrorAdd)/(Ec[4] - Ec[3]);
			pid.fuzzy.js = 4;
		}else if(camServoErrorAdd<=Ec[5]){
			pid.fuzzy.Ebs = (Ec[5] - camServoErrorAdd)/(Ec[5] - Ec[4]);
			pid.fuzzy.js = 5;
		}else if(camServoErrorAdd<=Ec[6]){
			pid.fuzzy.Ebs = (Ec[6] - camServoErrorAdd)/(Ec[6] - Ec[5]);
			pid.fuzzy.js = 6;
		};    
	};
	pid.servo.cam.normal.kp1 = pid.fuzzy.Eas*pid.fuzzy.Ebs*fuzzykp[pid.fuzzy.lis][pid.fuzzy.js] +
									(1-pid.fuzzy.Eas)*pid.fuzzy.Ebs*fuzzykp[pid.fuzzy.lis-1][pid.fuzzy.js] +
									pid.fuzzy.Eas*(1-pid.fuzzy.Ebs)*fuzzykp[pid.fuzzy.lis][pid.fuzzy.js-1] +
									(1-pid.fuzzy.Eas)*(1-pid.fuzzy.Ebs)*fuzzykp[pid.fuzzy.lis-1][pid.fuzzy.js-1];
	
	return pid.servo.cam.normal.kp1;
}

float FuzzymotorKpl(void)
{		int leftmotorerrorAdd = pid.leftMotor.error - pid.leftMotor.errorPre;
		int Em[7] = {-400,-200,-100, 0, 100, 200, 400,};
		int Emc[7]= { -200, -100, -50, 0,  50,  100,  200,};
		int Emn[7]= {  -9,  -6,  -3, 0,   3,   6,   9,};
		int fuzzykpl[7][7]={
			//   				 Kp模糊规则表
			//							 Ec
			// 		 -12, -8, -4,  0,  4,  8, 12,
						{  0,  0,  4,  6,  6,  6,  6,},//-12
						{  0,  0,  4,  4,  6,  6,  6,},//-8
						{ -4, -4,  0,  4,  4,  4,  6,},//-4
						{ -2, -2, -2,  0,  4,  4,  4,},// 0		E
						{ -6, -4, -4, -4,  0,  2,  2,},// 4
						{ -6, -6, -6, -4,  0,  0,  0,},// 8
						{ -6, -6, -6, -6, -6, -2,  0,} // 12
				};
		if(pid.leftMotor.error>Em[6]){
			pid.leftMotor.error = Em[6];
		}else if(pid.leftMotor.error<Em[0]){
			pid.leftMotor.error = Em[0];
		};
	if(pid.leftMotor.error<=Em[6] && pid.leftMotor.error>=Em[0]){
		if(pid.leftMotor.error<=Em[1]){
			pid.fuzzy.Eam = (Em[1] - pid.leftMotor.error)/(Em[1] - Em[0]);
			pid.fuzzy.lim = 1;
		}else if(pid.leftMotor.error<=Em[2]){
			pid.fuzzy.Eam = (Em[2] - pid.leftMotor.error)/(Em[2] - Em[1]);
			pid.fuzzy.lim = 2;
		}else if(pid.leftMotor.error<=Em[3]){
			pid.fuzzy.Eam = (Em[3] - pid.leftMotor.error)/(Em[3] - Em[2]);
			pid.fuzzy.lim = 3;
		}else if(pid.leftMotor.error<=Em[4]){
			pid.fuzzy.Eam = (Em[4] - pid.leftMotor.error)/(Em[4] - Em[3]);
			pid.fuzzy.lim= 4;
		}else if(pid.leftMotor.error<=Em[5]){
			pid.fuzzy.Eam = (Em[5] - pid.leftMotor.error)/(Em[5] - Em[4]);
			pid.fuzzy.lim = 5;
		}else if(pid.leftMotor.error<=Em[6]){
			pid.fuzzy.Eam = (Em[6] - pid.leftMotor.error)/(Em[6] - Em[5]);
			pid.fuzzy.lim = 6;
		};    
	};
	if(leftmotorerrorAdd<=Emc[6] && leftmotorerrorAdd>=Emc[0]){
		if(leftmotorerrorAdd<=Emc[1]){
			pid.fuzzy.Ebm = (Emc[1] - leftmotorerrorAdd)/(Emc[1] - Emc[0]);
			pid.fuzzy.jm = 1;
		}else if(leftmotorerrorAdd<=Emc[2]){
			pid.fuzzy.Ebm = (Emc[2] - leftmotorerrorAdd)/(Emc[2] - Emc[1]);
			pid.fuzzy.jm = 2;
		}else if(leftmotorerrorAdd<=Emc[3]){
			pid.fuzzy.Ebm= (Emc[3] - leftmotorerrorAdd)/(Emc[3] - Emc[2]);
			pid.fuzzy.jm = 3;
		}else if(leftmotorerrorAdd<=Emc[4]){
			pid.fuzzy.Ebm = (Emc[4] - leftmotorerrorAdd)/(Emc[4] - Emc[3]);
			pid.fuzzy.jm = 4;
		}else if(leftmotorerrorAdd<=Emc[5]){
			pid.fuzzy.Ebm = (Emc[5] - leftmotorerrorAdd)/(Emc[5] - Emc[4]);
			pid.fuzzy.jm = 5;
		}else if(leftmotorerrorAdd<=Emc[6]){
			pid.fuzzy.Ebm = (Emc[6] - leftmotorerrorAdd)/(Emc[6] - Emc[5]);
			pid.fuzzy.jm = 6;
		};    
	};
	pid.leftMotor.kp = pid.fuzzy.Eam*pid.fuzzy.Ebm*fuzzykpl[pid.fuzzy.lim][pid.fuzzy.jm] +
									(1-pid.fuzzy.Eam)*pid.fuzzy.Ebm*fuzzykpl[pid.fuzzy.lim-1][pid.fuzzy.jm] +
									pid.fuzzy.Eam*(1-pid.fuzzy.Ebm)*fuzzykpl[pid.fuzzy.lim][pid.fuzzy.jm-1] +
									(1-pid.fuzzy.Eam)*(1-pid.fuzzy.Ebm)*fuzzykpl[pid.fuzzy.lim-1][pid.fuzzy.jm-1];
	
	return pid.leftMotor.kp;
}

float FuzzymotorKil(void)
{		int leftmotorerrorAdd = pid.leftMotor.error - pid.leftMotor.errorPre;
		int Em[7] = {-400,-200,-100, 0, 100, 200, 400,};
		int Emc[7]= { -200, -100, -50, 0,  50,  100,  200,};
		int Emn[7]= {  -9,  -6,  -3, 0,   3,   6,   9,};
		int fuzzykil[7][7]={
			//   				    Ki模糊规则表
			//							    Ec
			// 		 -12, -8, -4,  0,  4,  8, 12,
						{  0,  0,  2,  3,  3,  3,  3,},//-12
						{  0,  0,  2,  2,  3,  3,  3,},//-8
						{ -2, -2,  0,  2,  2,  2,  3,},//-4
						{ -1, -1, -1,  0,  2,  2,  2,},// 0		E
						{ -3, -2, -2, -2,  0,  1,  1,},// 4
						{ -3, -3, -3, -2,  0,  0,  0,},// 8
						{ -3, -3, -3, -3, -3, -1,  0,} // 12
				};
		
		if(pid.leftMotor.error>Em[6]){
			pid.leftMotor.error = Em[6];
		}else if(pid.leftMotor.error<Em[0]){
			pid.leftMotor.error = Em[0];
		};
	if(pid.leftMotor.error<=Em[6] && pid.leftMotor.error>=Em[0]){
		if(pid.leftMotor.error<=Em[1]){
			pid.fuzzy.Eam = (Em[1] - pid.leftMotor.error)/(Em[1] - Em[0]);
			pid.fuzzy.lim = 1;
		}else if(pid.leftMotor.error<=Em[2]){
			pid.fuzzy.Eam = (Em[2] - pid.leftMotor.error)/(Em[2] - Em[1]);
			pid.fuzzy.lim = 2;
		}else if(pid.leftMotor.error<=Em[3]){
			pid.fuzzy.Eam = (Em[3] - pid.leftMotor.error)/(Em[3] - Em[2]);
			pid.fuzzy.lim = 3;
		}else if(pid.leftMotor.error<=Em[4]){
			pid.fuzzy.Eam = (Em[4] - pid.leftMotor.error)/(Em[4] - Em[3]);
			pid.fuzzy.lim= 4;
		}else if(pid.leftMotor.error<=Em[5]){
			pid.fuzzy.Eam = (Em[5] - pid.leftMotor.error)/(Em[5] - Em[4]);
			pid.fuzzy.lim = 5;
		}else if(pid.leftMotor.error<=Em[6]){
			pid.fuzzy.Eam = (Em[6] - pid.leftMotor.error)/(Em[6] - Em[5]);
			pid.fuzzy.lim = 6;
		};    
	};
	if(leftmotorerrorAdd<=Emc[6] && leftmotorerrorAdd>=Emc[0]){
		if(leftmotorerrorAdd<=Emc[1]){
			pid.fuzzy.Ebm = (Emc[1] - leftmotorerrorAdd)/(Emc[1] - Emc[0]);
			pid.fuzzy.jm = 1;
		}else if(leftmotorerrorAdd<=Emc[2]){
			pid.fuzzy.Ebm = (Emc[2] - leftmotorerrorAdd)/(Emc[2] - Emc[1]);
			pid.fuzzy.jm = 2;
		}else if(leftmotorerrorAdd<=Emc[3]){
			pid.fuzzy.Ebm= (Emc[3] - leftmotorerrorAdd)/(Emc[3] - Emc[2]);
			pid.fuzzy.jm = 3;
		}else if(leftmotorerrorAdd<=Emc[4]){
			pid.fuzzy.Ebm = (Emc[4] - leftmotorerrorAdd)/(Emc[4] - Emc[3]);
			pid.fuzzy.jm = 4;
		}else if(leftmotorerrorAdd<=Emc[5]){
			pid.fuzzy.Ebm = (Emc[5] - leftmotorerrorAdd)/(Emc[5] - Emc[4]);
			pid.fuzzy.jm = 5;
		}else if(leftmotorerrorAdd<=Emc[6]){
			pid.fuzzy.Ebm = (Emc[6] - leftmotorerrorAdd)/(Emc[6] - Emc[5]);
			pid.fuzzy.jm = 6;
		};    
	};
	pid.leftMotor.ki = pid.fuzzy.Eam*pid.fuzzy.Ebm*fuzzykil[pid.fuzzy.lim][pid.fuzzy.jm] +
									(1-pid.fuzzy.Eam)*pid.fuzzy.Ebm*fuzzykil[pid.fuzzy.lim-1][pid.fuzzy.jm] +
									pid.fuzzy.Eam*(1-pid.fuzzy.Ebm)*fuzzykil[pid.fuzzy.lim][pid.fuzzy.jm-1] +
									(1-pid.fuzzy.Eam)*(1-pid.fuzzy.Ebm)*fuzzykil[pid.fuzzy.lim-1][pid.fuzzy.jm-1];
	
	return pid.leftMotor.ki;
}


