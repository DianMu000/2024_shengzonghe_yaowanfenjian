#include "servos_covering.h"
#include "stm32f10x.h"



#define servos_covering_pwm_port GPIOB
#define servos_covering_pwm_pin GPIO_Pin_0

uint8_t servos_covering_init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = servos_covering_pwm_pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(servos_covering_pwm_port, &GPIO_InitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;		
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	
	TIM_Cmd(TIM3, ENABLE);
	TIM_SetCompare3(TIM3, servos_covering_crr_off);
	
    return 0;
}
uint8_t servos_covering_on(void){
    TIM_SetCompare3(TIM3, servos_covering_crr_on);
    return 0;
}
uint8_t servos_covering_off(void){
    TIM_SetCompare3(TIM3, servos_covering_crr_off);
    return 0;
}

