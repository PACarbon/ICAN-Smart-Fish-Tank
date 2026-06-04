#include "user.h"
extern uint8_t RX_data[];
extern uint8_t usernum;
extern volatile int target;
extern int tempture;
/*
*函数功能：用户设置温度函数
*形参:无
*返回值：无
*/
//void user_select()
//{
//	
//	
//}
void user_Init(void)
{
		oled_clear();
		oled_show_string(40,0,"welcome",2);
}

void user_set(void)
{
	
	oled_clear();
	oled_show_string(0,0,"target:",2);
	oled_display_float(55,0,target,2);
}

void user_set2(void)
{
	oled_clear();
	oled_show_string(50,0,"error",2);
}

/*
*函数功能：鱼缸工作温度查看
*形参:用户设定目标温度
*返回值：无
*/
void user_show(target)
{
	
		oled_clear();
		oled_show_string(0,0,"target:",2);
		oled_display_float(55,0,target,2);
		oled_show_string(0,5,"tempture:",2);
		oled_display_float(70,5,tempture,2);
}

//void user_temptureput(target)
//{
//		oled_clear();
//		target++;
//		oled_show_string(0,0,"target:",2);
//		oled_display_float(55,0,target,2);

//}

//void user_tempturedowm(target)
//{
//		oled_clear();
//		target--;
//		oled_show_string(0,0,"target:",2);
//		oled_display_float(55,0,target,2);

//}