#include "serial_openmv.h"
#include "stdlib.h"
#include "delay.h"

extern serial_openmv_buffer_t serial_openmv_buffer;

static void SendByte(uint8_t data){
    USART_SendData(USART2, (uint16_t)data);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == 0);
}

static uint8_t GetBufBitsNoRead(const serial_openmv_buffer_t *serial_openmv_buffer){
    return (int8_t)(((int16_t)(serial_openmv_buffer->write_index) - (int16_t)(serial_openmv_buffer->read_index)) + (int16_t)(serial_openmv_buffer->rec_size)) % (int16_t)(serial_openmv_buffer->rec_size);
}

static uint8_t read_byte(void){
    uint8_t byte = serial_openmv_buffer.padd_rec_buf[serial_openmv_buffer.read_index];
    serial_openmv_buffer.read_index = (serial_openmv_buffer.read_index + 1) % serial_openmv_buffer.rec_size;
                                                                                                                                                                                                                                                                                                                
    return byte;
}

uint8_t openmv_init(serial_openmv_buffer_t *serial_openmv_buffer)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART2, &USART_InitStructure);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART2, ENABLE);

    serial_openmv_buffer->padd_rec_buf = (uint8_t*)malloc(size_openmv_rec_buffer);
    serial_openmv_buffer->rec_size = size_openmv_rec_buffer;

    serial_openmv_buffer->read_index = 0;
    serial_openmv_buffer->write_index = 0;

    return 0;
}

uint8_t openmv_read(void)
{
    while(GetBufBitsNoRead(&serial_openmv_buffer) != 0){
        if(read_byte() == 0x60){
            while(GetBufBitsNoRead(&serial_openmv_buffer) >= 6){
                if(read_byte() == 0x00){
                    return 1;//接受到数据帧，无数据
                }
                else{
                    serial_openmv_buffer.decode_buf[0] = (int16_t)(read_byte() << 8)  | (int16_t)read_byte();
                    serial_openmv_buffer.decode_buf[1] = (int16_t)(read_byte() << 8)  | (int16_t)read_byte();
                }
                if(read_byte() == 0x6B){
                    return 0;//接受到数据帧，有数据，校验位正确
                }
                else return 2;//接受到数据帧，有数据，校验位错误
            }
        } 
    }
    return 3;//无数据进入缓冲区
}

uint8_t openmv_send(state_openmv_t state)
{
    SendByte(0x60);
    SendByte(state);
    SendByte(0x6B);

    return 0;
}

/// @brief 串口中断，数据进入缓冲区
/// @param  
void USART2_IRQHandler(void){
    if(USART_GetITStatus(USART2,USART_IT_RXNE)==1){
        serial_openmv_buffer.padd_rec_buf[serial_openmv_buffer.write_index] = (uint8_t)USART_ReceiveData(USART2);
        serial_openmv_buffer.write_index = (serial_openmv_buffer.write_index + 1) % serial_openmv_buffer.rec_size;

        USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    }
}
