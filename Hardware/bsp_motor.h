#ifndef __BSP_MOTOR_H
#define __BSP_MOTOR_H
#include "stm32f10x.h"

#define size_motor_rec_buffer 50
#define size_motor_decode_buffer 50

#define motor_x_ID 0x01
#define motor_y_ID 0x02
#define motor_z_ID 0x03

typedef struct{
    uint8_t *padd_rec_buf;
    uint8_t *padd_decode_buf;
    uint8_t read_index;
    uint8_t write_index;
    uint8_t rec_size;
    uint8_t decode_size;
}serial_motor_buffer_t;


uint8_t motor_init(serial_motor_buffer_t* serial_motor_buffer);

uint8_t motor_x_move_speed(uint8_t direction, uint16_t speed);

uint8_t motor_x_move_position(uint8_t direction, uint16_t speed, uint32_t pulse);

uint8_t motor_x_stop(void);

uint8_t motor_x_set0(void);

uint8_t motor_x_send_get_flag_set0(void); //0x3B

uint8_t motor_x_read_get_flag_set0(void);

uint8_t motor_x_send_get_flag_arrive(void);

uint8_t motor_x_read_get_flag_arrive(void);
#endif
