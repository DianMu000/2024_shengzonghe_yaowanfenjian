#ifndef __SERIAL_OPENMV_H  
#define __SERIAL_OPENMV_H  

#include "stm32f10x.h"

#define size_openmv_rec_buffer 50

typedef struct{
    uint8_t *padd_rec_buf;
    int16_t decode_buf[2];
    uint8_t read_index;
    uint8_t write_index;
    uint8_t rec_size;
}serial_openmv_buffer_t;

typedef enum {
    pill_red,
    pill_green,
    pill_blue,
    bottle_body,
    bottle_cover,
}state_openmv_t;

uint8_t openmv_init(void);

uint8_t openmv_read(void);

uint8_t openmv_send(state_openmv_t state);

#endif