#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"


#define key_r_port GPIOB
#define key_r_pin GPIO_Pin_15

#define key_g_port GPIOB
#define key_g_pin GPIO_Pin_14

#define key_b_port GPIOB
#define key_b_pin GPIO_Pin_13

#define key_stop_port GPIOB
#define key_stop_pin GPIO_Pin_12

typedef enum{
    red = 0,
    green,
    blue,
	none,
}color_t;

uint8_t key_init(void);

uint8_t key_read_color_plate_and_color_bottle(color_t* color_plate, color_t* color_bottle);
#endif
