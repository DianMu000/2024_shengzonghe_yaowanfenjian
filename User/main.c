#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "bsp_motor.h"
#include "stdbool.h"




serial_motor_buffer_t  serial_motor_buffer;
int main(void)
{
	motor_init(&serial_motor_buffer);
	
	motor_move_speed(motor_x_id, 1, 300);
	
//	until_motor_get_flag_(motor_x_id, set0);

//	motor_move_position(motor_x_id, 1, 300 ,16000);
	


	while(1)
	{

		
	}
}
