#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "bsp_motor.h"


serial_motor_buffer_t  serial_motor_buffer;
int main(void)
{
	motor_init(&serial_motor_buffer);
	
	motor_x_set0();
	
	

	while(1)
	{
		
		
	}
}
