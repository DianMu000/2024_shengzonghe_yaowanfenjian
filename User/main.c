#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "bsp_motor.h"
#include "stdbool.h"




serial_motor_buffer_t  serial_motor_buffer;
int main(void)
{
	motor_init(&serial_motor_buffer);
	
	motor_x_set0();
	
	until_motor_x_get_flag_(set0);

	motor_x_move_position(1,30 ,16000);
	


	while(1)
	{

		
	}
}
