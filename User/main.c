#include "stm32f10x.h"                  // Device header
#include "Delay.h"

#include "motor.h"
#include "serial_openmv.h"
#include "key.h"
#include "led.h"
#include "motor_covering_round.h"
#include "pump.h"
#include "servos_body.h"
#include "servos_covering.h"

#include "position.h"
#include "position_axis.h"

#include "SerialDebug.h"

#define pulse_to_distance_fator 1600



uint8_t motor_move_x(uint8_t dircetion, uint16_t speed, float distance);
uint8_t motor_move_y(uint8_t dircetion, uint16_t speed,  float distance);
uint8_t motor_move_z(uint8_t dircetion, uint16_t speed, float distance);
uint8_t motor_inter_get_flag_arrive(void);
uint8_t motor_inter_get_flag_set0(void);

uint8_t motor_x_moved_flag;
uint8_t motor_y_moved_flag;
uint8_t motor_z_moved_flag;

void assert_failed(uint8_t* file, uint32_t line)
{
    while(1);
}

serial_openmv_buffer_t serial_openmv_buffer;
serial_motor_buffer_t  serial_motor_buffer;

//如果需要连续发送命令，电机每个命令之间间隔100ms，保证数据能被完整接受
int main(void)
{
	
	//初始化标志位
	motor_x_moved_flag = 0;
	motor_y_moved_flag = 0;
	motor_z_moved_flag = 0;
	//颜色状态标志
	color_t color_bottle = none;
	color_t color_plate = none;
	
	//初始化（usart2初始化之后再初始化LED会发送错误数据，原因待查明）
	SerialDebug_Init();
	led_init();
	motor_init(&serial_motor_buffer);
	openmv_init(&serial_openmv_buffer);
	key_init();
	motor_covering_round_init();
	pump_init();
	servos_body_init();
	servos_covering_init();
	
	servos_body_off();
	servos_covering_off();
	pump_off();
	motor_covering_round_off();

	//初始化position
	position_t position;
	
	position_conf_t position_conf;
	position_conf.polarity_x = negative;
	position_conf.polarity_y = positive;
	position_conf.polarity_z = positive;
	
	position_interface_t position_interface;
	position_interface.pfget_flag_move = motor_inter_get_flag_arrive;
	position_interface.pfget_flag_set0 = motor_inter_get_flag_set0;
	position_interface.pfmove_x = motor_move_x;
	position_interface.pfmove_y = motor_move_y;
	position_interface.pfmove_z = motor_move_z;
	
	position_initialize(&position, &position_interface, &position_conf, axis_x_max, axis_x_min, axis_y_max, axis_y_min, axis_z_max, axis_z_min);



	//按键读取颜色标志，按下一次就闪烁一次）
	key_read_color_plate_and_color_bottle(&color_plate, &color_bottle);

	
//*********************debug**************************
	//第一次移动，使所有轴都移动，使电机到位标志位置1
//	position.pfmove_to_z(&position,axis_z_moving, 1000);
//	
//	position.pfmove_to_xy(&position, 10, 10, 1000);
//	while(position.pfget_flag_move(&position) == 0);

	
//	Delay_s(1);
//	openmv_send(pill_red);
//	Delay_ms(100);
//	uint8_t openmv_result = openmv_read();
//	if(openmv_result == 0){
//		SerialDebug_SendData((uint8_t*)serial_openmv_buffer.decode_buf, 2);
//	}
//	else{
//		SerialDebug_SendByte(openmv_result);
//	}
//	


	//*****************************校准视觉***************************************
//	//移动到识别位置xy
//		position.pfmove_to_xy(&position, (float)(axis_x_plate_identify - axis_x_openmv), (float)(axis_y_plate_identify - axis_y_openmv), 2000);
//		while(position.pfget_flag_move(&position) == 0);
//		//移动到识别位置z
//		position.pfmove_to_z(&position, axis_z_identify ,2000);
//		while(position.pfget_flag_move(&position) == 0);
//		Delay_ms(700);
//	//识别
//		openmv_send((state_openmv_t)color_plate);
//		Delay_ms(500);
//		if(openmv_read() == 0){
//			//识别到药丸，抬起并移动xy
//			position.pfmove_to_z(&position,axis_z_moving, 2000);
//			while(position.pfget_flag_move(&position) == 0);
//			
//			float diatance_x = serial_openmv_buffer.decode_buf[0] + axis_x_openmv - axis_x_pump;
//			float diatance_y = serial_openmv_buffer.decode_buf[1];

//			if(diatance_x >= 0){
//				position.pfmove_x(&position, 0, 2000, (float) diatance_x);
//			}
//			else{
//				position.pfmove_x(&position, 1, 2000, (float)(-1 * diatance_x));
//			}
//			
//			if(diatance_y >= 0){
//				position.pfmove_y(&position, 0, 2000, (float) diatance_y);
//			}
//			else{
//				position.pfmove_y(&position, 1, 2000, (float)(-1 * diatance_y));
//			}
//			
//			while(position.pfget_flag_move(&position) == 0);
//			Delay_ms(100);
//			
//			//下降
//			position.pfmove_to_z(&position, (float)axis_z_pill_suck, 2000);
//			pump_on();
//			while(position.pfget_flag_move(&position) == 0);
//			Delay_ms(100);
//			//吸取
//			Delay_ms(2000);
//			pump_off();
//			//上升准备移动
//			position.pfmove_to_z(&position, (float)axis_z_moving, 2000);
//			while(position.pfget_flag_move(&position) == 0);
//			Delay_ms(100);
//	}
		//******************************************************************************

//position.pfmove_to_z(&position, axis_z_identify ,2000);

//		position.pfmove_to_xy(&position, (float)(axis_x_plate_identify - axis_x_openmv), (float)(axis_y_plate_identify - axis_y_openmv), 2000);

//position.pfmove_to_z(&position, axis_z_pill_suck, 2000);
//pump_on();



//*********************body!!!**************************



		
//药丸抓取到药瓶部分
	float arr_axis_x_plate_place[6] = {
	axis_x_plate_place_1,
	axis_x_plate_place_2,
	axis_x_plate_place_3,
	axis_x_plate_place_4,
	axis_x_plate_place_5,
	axis_x_plate_place_6,
	
	};
	
	float arr_axis_y_plate_place[6] = {
	axis_y_plate_place_1,
	axis_y_plate_place_2,
	axis_y_plate_place_3,
	axis_y_plate_place_4,
	axis_y_plate_place_5,
	axis_y_plate_place_6,
	
	};
uint8_t identified_flag = 1;
uint8_t noresult = 0;
	
	//将药丸吸取到药板上，6次
for(uint8_t index_identified = 0; index_identified < 6;  ){
	if (identified_flag == 1){
	//移动到识别位置xy
		position.pfmove_to_xy(&position, (float)(axis_x_plate_identify - axis_x_openmv), (float)(axis_y_plate_identify - axis_y_openmv), 2000);
		while(position.pfget_flag_move(&position) == 0);
		//移动到识别位置z
		position.pfmove_to_z(&position, axis_z_identify ,2000);
		while(position.pfget_flag_move(&position) == 0);
		motor_move_position(motor_x_id, 0, 1000, 1600*1);
		motor_move_position(motor_y_id, 1, 1000, 1600*1);

		Delay_ms(750);
	}
	//识别
		
		openmv_send((state_openmv_t)color_plate);
		Delay_ms(500);
		if(openmv_read() == 0){
			identified_flag = 1;
			noresult = 0;
			//识别到药丸，抬起并移动xy
			position.pfmove_to_z(&position,axis_z_moving, 2000);
			while(position.pfget_flag_move(&position) == 0);
			Delay_ms(100);

			float diatance_x = serial_openmv_buffer.decode_buf[0] + axis_x_openmv - axis_x_pump;
			float diatance_y = serial_openmv_buffer.decode_buf[1];

			if(diatance_x >= 0){
				position.pfmove_x(&position, 0, 2000, (float) diatance_x);
			}
			else{
				position.pfmove_x(&position, 1, 2000, (float)(-1 * diatance_x));
			}
			
			if(diatance_y >= 0){
				position.pfmove_y(&position, 0, 2000, (float) diatance_y);
			}
			else{
				position.pfmove_y(&position, 1, 2000, (float)(-1 * diatance_y));
			}
			
			while(position.pfget_flag_move(&position) == 0);
			Delay_ms(100);
			
			//下降
			position.pfmove_to_z(&position, (float)axis_z_pill_suck, 2000);
			pump_on();
			while(position.pfget_flag_move(&position) == 0);
			Delay_ms(100);
			//吸取
			Delay_ms(1250);
			//上升
			position.pfmove_to_z(&position, (float)axis_z_moving, 2000);
			while(position.pfget_flag_move(&position) == 0);
			Delay_ms(100);

			//移动到药板位置xy
			position.pfmove_to_xy(&position, (float)(arr_axis_x_plate_place[index_identified] - axis_x_pump), (float)(arr_axis_y_plate_place[index_identified] - axis_y_pump), 2000);
			while(position.pfget_flag_move(&position) == 0);
			Delay_ms(100);
			//下降移动到药板位置z
			position.pfmove_to_z(&position, (float)(axis_z_pill_place), 2000);
			while(position.pfget_flag_move(&position) == 0);
			Delay_ms(100);
			//放下
			pump_off();
			Delay_ms(1250);
			//上升准备移动
			position.pfmove_to_z(&position, (float)axis_z_moving, 2000);
			while(position.pfget_flag_move(&position) == 0);
			Delay_ms(100);
			
			index_identified++;
			
			
		}	
		else{
			identified_flag = 0;
			noresult ++;
		}
		if(noresult >=12){break;}
	
	}
		
	identified_flag = 1;
	noresult = 0;
//药丸抓取到药瓶部分
//将药丸吸取到药瓶上，6次
	//移动到识别位置xy
for(uint8_t index_identified = 0; index_identified < 6;  ){
	if (identified_flag == 1){
		position.pfmove_to_xy(&position, (float)(axis_x_plate_identify - axis_x_openmv - index_identified), (float)(axis_y_plate_identify - axis_y_openmv), 2000);
		while(position.pfget_flag_move(&position) == 0);
		//移动到识别位置z
		position.pfmove_to_z(&position, axis_z_identify ,2000);
		while(position.pfget_flag_move(&position) == 0);
		motor_move_position(motor_x_id, 0, 1000, 1600*1);
		motor_move_position(motor_y_id, 1, 1000, 1600*1);

		Delay_ms(700);
	}
	//识别

		openmv_send((state_openmv_t)color_bottle);
		Delay_ms(500);
		if(openmv_read() == 0){
			identified_flag = 1;
			noresult = 0;
			//识别到药丸，抬起并移动xy
			position.pfmove_to_z(&position,axis_z_moving, 2000);
			while(position.pfget_flag_move(&position) == 0);
			Delay_ms(100);

			float diatance_x = serial_openmv_buffer.decode_buf[0] + axis_x_openmv - axis_x_pump;
			float diatance_y = serial_openmv_buffer.decode_buf[1];

			if(diatance_x >= 0){
				position.pfmove_x(&position, 0, 2000, (float) diatance_x);
			}
			else{
				position.pfmove_x(&position, 1, 2000, (float)(-1 * diatance_x));
			}
			
			if(diatance_y >= 0){
				position.pfmove_y(&position, 0, 2000, (float) diatance_y);
			}
			else{
				position.pfmove_y(&position, 1, 2000, (float)(-1 * diatance_y));
			}
			
			while(position.pfget_flag_move(&position) == 0);
			Delay_ms(100);
			
			//下降
			position.pfmove_to_z(&position, (float)axis_z_pill_suck, 2000);
			pump_on();
			while(position.pfget_flag_move(&position) == 0);
			Delay_ms(100);
			//吸取
			Delay_ms(1250);
			//上升
			position.pfmove_to_z(&position, (float)axis_z_moving, 2000);
			while(position.pfget_flag_move(&position) == 0);
			Delay_ms(100);

			//移动到药瓶放药丸位置xy
			position.pfmove_to_xy(&position, (float)(axis_x_bottle_body - axis_x_pump), (float)(axis_y_bottle_body - axis_y_pump), 2000);
			while(position.pfget_flag_move(&position) == 0);
			Delay_ms(100);
			//下降移动放药瓶位置z
			position.pfmove_to_z(&position, (float)(axis_z_bottle_pill_place), 2000);
			while(position.pfget_flag_move(&position) == 0);
			Delay_ms(100);
			//放下
			pump_off();
			Delay_ms(1250);
			//上升准备移动
			position.pfmove_to_z(&position, (float)axis_z_moving, 2000);
			while(position.pfget_flag_move(&position) == 0);
			Delay_ms(100);
			
			index_identified++;
			
		}	
		else{
			identified_flag = 0;
			noresult ++;
		}
		if(noresult >=12){break;}
	}
	
	
	
	
		motor_set0(motor_x_id);
		while(motor_get_flag_set0(motor_x_id) == 0);
		motor_set0(motor_y_id);
		while(motor_get_flag_set0(motor_y_id) == 0);
		motor_set0(motor_z_id);
		while(motor_get_flag_set0(motor_z_id) == 0);

	
//	position.pfmove_to_z(&position, axis_z_identify ,2000);
//	while(position.pfget_flag_move(&position) == 0);
//	Delay_ms(700);
//有关瓶盖部分
//夹瓶盖并移动
//	position.pfmove_to_xy(&position, axis_x_bottle_covering - axis_x_motor_covering_round, axis_y_bottle_covering - axis_y_motor_covering_round, 2000);
//	while(position.pfget_flag_move(&position) == 0);
//	Delay_ms(100);
//	//下降
//	position.pfmove_to_z(&position, (float)axis_z_pill_suck, 2000);
//	while(position.pfget_flag_move(&position) == 0);
//	Delay_ms(100);
//	//抓取
//	servos_covering_on();
//	Delay_ms(1250);
//	//上升
//	position.pfmove_to_z(&position, (float)axis_z_moving, 2000);
//	while(position.pfget_flag_move(&position) == 0);
//	Delay_ms(100);



//  //移动到瓶身位置
//	position.pfmove_to_xy(&position, axis_x_bottle_body - axis_x_motor_covering_round, axis_x_bottle_body - axis_y_motor_covering_round, 2000);
//	while(position.pfget_flag_move(&position) == 0);
//	Delay_ms(100);


//	//下降到扭瓶盖最高点
//	position.pfmove_to_z(&position, (float)axis_z_bottle_covering_place_high, 2000);
//	while(position.pfget_flag_move(&position) == 0);
//	Delay_ms(100);

//	Delay_ms(500);


//  //启动电机  再下降，边扭瓶盖边下降
//  motor_covering_round_on();
//  position.pfmove_to_z(&position, (float)axis_z_bottle_covering_place_down, covering_round_motor_move_speed);
//  Delay_ms(round_time);
//	motor_covering_round_off();

//	Delay_ms(500);

//移动药瓶
//	position.pfmove_to_z(&position, (float)axis_z_moving, 2000);
//	while(position.pfget_flag_move(&position) == 0);
//	Delay_ms(100);

//	position.pfmove_to_xy(&position, 0, 0, 2000);


//松开夹爪，移动到z轴移动位置,回到零点
//	servos_covering_off();
//	position.pfmove_to_z(&position, (float)axis_z_moving, 2000);
//	while(position.pfget_flag_move(&position) == 0);
//	Delay_ms(100);
//	position.pfmove_to_xy(&position, 0, 0, 2000);




	while(1){
	}

}	

uint8_t motor_move_x(uint8_t dircetion, uint16_t speed, float distance){
	motor_move_position(motor_x_id, dircetion, speed, (uint32_t)(distance * pulse_to_distance_fator));
	motor_x_moved_flag = 1;
	return 0;
}

uint8_t motor_move_y(uint8_t dircetion, uint16_t speed,  float distance){
	motor_move_position(motor_y_id, dircetion, speed, (uint32_t)(distance * pulse_to_distance_fator));
	motor_y_moved_flag = 1;
	return 0;
}

uint8_t motor_move_z(uint8_t dircetion, uint16_t speed, float distance){
	motor_move_position(motor_z_id, dircetion, speed, (uint32_t)(distance * pulse_to_distance_fator));
	motor_z_moved_flag = 1;
	return 0;
}

uint8_t motor_inter_get_flag_arrive(void){
	uint8_t result_x;
	uint8_t result_y;
	uint8_t result_z;
	
	if(motor_x_moved_flag == 0){
		result_x = 1;
	}
	else {
		result_x = motor_get_flag_arrive(motor_x_id);
		if (result_x == 1) motor_x_moved_flag = 0;
	}
	
	if(motor_y_moved_flag == 0){
		result_y = 1;
	}
	else {
		result_y = motor_get_flag_arrive(motor_y_id);
		if (result_y == 1) motor_y_moved_flag = 0;
	}
	
	if(motor_z_moved_flag == 0){
		result_z = 1;
	}
	else {
		result_z = motor_get_flag_arrive(motor_z_id);
		if (result_z == 1) motor_z_moved_flag = 0;
	}
	
	return result_x & result_y & result_z;
}

uint8_t motor_inter_get_flag_set0(void){
	uint8_t result;
	result = motor_get_flag_set0(motor_x_id) & motor_get_flag_set0(motor_y_id) & motor_get_flag_set0(motor_z_id);
	return result;
}
