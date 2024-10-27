 #include "stm32f10x.h"

 #define pump_in1port GPIO
 #define pump_in1_pin  GPIO_Pin_6
 
uint8_t pump_init(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitTypeDef GPIO_Initstructure;
    GPIO_Initstructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Initstructure.GPIO_Pin = pump_pin;
    GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_Initstructure);

    return 0;
    }

uint8_t pump_start(void){
    GPIO_SetBits(GPIOA, pump_pin);
	return 0;
}

uint8_t pump_stop(void){
    GPIO_ResetBits(GPIOA, pump_pin);
	return 0;
}
