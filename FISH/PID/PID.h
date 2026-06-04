#ifndef __PID_H__
#define __PID_H__

#include "main.h"
void PID_Init(void);
int calculate_tempture(int target_temp,int ntemp);
int tempture_restrict(int result);
#endif
