#include "key.h"

#define motor_covering_round_ccr_start 200
#define motor_covering_round_ccr_stop 0


#define motor_covering_round_in1_port GPIOA
#define motor_covering_round_in1_pin GPIO_Pin_0

#define motor_covering_round_in2_port GPIOA
#define motor_covering_round_in2_pin GPIO_Pin_1


#define motor_covering_round_pwm_port GPIOA
#define motor_covering_round_pwm_pin GPIO_Pin_7

uint8_t motor_covering_round_init(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_Initstructure;
    GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Initstructure.GPIO_Pin = motor_covering_round_in1_pin;
    GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(motor_covering_round_in1_port, &GPIO_Initstructure);

    GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Initstructure.GPIO_Pin = motor_covering_round_in2_pin;
    GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(motor_covering_round_in2_port, &GPIO_Initstructure);

    GPIO_Initstructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Initstructure.GPIO_Pin = motor_covering_round_pwm_pin;
    GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(motor_covering_round_pwm_port, &GPIO_Initstructure);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);


	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;		//CCR
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    

    TIM_Cmd(TIM3, ENABLE);
    TIM_SetCompare2(TIM3, motor_covering_round_ccr_stop);

    return 0;
    }

uint8_t motor_covering_round_on(void){
	GPIO_SetBits(motor_covering_round_in1_port, motor_covering_round_in1_pin); 
    GPIO_ResetBits(motor_covering_round_in2_port, motor_covering_round_in2_pin);
	TIM_SetCompare2(TIM3, motor_covering_round_ccr_start);
	return 0;
}

uint8_t motor_covering_round_off(void){
	GPIO_ResetBits(motor_covering_round_in1_port, motor_covering_round_in1_pin); 
    GPIO_ResetBits(motor_covering_round_in2_port, motor_covering_round_in2_pin);
    TIM_SetCompare2(TIM3, motor_covering_round_ccr_stop);
	return 0;
}
