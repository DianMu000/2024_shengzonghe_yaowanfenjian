
#include "position.h"

/// @brief 获取当前位置信息,如果在移动过程中返回的是的移动到位之后的坐标
/// @param position 自身结构体
/// @param pposition_x 储存x坐标的指针
/// @param pposition_y 储存y坐标的指针
/// @param pposition_z 储存z坐标的指针
/// @return 
static uint8_t get_position (position_t *position, float *pposition_x, float *pposition_y, float *pposition_z){
    *pposition_x = position->position_inf.position_x;
    *pposition_y = position->position_inf.position_y;
    *pposition_z = position->position_inf.position_z;

    return 0;
}

/// @brief x轴的移动
/// @param position 自身结构体
/// @param direction 方向，0为正向，1为负向
/// @param distance 移动的距离
/// @param speed 速度
/// @return 
static uint8_t move_x (position_t *position, uint8_t direction, uint16_t speed, float distance){
	
	float move_distance;
	
	int16_t target_position_x;	
	if(direction == 0) target_position_x = position->position_inf.position_x + distance;
	else target_position_x = position->position_inf.position_x - distance;
	
	if(target_position_x > position->position_inf.position_x_max){
		move_distance = position->position_inf.position_x_max - position->position_inf.position_x;
		position->position_inf.position_x = position->position_inf.position_x_max;
	}
	
	else if(target_position_x < position->position_inf.position_x_min){
		move_distance = position->position_inf.position_x - position->position_inf.position_x_min;
		position->position_inf.position_x = position->position_inf.position_x_min;
	}
	else{
		move_distance = distance;		
		position->position_inf.position_x = target_position_x;

	}
	
    position->position_interface.pfmove_x(direction ^ position->position_conf.polarity_x, speed, move_distance);
    

    return 0;
}

/// @brief y轴的移动
/// @param position 自身结构体
/// @param direction 方向，0为正向，1为负向
/// @param distance 移动的距离
/// @param speed 速度
/// @return 
static uint8_t move_y (position_t *position, uint8_t direction,  uint16_t speed, float distance){
    
	float move_distance;
	
	int16_t target_position_y;	
	if(direction == 0) target_position_y = position->position_inf.position_y + distance;
	else target_position_y = position->position_inf.position_y - distance;
	
	if(target_position_y > position->position_inf.position_y_max){
		move_distance = position->position_inf.position_y_max - position->position_inf.position_y;
		position->position_inf.position_y = position->position_inf.position_y_max;
	}
	
	else if(target_position_y < position->position_inf.position_y_min){
		move_distance = position->position_inf.position_y - position->position_inf.position_y_min;
		position->position_inf.position_y = position->position_inf.position_y_min;
	}
	else{
		move_distance = distance;
		position->position_inf.position_y = target_position_y;

	}

    position->position_interface.pfmove_y(direction ^ position->position_conf.polarity_y, speed, move_distance);
    return 0;
}

/// @brief z轴的移动
/// @param position 自身结构体
/// @param direction 方向，0为正向，1为负向
/// @param distance 移动的距离
/// @param speed 速度
/// @return 
static uint8_t move_z (position_t *position, uint8_t direction, uint16_t speed, float distance){

	float move_distance;
	
	int16_t target_position_z;	
	if(direction == 0) target_position_z = position->position_inf.position_z + distance;
	else target_position_z = position->position_inf.position_z - distance;
	
	if(target_position_z > position->position_inf.position_z_max){
		move_distance = position->position_inf.position_z_max - position->position_inf.position_z;
		position->position_inf.position_z = position->position_inf.position_z_max;
	}
	
	else if(target_position_z < position->position_inf.position_z_min){
		move_distance = position->position_inf.position_z - position->position_inf.position_z_min;
		position->position_inf.position_z = position->position_inf.position_z_min;
	}
	else{
		move_distance = distance;
		position->position_inf.position_z = target_position_z;

	}
	
    position->position_interface.pfmove_z(direction ^ position->position_conf.polarity_z, speed, move_distance);
        
    return 0;
}

/// @brief 移动到目标xy位置
/// @param position 自身结构体
/// @param position_x_target x坐标
/// @param position_y_target y坐标
/// @param speed 速度
/// @return 
static uint8_t move_to_xy (position_t *position, float position_x_target, float position_y_target, uint16_t speed){
    float distance_x = position_x_target - position->position_inf.position_x;
    float distance_y = position_y_target - position->position_inf.position_y;

    if(distance_x < 0){
        move_x(position, 1, speed, -1 * distance_x);
    }
    else{
        move_x(position, 0, speed, distance_x);
    }
	
	if(distance_y < 0){
        move_y(position, 1, speed, -1 * distance_y);
    }
    else{
        move_y(position, 0, speed, distance_y);
    }

 

    return 0;
}
/// @brief 移动到目标z位置
/// @param position 自身结构体
/// @param position_z_target z坐标
/// @param speed 速度
/// @return 
static uint8_t move_to_z (position_t *position, float position_z_target, uint16_t speed){
    float distance_z = position_z_target - position->position_inf.position_z;


    if(distance_z < 0){
        move_z(position, 1, speed, -1 * distance_z);
    }
    else{
        move_z(position, 0, speed, distance_z);
    }



    return 0;
}

// @brief 获得移动到位标志位，1为到位，0为不到位
// @param position 
// @return 
static uint8_t get_flag_move(position_t *position){
    return position->position_interface.pfget_flag_move();
}

// @brief 获得回零到位标志位，1为到位，0为不到位
// @param position 
// @return 
static uint8_t get_flag_set0(position_t *position){
    return position->position_interface.pfget_flag_set0();
}
// @brief 结构体初始化
// @param position 初始化的结构体
// @param position_interface 接口函数结构体
// @param position_conf 配置结构体
// @param position_x_max x轴的最大值
//0 @param position_x_min x轴的最小值
// @param position_y_max y轴的最大值
// @param position_y_min y轴的最小值
// @param position_z_max z轴的最大值
// @param position_z_min z轴的最小值
// @return 
uint8_t position_initialize(position_t *position, position_interface_t *position_interface, position_conf_t *position_conf,
float position_x_max, float position_x_min, float position_y_max, float position_y_min, float position_z_max, float position_z_min)
{
    assert_param(position != NULL && position_interface != NULL && position_conf != 0);
    assert_param(position_interface->pfmove_x != NULL && position_interface->pfmove_y != NULL && 
                position_interface->pfmove_z && position_interface->pfget_flag_move && position_interface->pfget_flag_set0);
 
    position->position_interface.pfmove_x = position_interface->pfmove_x;
    position->position_interface.pfmove_y = position_interface->pfmove_y;
    position->position_interface.pfmove_z = position_interface->pfmove_z;
    position->position_interface.pfget_flag_move = position_interface->pfget_flag_move;
    position->position_interface.pfget_flag_set0 = position_interface->pfget_flag_set0;

    position->position_inf.position_x = 0;
    position->position_inf.position_x_min = position_x_min;
    position->position_inf.position_x_max = position_x_max;
    position->position_inf.position_y = 0;
    position->position_inf.position_y_max = position_y_max;
    position->position_inf.position_y_min = position_y_min;
    
    position->position_inf.position_z = 0;
    position->position_inf.position_z_max = position_z_max;
    position->position_inf.position_z_min = position_z_min;

    position->position_conf.polarity_x = position_conf->polarity_x;
    position->position_conf.polarity_y = position_conf->polarity_y;
    position->position_conf.polarity_z = position_conf->polarity_z;


    position->pfget_position = get_position;
    position->pfmove_to_xy = move_to_xy;
    position->pfmove_to_z = move_to_z;
    position->pfmove_x = move_x;
    position->pfmove_y =move_y;
    position->pfmove_z =move_z;
    position->pfget_flag_move = get_flag_move;
    position->pfget_flag_set0 = get_flag_set0;

    return 0;
}




