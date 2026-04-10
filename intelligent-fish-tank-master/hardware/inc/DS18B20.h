#ifndef __DS18B20_H
#define __DS18B20_H

#include "stm32f10x_conf.h"

void DS18B20_Init(void);
float DS18B20_ReadTemp(void);
uint8_t DS18B20_Reset(void);
extern u8 ds18b20_flag;

#endif
