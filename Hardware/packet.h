#ifndef __PACKET_H
#define __PACKER_H
#include "stm32f10x.h"

/// @brief 接受状态结构体
typedef enum{
    search_head,
	search_id,
    wait_id,
    wait_length,
    wait_order,
    wait_end,
} RecBufState_t; 

/// @brief 映射表单元
typedef struct 
{
    uint32_t order;
    uint8_t length_data;
}map_table_member_t;

/// @brief 映射表结构体
typedef struct 
{
    map_table_member_t *map_table_member_t;
    uint8_t length_table;
}map_table_t;
 


/// @brief 通讯类型各段长度即数据类型
/// none:通讯协议无此段
/// uint8:8位，1字节
/// uint16:16位，2字节
/// uint32:32位，4字节
typedef enum 
{   none = 0,
    uint8 = 1,
    uint16 = 2,
    uint32 = 4,
}type_t;

/// @brief 校验位类型
/// none:无校验位
/// Xor:逐字节异或
/// constant:固定8位数据
typedef enum 
{   non = 0,   
    Xor = 2,
    constant = 3,
}mode_cheak_t;

typedef struct 
{
    uint8_t *padd_buffer;
    uint8_t size;
}decoded_buffer_t;


/// @brief 缓冲区配置
typedef struct{
    uint8_t *padd_buffer;
    uint8_t read_index;
    uint8_t write_index;
    uint8_t size;
   
}recevie_buffer_t;

/// @brief 通讯协议配置
typedef struct 
{
    type_t type_head; 
    type_t type_addr; 
    type_t type_length_data;
    type_t type_order;
    mode_cheak_t mode_cheak;
    uint8_t num_constant_cheak;
}config_t;

/// @brief packet接口
typedef struct{
    uint8_t (*pfinit) (void);
    uint8_t (*pfdeinit) (void);
    uint8_t (*pfsendbyte) (uint8_t byte);
}intereface_t;

/// @brief packet接口及方法
typedef struct{
    intereface_t intereface;
    recevie_buffer_t recevie_buffer;
    config_t config;
    decoded_buffer_t decoded_buffer;
    map_table_t map_table;

    uint8_t flag_initalize;//初始化标志位

    uint8_t (*pfinit) (void);

    uint8_t (*pfdeinit) (void);

    uint8_t (*pfread_rec_buffer) (uint32_t head, uint8_t head_control, uint32_t addr, uint8_t addr_control);

    uint8_t (*pfsend) (uint32_t head, uint32_t addr, uint32_t length_data, uint32_t order, uint8_t *data);

     
}packet_t;

/// @brief 中断服务函数的callback函数，参数与对应的packet结构体初始化传入的一致
/// @param precevie_buffer 
/// @return 
uint8_t interrupt_callback (packet_t * const packet, uint8_t (*pfdriver_rec_data) (void));



/// @brief 对packet_t类型的数据进行初始化，进行函数的挂载实例化，和通信协议的配置
/// @param packet 初始化对象的地址
/// @param config 通讯协议配置结构体的地址
/// @param map_table 通讯协议配置结构体的地址，当通讯协议中没有数据长度的段时传入映射表
/// @param pfinit_interface 硬件初始化接口
/// @param pfdeinit_interface 硬件去初始化接口
/// @param pfsendbyte_interface 发送一字节数据接口
/// @param pinterrupt_callback 在中断服务函数中传入的全局packet_t结构体指针,指向此结构体
/// @param recbuf_size 设置的缓冲区大小，应该大于协议中的最长数据长度
/// @param decodebuf_size 设置的解码区大小，应该大于协议中的最长数据长度
/// @return 
uint8_t packet_initalize (packet_t *packet, config_t *config, map_table_t *map_table,
        uint8_t (*pfinit_interface) (void), uint8_t (*pfdeinit_interface) (void), uint8_t (*pfsendbyte_interface) (void),
        packet_t *pinterrupt_callback, uint8_t recbuf_size, uint8_t decodebuf_size);

#endif