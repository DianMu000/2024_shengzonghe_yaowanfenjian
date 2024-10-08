#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "bsp_motor.h"

#include "serial_blueteeth.h"
#include "pump.h"

typedef enum{
    set0,
    x_move_forward,
    x_move_backward,
    y_move_forward,
    y_move_backward,   
    z_move_forward,
    z_move_backward,
    xyz_stop,
    pumpstart,
    pumpstop,
}   order_t;

serial_blueteeth_buffer_t serial_blueteeth_buffer;
serial_motor_buffer_t  serial_motor_buffer;
int main(void)
{
	motor_init(&serial_motor_buffer);
	blueteeth_init(&serial_blueteeth_buffer);
	pump_init();
	
	while(1)
	{
		switch((order_t)blueteeth_read()){
			case set0:
			motor_set0(motor_x_id);
				until_motor_get_flag_(motor_x_id, set0);
			break;

			case x_move_forward:
				motor_move_speed(motor_x_id, 1, 300);
			break;
			
			case x_move_backward:
				motor_move_speed(motor_x_id, 0, 300);
			break;
			
			case y_move_forward:
				motor_move_speed(motor_y_id, 1, 300);
			break;
			
			case y_move_backward:
				motor_move_speed(motor_y_id, 0, 300);
			break;
			
			case z_move_forward:
				motor_move_speed(motor_y_id, 0, 300);
			break;
			
			case z_move_backward:
				motor_move_speed(motor_z_id, 1, 300);
			break;
			
			case xyz_stop:
				motor_move_speed(motor_z_id, 0, 300);
			break;

			case pumpstart:
				pump_start();
			break;

			case pumpstop:
				pump_stop();
			break;
		}
		
	}
}
