#include "bsp_motor.h"
#include "position.h"



#define _approach(position_approach_method, x, y) approach_##position_approach_method(x, y)//宏定义中逐级展开，实参被##和#处理之后不在展开，因此在此之前就应该用一级宏函数将实参替换
#define approach(position_approach_method, x, y) _approach(position_approach_method, x, y)

uint8_t position_approach(int16_t x, int16_t y){
    approach(position_approach_method, x, y);
} 


static uint8_t approch_pid(int16_t x, int16_t y){
    uint8_t result_x = PID_calculate(x);
    uint8_t result_y = PID_calculate(y);

    if(result_x >= 0){
        motor_move_speed(motor_x_id, 1, result_x);
    }
    else{
        motor_move_speed(motor_x_id, 0, -result_x);
    }
    if(result_y >= 0){
        motor_move_speed(motor_y_id, 1, result_y);
    
    }
    else{
        motor_move_speed(motor_y_id, 0, -result_y);
    }
}

static uint8_t approch_absolute(int16_t x, int16_t y){
    uint8_t result_x = map_distance_to_pulse(x);
    uint8_t result_y = map_distance_to_pulse(y);

    if(result_x >= 0){
        motor_move_speed(motor_x_id, 1, result_x);
    }
    else{
        motor_move_speed(motor_x_id, 0, -result_x);
    }
    if(result_y >= 0){
        motor_move_speed(motor_y_id, 1, result_y);
    
    }
    else{
        motor_move_speed(motor_y_id, 0, -result_y);
    }
}


static uint8_t map_distance_to_pulse(uint16_t distance){
    return distance * position_absolute_factor;

}

static uint8_t PID_calculate(uint16_t error){
    static error_last = 0;
    static error_tatol = 0;
    
    error_tatol += error;

    uint16_t result_p = position_PID_p * error;
    uint16_t result_i = position_PID_i * error_tatol;
    uint16_t result_d = position_PID_d * (error - error_last);

    error_last = error;
    
    return result_p + result_i + result_d;
}
 
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     