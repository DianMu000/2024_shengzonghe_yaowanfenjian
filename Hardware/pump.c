#include "pump.h"

#define pump_ccr_start 20000
#define pump_ccr_stop 0

 
#define pump_in1_port GPIOA
#define pump_in1_pin GPIO_Pin_4

#define pump_in2_port GPIOA
#define pump_in2_pin GPIO_Pin_5


#define pump_pwm_port GPIOA
#define pump_pwm_pin GPIO_Pin_6

uint8_t pump_init(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_Initstructure;
    GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Initstructure.GPIO_Pin = pump_in1_pin;
    GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(pump_in1_port, &GPIO_Initstructure);

    GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Initstructure.GPIO_Pin = pump_in2_pin;
    GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(pump_in2_port, &GPIO_Initstructure);

    GPIO_Initstructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Initstructure.GPIO_Pin = pump_pwm_pin;
    GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(pump_pwm_port, &GPIO_Initstructure);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	 
	TIM_InternalClockConfig(TIM3);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;		//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;		//PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;		//CCR
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    

    TIM_Cmd(TIM3, ENABLE);
    TIM_SetCompare1(TIM3, pump_ccr_stop);

    return 0;
    }

uint8_t pump_on(void){
	GPIO_SetBits(pump_in1_port, pump_in1_pin);
	GPIO_ResetBits(pump_in2_port, pump_in2_pin);
	
    TIM_SetCompare1(TIM3, pump_ccr_start);
	return 0;
}

uint8_t pump_off(void){
	GPIO_ResetBits(pump_in1_port, pump_in1_pin);
	GPIO_ResetBits(pump_in2_port, pump_in2_pin);
	
    TIM_SetCompare1(TIM3, pump_ccr_stop);
	return 0;
}
