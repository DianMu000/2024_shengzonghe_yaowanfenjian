#include "key.h"
#include "stm32f10x.h"
#include "delay.h"
#include "motor.h"
#include "pump.h"
#include "led.h"
#include "motor_covering_round.h"







uint8_t key_init(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = key_r_pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(key_r_port, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = key_g_pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(key_g_port, &GPIO_InitStructure);
    
    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = key_b_pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(key_b_port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = key_stop_pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(key_stop_port, &GPIO_InitStructure);
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);

    EXTI_InitTypeDef EXTI_Initsturture;
    EXTI_Initsturture.EXTI_Line = EXTI_Line12;
    EXTI_Initsturture.EXTI_LineCmd = ENABLE;
    EXTI_Initsturture.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_Initsturture.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_Initsturture);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitTypeDef NVIC_Initstructure;
    NVIC_Initstructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_Initstructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Initstructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_Initstructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_Initstructure);

    return 0;
}

uint8_t key_read_color_plate_and_color_bottle(color_t* color_plate, color_t* color_bottle){
    while(*color_plate == none){
        if(GPIO_ReadInputDataBit(key_r_port, key_r_pin) == 0){
            *color_plate = red;
			
            Delay_ms(100);
            while(GPIO_ReadInputDataBit(key_r_port, key_r_pin) == 0);
            Delay_ms(100);
        }
        else if(GPIO_ReadInputDataBit(key_g_port, key_g_pin) == 0){
            *color_plate = green;
			
            Delay_ms(100);
            while(GPIO_ReadInputDataBit(key_g_port, key_g_pin) == 0);
            Delay_ms(100);
        }
        else if(GPIO_ReadInputDataBit(key_b_port, key_b_pin) == 0){
            *color_plate = blue;
			
            Delay_ms(100);
            while(GPIO_ReadInputDataBit(key_b_port, key_b_pin) == 0);
            Delay_ms(100);

        }
    }
	
	led_b_on();
	Delay_ms(200);
	led_b_off();
	
    while(*color_bottle == none){
        if(GPIO_ReadInputDataBit(key_r_port, key_r_pin) == 0){
            *color_bottle = red;
			
            Delay_ms(100);
            while(GPIO_ReadInputDataBit(key_r_port, key_r_pin) == 0);
            Delay_ms(100);
        }
        else if(GPIO_ReadInputDataBit(key_g_port, key_g_pin) == 0){
            *color_bottle = green;
			
            Delay_ms(100);
            while(GPIO_ReadInputDataBit(key_g_port, key_g_pin) == 0);
            Delay_ms(100);
        }
        else if(GPIO_ReadInputDataBit(key_b_port, key_b_pin) == 0){
            *color_bottle = blue;
			
            Delay_ms(100);
            while(GPIO_ReadInputDataBit(key_b_port, key_b_pin) == 0);
            Delay_ms(100);
        }
    }
	
	
	led_b_on();
	Delay_ms(200);
	led_b_off();
	
    return 0;
}

void EXTI15_10_IRQHandler(void){
    if (EXTI_GetITStatus(EXTI_Line12) == SET)
	{
		motor_stop(motor_x_id);
		motor_stop(motor_y_id);
		motor_stop(motor_z_id);
		
		pump_off();
		
		motor_covering_round_off();
		
		led_b_on();
		
		Delay_ms(100);
		while(GPIO_ReadInputDataBit(key_stop_port, key_stop_pin) == RESET);//
		Delay_ms(100);
		
		while(GPIO_ReadInputDataBit(key_stop_port, key_stop_pin) == SET);
		
		led_r_on();
		
		motor_set0(motor_x_id);
		while(motor_get_flag_set0(motor_x_id) == 0);
		motor_set0(motor_y_id);
		while(motor_get_flag_set0(motor_y_id) == 0);
		motor_set0(motor_z_id);
		while(motor_get_flag_set0(motor_z_id) == 0);

		while(1);
	}

}
