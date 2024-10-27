#ifndef __POSITION_H
#define __POSITION_H
#include "stm32f10x.h"


#define NULL 0

/// @brief 方向极性枚举
typedef enum {
    positive = 0,
    negative = 1,
}polarity_t;

/// @brief position接口结构体
typedef struct{
    /// @brief 在move_函数中，可以使用polarity控制正反转的极性，以与坐标系方向统一
    uint8_t (*pfmove_x) (uint8_t dircetion, uint16_t speed, float distance);
    uint8_t (*pfmove_y) (uint8_t dircetion, uint16_t speed, float distance);
    uint8_t (*pfmove_z) (uint8_t dircetion, uint16_t speed, float distance);
    uint8_t (*pfget_flag_move) (void);
    uint8_t (*pfget_flag_set0) (void);
}position_interface_t;


/// @brief position信息结构体
typedef struct{
    float position_x;
    float position_y;
    float position_z;

    float position_x_max;
    float position_x_min;
    float position_y_max;
    float position_y_min;
    float position_z_max;
    float position_z_min;
}position_inf_t;


/// @brief position配置结构体
typedef struct 
{
    polarity_t polarity_x;
    polarity_t polarity_y;
    polarity_t polarity_z;
}position_conf_t;

/// @brief position结构体
typedef struct position_t{
    position_interface_t position_interface;
    position_inf_t position_inf;
    position_conf_t position_conf;

    uint8_t (*pfmove_to_xy) (struct position_t *position, float position_x_target, float position_y_target, uint16_t speed);
    uint8_t (*pfmove_to_z) (struct position_t *position,float position_z_target, uint16_t speed);
    uint8_t (*pfget_position) (struct position_t *position, float *pposition_x, float *pposition_y, float *pposition_z); 
    uint8_t (*pfmove_x) (struct position_t *position, uint8_t direction, uint16_t speed, float distance);
    uint8_t (*pfmove_y) (struct position_t *position, uint8_t direction, uint16_t speed, float distance);
    uint8_t (*pfmove_z) (struct position_t *position, uint8_t direction, uint16_t speed, float distance);
    uint8_t (*pfget_flag_move) (struct position_t *position);
    uint8_t (*pfget_flag_set0) (struct position_t *position);
}position_t;

/// @brief 结构体初始化
/// @param position 初始化的结构体
/// @param position_interface 接口函数结构体
/// @param position_conf 配置结构体
/// @param position_x_max x轴的最大值
/// @param position_x_min x轴的最小值
/// @param position_y_max y轴的最大值
/// @param position_y_min y轴的最小值
/// @param position_z_max z轴的最大值
/// @param position_z_min z轴的最小值
/// @return 
uint8_t position_initialize(position_t *position, position_interface_t *position_interface, position_conf_t *position_conf,
float position_x_max, float position_x_min, float position_y_max, float position_y_min, float position_z_max, float position_z_min);

#endif
