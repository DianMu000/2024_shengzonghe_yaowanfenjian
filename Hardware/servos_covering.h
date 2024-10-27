#ifndef __SERVOS_COVERING_H
#define __SERVOS_COVERING_H

#include "stm32f10x.h"

#define servos_covering_crr_on 0 / 270 * 2000 + 500
#define servos_covering_crr_off 90 / 270 * 2000 + 500
#define round_time 0
#define covering_round_motor_move_speed 0

uint8_t servos_covering_init(void);
uint8_t servos_covering_on(void);
uint8_t servos_covering_off(void);



#endif
