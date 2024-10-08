#include "stm32f10x.h"
#include "SerialDebug.h"

#define SerialDebug_Port USART3
#define SerialDebug_TX_Pin GPIO_Pin_10
#define SerialDebug_TX_Port GPIOB
#define SerialDebug_RX_Pin GPIO_Pin_11
#define SerialDebug_RX_Port GPIOB

#define SerialDebug_ClkCmd RCC_APB1PeriphClockCmd
#define SerialDebug_Clk RCC_APB1Periph_USART3

#define SerialDebug_BaudRate 9600

#define SerialDebug_GPIO_ClkCmd RCC_APB2PeriphClockCmd
#define SerialDebug_GPIO_Clk RCC_APB2Periph_GPIOB

#define SerialDebug_IRQn USART3_IRQn
#define SerialDebug_IRQHandler USART3_IRQHandler

uint8_t SerialDebug_ReceiveData;  

void SerialDebug_Init(){
    SerialDebug_GPIO_ClkCmd(SerialDebug_GPIO_Clk,ENABLE);

    GPIO_InitTypeDef GPIO_InitStucture;
    GPIO_InitStucture.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStucture.GPIO_Pin = SerialDebug_TX_Pin;
    GPIO_InitStucture.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SerialDebug_TX_Port,&GPIO_InitStucture);

    GPIO_InitStucture.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStucture.GPIO_Pin = SerialDebug_RX_Pin;
    GPIO_Init(SerialDebug_RX_Port,&GPIO_InitStucture);


   	SerialDebug_ClkCmd(SerialDebug_Clk,ENABLE);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = SerialDebug_BaudRate;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(SerialDebug_Port,&USART_InitStructure);

    USART_ITConfig(SerialDebug_Port,USART_IT_RXNE,ENABLE);    

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    NVIC_InitTypeDef NVIC_InitStruture;
    NVIC_InitStruture.NVIC_IRQChannel = SerialDebug_IRQn;
    NVIC_InitStruture.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruture.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruture.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStruture);


    USART_Cmd(SerialDebug_Port,ENABLE);
}

void USART3_IRQHandler(void){
    if(USART_GetITStatus(SerialDebug_Port,USART_IT_RXNE)==1){
        SerialDebug_ReceiveData=(uint8_t)USART_ReceiveData(SerialDebug_Port);
        USART_ClearITPendingBit(SerialDebug_Port,USART_IT_RXNE);
    }

}
uint8_t SerialDebug_GetReceiveData(void){
    return SerialDebug_ReceiveData;
}

void SerialDebug_SendByte(uint8_t data){
    USART_SendData(SerialDebug_Port,(uint16_t)data);
    while(USART_GetFlagStatus(SerialDebug_Port,USART_FLAG_TXE)==0);
}

void SerialDebug_SendData(uint8_t *data,uint8_t length){
    for (int i = 0; i < length; i++)
    {
        USART_SendData(SerialDebug_Port,(uint16_t)*(data++));
        while(USART_GetFlagStatus(SerialDebug_Port,USART_FLAG_TXE)==0);
    }
}

