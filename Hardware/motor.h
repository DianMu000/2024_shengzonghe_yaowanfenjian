#ifndef __MOTOR_H
#define __MOTOR_H
#include "stm32f10x.h"

#define size_motor_rec_buffer 50
#define size_motor_decode_buffer 50

#define motor_x_id 0x01
#define motor_y_id 0x02
#define motor_z_id 0x03

//#define until_motor_get_flag_(id, order) \
//do{\
//			motor_send_get_flag_##order(id);\
//			Delay_ms(8);\
//		}\
//		while(!motor_read_get_flag_##order(id));\

typedef struct{
    uint8_t *padd_rec_buf;
    uint8_t *padd_decode_buf;
    uint8_t read_index;
    uint8_t write_index;
    uint8_t rec_size;
    uint8_t decode_size;
}serial_motor_buffer_t;

uint8_t motor_init(serial_motor_buffer_t* serial_motor_buffer);

uint8_t motor_move_speed(uint8_t id, uint8_t direction, uint16_t speed);

uint8_t motor_move_position(uint8_t id, uint8_t direction, uint16_t speed, uint32_t pulse);

uint8_t motor_stop(uint8_t id);

uint8_t motor_set0(uint8_t id);

uint8_t motor_get_flag_set0(uint8_t id); //0x3B

uint8_t motor_get_flag_arrive(uint8_t id);

uint8_t motor_get_flag_arrive(uint8_t id);
#endif
