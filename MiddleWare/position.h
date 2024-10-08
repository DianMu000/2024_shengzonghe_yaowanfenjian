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


typedef struct{
    uint8_t (*move_x) (int16_t distance, uint8_t speed);
    uint8_t (*move_y) (int16_t distance, uint8_t speed);
}position_intereface_t;

typedef struct{
    int16_t position_x;
    int16_t position_y;
    int16_t position_x_max;
    int16_t position_x_min;
    int16_t position_y_max;
    int16_t position_y_min;
}position_inf_t;

typedef struct {
    position_intereface_t position_intereface;
    position_inf_t position_inf;

    uint8_t (*move_to) (int16_t position_x, int16_t position_y);
    uint8_t (*get_position) (uint16_t *position_x, uint16_t *position_y);
    uint8_t (*move_x) (uint16_t speed, int16_t position);
    uint8_t (*move_y) (uint16_t speed, int16_t position);
}position_t;



uint8_t position_initialize(position_t* position, position_intereface_t *position_intereface, position_inf_t *position_inf);


#endif
