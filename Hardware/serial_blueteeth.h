#ifndef __SERIAL_BLUETEETH
#define __SERIAL_BLUETEETH

#include "stm32f10x.h"

#define size_blueteeth_rec_buffer 50

typedef struct{
    uint8_t *padd_rec_buf;
    uint8_t read_index;
    uint8_t write_index;
    uint8_t rec_size;
}serial_blueteeth_buffer_t;



uint8_t blueteeth_init(serial_blueteeth_buffer_t* serial_blueteeth_buffer);

int8_t blueteeth_read(void);

#endif
