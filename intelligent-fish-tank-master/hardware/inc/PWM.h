#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x_conf.h"

void PWM_Init(void);
void PWM_SetCompare2(uint16_t Compare);

#endif
