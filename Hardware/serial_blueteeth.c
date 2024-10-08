#include "stm32f10x.h"
#include "serial_blueteeth.h"
#include "stdlib.h"
extern serial_blueteeth_buffer_t serial_blueteeth_buffer;

static uint8_t GetBufBitsNoRead(void){
    return (int8_t)(((int16_t)(serial_blueteeth_buffer.write_index) - (int16_t)(serial_blueteeth_buffer.read_index)) + (int16_t)(serial_blueteeth_buffer.rec_size)) % (int16_t)(serial_blueteeth_buffer.rec_size);
}

static uint8_t read_byte(void){
    uint8_t byte = serial_blueteeth_buffer.padd_rec_buf[serial_blueteeth_buffer.read_index];
    serial_blueteeth_buffer.read_index = (serial_blueteeth_buffer.read_index + 1) % serial_blueteeth_buffer.rec_size;
                                                                                                                                                                                                                                                                                                                
    return byte;
}

uint8_t blueteeth_init(serial_blueteeth_buffer_t* serial_blueteeth_buffer){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_HardwareFlowControl =  USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART3, &USART_InitStructure);

    USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_Cmd(USART3, ENABLE);

    serial_blueteeth_buffer->padd_rec_buf = (uint8_t*)malloc(size_blueteeth_rec_buffer);
    serial_blueteeth_buffer->rec_size = size_blueteeth_rec_buffer;

    serial_blueteeth_buffer->read_index = 0;
    serial_blueteeth_buffer->write_index = 0;
    return 0; 
}

int8_t blueteeth_read(void){
    if(GetBufBitsNoRead() != 0){
        return read_byte();
	}
    else{
        return -1;
	}
}

/// @brief 串口中断，数据进入缓冲区
/// @param  
void USART3_IRQHandler(void){
    if(USART_GetITStatus(USART3,USART_IT_RXNE)==1){
        serial_blueteeth_buffer.padd_rec_buf[serial_blueteeth_buffer.write_index] = (uint8_t)USART_ReceiveData(USART3);
        serial_blueteeth_buffer.write_index = (serial_blueteeth_buffer.write_index + 1) % serial_blueteeth_buffer.rec_size;

        USART_ClearITPendingBit(USART3,USART_IT_RXNE);
    }
}
