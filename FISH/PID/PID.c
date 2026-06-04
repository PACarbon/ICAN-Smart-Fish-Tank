#include "PID.h"
static int ntemp;//比例系数，积分系数,目标温度，当前温度
static float kp,ki;
extern volatile int target;
/*
增量式控制算法
result+=kp*(Ek-Ek_1)+ki*Ek+kd*(Ek-2Ek_1+Ek_2)

函数功能：温度控制
参数：目标温度，当前温度
返回值：控制温度
*/
void PID_Init(void)
{
	target = 28;
	kp = 10;
	ki = 0.298;
}

volatile int Itergral;
int calculate_tempture(int target,int ntemp)
{
	int Ek,control_temp,Ek_1;
	 Ek=target-ntemp;
	 Itergral += Ek;
	 control_temp=kp*Ek+ki*Itergral;
	 Ek_1=Ek;
	if(control_temp >= 100)
	{
	control_temp = 100;
		
	}
	else if(control_temp <=0)
	{
	control_temp = 0;
	}
	else{};
	return control_temp;
}
