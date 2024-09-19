#include "bsp_motor.h"
#include "stdlib.h"
#include "delay.h"
extern serial_motor_buffer_t serial_motor_buffer;



		
/// @brief 串口1发送byte
/// @param data 
static void SendByte(uint8_t data){
    USART_SendData(USART1,(uint16_t)data);
      while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==0);
}

static uint8_t GetBufBitsNoRead(const serial_motor_buffer_t *serial_motor_buffer){
    return (int8_t)(((int16_t)(serial_motor_buffer->write_index) - (int16_t)(serial_motor_buffer->read_index)) + (int16_t)(serial_motor_buffer->rec_size)) % (int16_t)(serial_motor_buffer->rec_size);
}

static uint8_t read_byte(void){
    uint8_t byte = serial_motor_buffer.padd_rec_buf[serial_motor_buffer.read_index];
    serial_motor_buffer.read_index = (serial_motor_buffer.read_index + 1) % serial_motor_buffer.rec_size;
                                                                                                                                                                                                                                                                                                                
    return byte;
}



/// @brief 读回零命令接返回标志
/// @param  void
/// @return  读到返回1，读不到返回0
 static uint8_t motor_read_get_set0_recieve(uint8_t id){

    while(GetBufBitsNoRead(&serial_motor_buffer) != 0){
        if(read_byte() == id ){
			while(GetBufBitsNoRead(&serial_motor_buffer) >= 3){
				if(read_byte() == 0x9A && read_byte() == 0x02 && read_byte() == 0x6B){
					return 1;
				}
				else{
					return 0;
				}
					
			}
        }
    }
    return 0;
}
 
/// @brief 读位置控制命令返回标志
/// @param  void
/// @return  读到返回1，读不到返回0
 static uint8_t motor_read_get_position_control_recieve(uint8_t id){

    while(GetBufBitsNoRead(&serial_motor_buffer) != 0){
        if(read_byte() == id ){
			while(GetBufBitsNoRead(&serial_motor_buffer) >= 3){
				if(read_byte() == 0xFD && read_byte() == 0x02 && read_byte() == 0x6B){
					return 1;
				}
				else{
					return 0;
				}
					
			}
        }
    }
    return 0;
}
 
/// @brief 读速度控制命令返回标志
/// @param  void
/// @return  读到返回1，读不到返回0
 static uint8_t motor_read_get_speed_control_recieve(uint8_t id){

    while(GetBufBitsNoRead(&serial_motor_buffer) != 0){
        if(read_byte() == id ){
			while(GetBufBitsNoRead(&serial_motor_buffer) >= 3){
				if(read_byte() == 0xFE && read_byte() == 0x02 && read_byte() == 0x6B){
					return 1;
				}
				else{
					return 0;
				}
					
			}
        }
    }
    return 0;
}
 
/// @brief 读停止命令返回标志
/// @param  void
/// @return  读到返回1，读不到返回0
 static uint8_t motor_read_get_stop_recieve(uint8_t id){

    while(GetBufBitsNoRead(&serial_motor_buffer) != 0){
        if(read_byte() == id ){
			while(GetBufBitsNoRead(&serial_motor_buffer) >= 3){
				if(read_byte() == 0xF6 && read_byte() == 0x02 && read_byte() == 0x6B){
					return 1;
				}
				else{
					return 0;
				}
					
			}
        }
    }
    return 0;
}
uint8_t motor_init(serial_motor_buffer_t* serial_motor_buffer){
    /*Part 1 System config*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_HardwareFlowControl =  USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1,&USART_InitStructure);

    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_Cmd(USART1,ENABLE);

    serial_motor_buffer->padd_rec_buf = (uint8_t*)malloc(size_motor_rec_buffer);
    serial_motor_buffer->rec_size = size_motor_rec_buffer;

    serial_motor_buffer->padd_decode_buf = (uint8_t*)malloc(size_motor_decode_buffer);
    serial_motor_buffer->decode_size = size_motor_decode_buffer;

    serial_motor_buffer->read_index = 0;
    serial_motor_buffer->write_index = 0;

    return 0;

}


/// @brief 发送速度模式控制命令
/// @param direction 方向
/// @param speed 速度
/// @return 
uint8_t motor_move_speed(uint8_t id, uint8_t direction, uint16_t speed){
    do{  
        SendByte(id);
        SendByte(0xf6);
        SendByte(direction);
        SendByte((uint8_t)(speed >> 8) & 0x00ff);
        SendByte((uint8_t)(speed & 0x00ff));
        SendByte(0x00);
        SendByte(0x00);
        SendByte(0x6B);
		Delay_ms(8);
  }while(!motor_read_get_speed_control_recieve(id));
    return 0;
}

/// @brief 发送位置模式控制命令
/// @param direction 方向
/// @param speed 速度
/// @param pulse 脉冲
/// @return 
uint8_t motor_move_position(uint8_t id, uint8_t direction, uint16_t speed, uint32_t pulse){
    do{
        SendByte(id);
        SendByte(0xFD);
        SendByte(direction);
        SendByte((uint8_t)((speed >> 8) & 0x00ff));
        SendByte((uint8_t)(speed & 0x00ff));
        SendByte(0x00);
        SendByte((uint8_t)((pulse >> 24) & 0xff));
        SendByte((uint8_t)((pulse >> 16) & 0xff));
        SendByte((uint8_t)((pulse >> 8) & 0xff));
        SendByte((uint8_t)(pulse & 0xff));
        SendByte(0x00);
        SendByte(0x00);
        SendByte(0x6B);
		Delay_ms(8);
    }while(!motor_read_get_position_control_recieve(id));
	   
    return 0;
}

/// @brief 发送强制停止命令
/// @param  
/// @return 
uint8_t motor_stop(uint8_t id){
    do{
        SendByte(id);
        SendByte(0xFE);
        SendByte(0x98);
        SendByte(0x00);
        SendByte(0x6B);
		Delay_ms(8);
    }while(!motor_read_get_stop_recieve(id));
    return 0;
}

/// @brief 发送回零命令
/// @param  
/// @return 
uint8_t motor_set0(uint8_t id){
    do{
        SendByte(id);
        SendByte(0x9A);
        SendByte(0x02);
        SendByte(0x00);
        SendByte(0x6B);
		Delay_ms(8);
    }while(!motor_read_get_set0_recieve(id));    
    return 0;

}

/// @brief 发送回零标志位查询命令
/// @param  
/// @return 
uint8_t motor_send_get_flag_set0(uint8_t id){

        SendByte(id);
        SendByte(0x3B);
        SendByte(0x6B);
		Delay_ms(8);  
    return 0;
} //0x3B

/// @brief 读回零标志位，1为回零成功，0为回零失败
/// @param  void
/// @return 1为回零成功，0为回零失败
uint8_t motor_read_get_flag_set0(uint8_t id){
    while(GetBufBitsNoRead(&serial_motor_buffer) != 0){
        if(read_byte() == id){
            if(read_byte() == 0x3B){
                while(GetBufBitsNoRead(&serial_motor_buffer) >= 2){
                    if(serial_motor_buffer.padd_rec_buf[(serial_motor_buffer.read_index + 1) % serial_motor_buffer.rec_size] == 0x6B){
                        uint8_t flag = serial_motor_buffer.padd_rec_buf[serial_motor_buffer.read_index];
                        uint8_t result = ~((flag & 0x04) | (flag & 0x08));
                        serial_motor_buffer.read_index = (serial_motor_buffer.read_index + 2) % serial_motor_buffer.rec_size;
                        return result;
                    }
                    serial_motor_buffer.read_index = (serial_motor_buffer.read_index + 2) % serial_motor_buffer.rec_size;
                }
            }
        }
    }
    return 0;
}
/// @brief 发送电机标志位查询命令
/// @param  
/// @return 
uint8_t motor_send_get_flag_arrive (uint8_t id) {
        SendByte(id);
        SendByte(0x3A);
        SendByte(0x6B);
		Delay_ms(8);
    return 0;
} //0x3B

/// @brief 读电机位置模式到位标志位，0为未到位，1为到位
/// @param  void
/// @return 0为未到位，1为到位
uint8_t motor_read_get_flag_arrive(uint8_t id){
    while(GetBufBitsNoRead(&serial_motor_buffer) != 0){
        if(read_byte() == id){
            if(read_byte() == 0x3A){
                while(GetBufBitsNoRead(&serial_motor_buffer) >= 2){
                    if(serial_motor_buffer.padd_rec_buf[(serial_motor_buffer.read_index + 1) % serial_motor_buffer.rec_size] == 0x6B){
                        uint8_t flag = serial_motor_buffer.padd_rec_buf[serial_motor_buffer.read_index];
                        uint8_t result = flag & 0x02;
                        serial_motor_buffer.read_index = (serial_motor_buffer.read_index + 2) % serial_motor_buffer.rec_size;
                        return result;
                    }
                    serial_motor_buffer.read_index = (serial_motor_buffer.read_index + 2) % serial_motor_buffer.rec_size;
                }
            }
        }
    }
    return 0;
}

/// @brief 串口中断，数据进入缓冲区
/// @param  
void USART1_IRQHandler(void){
    if(USART_GetITStatus(USART1,USART_IT_RXNE)==1){
        serial_motor_buffer.padd_rec_buf[serial_motor_buffer.write_index] = (uint8_t)USART_ReceiveData(USART1);
        serial_motor_buffer.write_index = (serial_motor_buffer.write_index + 1) % serial_motor_buffer.rec_size;

        USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    }
}
