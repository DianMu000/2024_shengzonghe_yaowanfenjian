#include "key.h"
#include "stm32f10x.h"

#define led_b_port GPIOA
#define led_b_pin GPIO_Pin_11

#define led_r_port GPIOA
#define led_r_pin GPIO_Pin_12

uint8_t led_init(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = led_b_pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(led_b_port, &GPIO_InitStructure);
	
	GPIO_SetBits(led_b_port, led_b_pin);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = led_r_pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(led_r_port, &GPIO_InitStructure);
	
	GPIO_SetBits(led_r_port, led_r_pin);
    GPIO_SetBits(led_b_port, led_b_pin);

	return 0;
}

uint8_t led_b_on(void){
    GPIO_ResetBits(led_b_port, led_b_pin);
	return 0;
}

uint8_t led_b_off(void){
    GPIO_SetBits(led_b_port, led_b_pin);
	return 0;
}

uint8_t led_r_on(void){
    GPIO_ResetBits(led_r_port, led_r_pin);
	return 0;
}

uint8_t led_r_off(void){
    GPIO_SetBits(led_r_port, led_r_pin);
	return 0;
}
