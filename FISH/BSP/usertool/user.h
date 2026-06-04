#ifndef __USER_H
#define __USER_H

#include "OLED.h"
#include "DS18B20.h"
void user_Init(void);
void user_set(void);
void user_set2(void);
void user_show (int target);
//void user_temptureput(int target);
//void user_tempturedowm(int target); 

#endif
