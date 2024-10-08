#include "stm32f10x.h"


void SerialDebug_Init(void);

uint8_t SerialDebug_GetReceiveData(void);

void SerialDebug_SendByte(uint8_t data);

void SerialDebug_SendData(uint8_t *data,uint8_t length);
