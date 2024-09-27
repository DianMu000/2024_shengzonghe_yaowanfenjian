#ifndef __POSITION_H
#define __POSITION_H
#include "stm32f10x.h"

/******************config***********************/
#define position_approach_method pid //选择“pid”或则“absolute”

#define position_PID_p 0  
#define position_PID_i 0 
#define position_PID_d 0

#define position_absolute_factor 0
/******************endconfig***********************/

uint8_t position_approach(int16_t x, int16_t y);


#endif
