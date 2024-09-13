#include "packet.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h> 


#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({              \         
const typeof( ((type *)0)->member ) *__mptr = (ptr);    \         
(type *)( (char *)__mptr - offsetof(type,member) );})
 

/// @brief 中断服务函数的callback函数，参数与对应的packet结构体初始化传入的一致,传入接收函数
/// @param precevie_buffer 
/// @return 
static uint8_t interrupt_callback(packet_t * const packet,uint8_t (*pfdriver_rec_data) (void))
{
    assert_param(packet->flag_initalize == 1);

    packet->recevie_buffer.padd_buffer[packet->recevie_buffer.write_index] = (uint8_t) pfdriver_rec_data ();
    packet->recevie_buffer.write_index = (packet->recevie_buffer.write_index + 1) % packet->recevie_buffer.size;
    
    return 0;
}


/// @brief 对packet_t类型的数据进行初始化，进行函数的挂载实例化，和通信协议的配置
/// @param packet 初始化对象的地址
/// @param config 通讯协议配置结构体的地址，当通讯协议中没有数据长度的段时传入映射表
/// @param map_table 映射表结构体的地址
/// @param pfinit_interface 硬件初始化接口
/// @param pfdeinit_interface 硬件去初始化接口
/// @param pfsendbyte_interface 发送一字节数据接口
/// @param pinterrupt_callback 在中断服务函数中传入的全局packet_t结构体指针,指向此结构体
/// @param recbuf_size 设置的缓冲区大小，应该大于协议中的最长数据长度
/// @param decodebuf_size 设置的解码区大小，应该大于协议中的最长数据长度
/// @return 
static uint8_t packet_initalize (packet_t *packet, config_t *config, map_table_t *map_table,
        uint8_t (*pfinit_interface) (void), uint8_t (*pfdeinit_interface) (void), uint8_t (*pfsendbyte_interface) (void),
        packet_t *pinterrupt_callback,uint8_t recbuf_size,uint8_t decodebuf_size)
{   
    assert_param(config->type_head | config->type_addr == 1);
    assert_param(config->type_length_data | config->type_order == 1);
    packet->config.type_head = config->type_head;
    packet->config.type_addr = config->type_addr;
    packet->config.type_length_data = config->type_length_data;
    packet->config.type_order = config->type_order;
    packet->config.mode_cheak = config->mode_cheak;
    packet->config.num_constant_cheak = config->num_constant_cheak;

    packet->map_table.map_table_member_t = map_table->map_table_member_t;
    packet->map_table.length_table = map_table->length_table;

    packet->intereface.pfinit = pfinit_interface;
    packet->intereface.pfdeinit = pfdeinit_interface;
    packet->intereface.pfsendbyte = pfsendbyte_interface;

    if(packet->intereface.pfinit == NULL|| packet->intereface.pfdeinit == NULL|| packet->intereface.pfsendbyte== NULL ){
        return 1;
    }
    
    packet->recevie_buffer.size = recbuf_size;
    packet->recevie_buffer.padd_buffer = (uint8_t *)malloc(recbuf_size);
    packet->recevie_buffer.read_index = 0;
    packet->recevie_buffer.write_index = 0;

    packet->decoded_buffer.size =decodebuf_size;
    packet->decoded_buffer.padd_buffer = (uint8_t *)malloc(decodebuf_size);

    packet->pfinit = init;          

    packet->pfdeinit = deinit;

    packet->pfread_rec_buffer = read_rec_buffer;

    packet->pfsend = send; 

    packet->flag_initalize = 1;
    pinterrupt_callback = packet;
    return 0;
}

    /// @brief 硬件初始化函数
    /// @param  
    /// @return 0为正常
static uint8_t init (void){
        packet_t *packet = (packet_t *)container_of(&init,packet_t,pfinit);
        packet->intereface.pfinit();
        return 0;
    }

    /// @brief 硬件去初始化函数
    /// @param  
    /// @return 
static uint8_t deinit (void){
        packet_t *packet = (packet_t *)container_of(&deinit,packet_t,pfdeinit);
       
        packet->intereface.pfdeinit();
        return 0;
    }



    /// @brief 除了length_data，传入数据段长度，按照通信协议的字节长度、以及参数的数据打包数据发送，在initaliize中选择none的段在函数中不生效，可填任意值
    /// @param head head数据
    /// @param addr addr数据
    /// @param length_data length数据 
    /// @param data data数据地址
    /// @return 
static uint8_t send (uint32_t head, uint32_t addr, uint32_t length_data, uint32_t order, uint8_t *data){
        packet_t *packet = (packet_t *)container_of(&send,packet_t,pfsend);

        uint16_t length_package =  (packet->config.type_head) + (packet->config.type_addr) + (packet->config.type_length_data) + (packet->config.type_order) + length_data + ((packet->config.mode_cheak)/2);

        uint8_t *psend_buffer = (uint8_t *)malloc(length_package);

        const uint8_t *psend_buffer_start = psend_buffer;

        switch(packet->config.type_head){
            case none:
                break;
            case uint8:
                *(psend_buffer++) = (uint8_t)head;
                break;
            case uint16:
                *(psend_buffer++) = ((uint8_t) (head & 0xFF));
                *(psend_buffer++) = ((uint8_t) ((head >> 8) & 0xFF));
                break;
            case uint32:
                *(psend_buffer++) = ((uint8_t) (head & 0xFF));
                *(psend_buffer++) = ((uint8_t) ((head >> 8) & 0xFF));
                *(psend_buffer++) = ((uint8_t) ((head >> 16) & 0xFF));
                break;
        }

        switch(packet->config.type_addr){
            case none:
                break;
            case uint8:
                *(psend_buffer++) = (uint8_t)addr;
                break;
            case uint16:
                *(psend_buffer++) = ((uint8_t) (addr & 0xFF));
                *(psend_buffer++) = ((uint8_t) ((addr >> 8) & 0xFF));
                break;
            case uint32:
                *(psend_buffer++) = ((uint8_t) (addr & 0xFF));
                *(psend_buffer++) = ((uint8_t) ((addr >> 8) & 0xFF));
                *(psend_buffer++) = ((uint8_t) ((addr >> 16) & 0xFF));
                break;
        }    

        switch(packet->config.type_length_data){
            case none:
                break;
            case uint8:
                *(psend_buffer++) = (uint8_t)length_data;
                break;
            case uint16:
                *(psend_buffer++) = ((uint8_t) (length_data & 0xFF));
                *(psend_buffer++) = ((uint8_t) ((length_data >> 8) & 0xFF));
                break;
            case uint32:
                *(psend_buffer++) = ((uint8_t) (length_data & 0xFF));
                *(psend_buffer++) = ((uint8_t) ((length_data >> 8) & 0xFF));
                *(psend_buffer++) = ((uint8_t) ((length_data >> 16) & 0xFF));
                break;
        } 

        switch(packet->config.type_order){
            case none:
                break;
            case uint8:
                *(psend_buffer++) = (uint8_t)order;
                break;
            case uint16:
                *(psend_buffer++) = ((uint8_t) (order & 0xFF));
                *(psend_buffer++) = ((uint8_t) ((order >> 8) & 0xFF));
                break;
            case uint32:
                *(psend_buffer++) = ((uint8_t) (order & 0xFF));
                *(psend_buffer++) = ((uint8_t) ((order >> 8) & 0xFF));
                *(psend_buffer++) = ((uint8_t) ((order >> 16) & 0xFF));
                break;
        }       

        memcpy(psend_buffer, data, length_data);
        psend_buffer += length_data;

        switch(packet->config.mode_cheak){
            case none:
                break;   
            case Xor:
                for (uint8_t i = 0; i < length_package; i++)
                {
                    *psend_buffer ^= psend_buffer_start[i]; 
                }
                break;
            case constant:
                *psend_buffer = packet->config.num_constant_cheak;
                break;
        }

        for (uint8_t i = 0; i < length_package; i++)
        {
            packet->intereface.pfsendbyte(psend_buffer_start[i]);
        }
        
        free(psend_buffer);
    }

/// @brief 获取缓冲区未读的byte数
/// @param packet 
/// @return 获取缓冲区未读的byte数
static uint8_t GetBufBitsNoRead(const packet_t *packet){
    return (int8_t)(((int16_t)(packet->recevie_buffer.write_index) - (int16_t)(packet->recevie_buffer.read_index)) + (int16_t)(packet->recevie_buffer.size)) % (int16_t)(packet->recevie_buffer.size);
}

    /// @brief 扫描读接受缓冲区函数，按照通信协议的字节长度、参数中的筛选控制位、以及参数的数据扫描接受缓冲区，如果通讯协议中没有length_data字段，通过查表确定
    ///        在initaliize中选择none的段在函数中不生效，控制位为关闭的不生效，可填任意值
    /// @param head 要扫描的头的数据
    /// @param head_control 是否筛选头，1为筛选，0为不筛选
    /// @param addr 地址ID
    /// @param addr_control 是否筛选地址ID，1为筛选，0为不筛选
    /// @return 0为成功接受到，数据拷贝到decoded_buffer
static uint8_t read_rec_buffer (uint32_t head, uint8_t head_control, uint32_t addr, uint8_t addr_control){
    packet_t *packet = (packet_t *)container_of(&read_rec_buffer,packet_t,pfread_rec_buffer);

    assert_param(!(addr_control == 1 && packet->config.type_addr == 0));// 防止通讯协议中没有此字段但是控制位为1
    assert_param(!(addr_control == 1 && packet->config.type_addr == 0));
    assert_param(head_control | addr_control == 1);

    static RecBufState_t RecBufState = search_head;
    switch(RecBufState){
        case search_head:
        {   if(packet->config.type_head != 0){//通讯协议有无帧头
                if(head_control == 1){      //通讯协议有帧头，有帧头控制
                    while(GetBufBitsNoRead(packet) >= packet->config.type_head){    //等待缓冲区未读位数超过或等于  
                        if(memcmp(packet->recevie_buffer.padd_buffer + (packet->recevie_buffer.read_index) - packet->config.type_head, &head, packet->config.type_head) == 0){//有，扫描帧头并比对
                            memcpy(packet->decoded_buffer.padd_buffer, &head, packet->config.type_head);                                                                    //扫描到帧头
                            packet->recevie_buffer.read_index = (packet->recevie_buffer.read_index + 1) % packet->recevie_buffer.size;
                            
                            if(addr_control == 1){
                                RecBufState = wait_id;
                            }
                            else if(packet->config.type_length_data != 0){
                            RecBufState = wait_length;
                            }
                            else{
                                RecBufState = wait_order;
                            }
                            read_rec_buffer (head, head_control, addr, addr_control); //继续下一状态
                            break;
                        }
                        else{
                            packet->recevie_buffer.read_index = (packet->recevie_buffer.read_index + 1) % packet->recevie_buffer.size;   //没扫描到帧头
                            break;
                        }     
               
                    }
                }   
                else{                           //通讯协议有帧头，无帧头控制
                    memcpy(packet->decoded_buffer.padd_buffer, &head, packet->config.type_head);
                    RecBufState = search_id;
                    read_rec_buffer (head, head_control, addr, addr_control); //继续下一状态
                    break;
                }
            }     
                
            else{       //通讯协议无帧头
                    RecBufState = search_id;
                    read_rec_buffer (head, head_control, addr, addr_control); //继续下一状态
                    break;
                }
            
        }
        case search_id:
        {
            while(GetBufBitsNoRead(packet) >= packet->config.type_addr){
                if(memcmp(packet->recevie_buffer.padd_buffer + (packet->recevie_buffer.read_index) - packet->config.type_addr, &addr, packet->config.type_addr) == 0){//扫描比对帧地址
                    memcpy(packet->decoded_buffer.padd_buffer + packet->config.type_head, &addr, packet->config.type_addr);//复制帧地址到解码区
                    packet->recevie_buffer.read_index = (packet->recevie_buffer.read_index + 1) % packet->recevie_buffer.size;
                    
                    if(packet->config.type_length_data != 0){           //帧地址正确，根据通讯协议是否有帧长度确定下一状态
                        RecBufState = wait_length;
                    }
                    else{
                        RecBufState = wait_order;
                    }
                    read_rec_buffer (head, head_control, addr, addr_control); //继续下一状态
                    break;
                }
                else{
                    packet->recevie_buffer.read_index = (packet->recevie_buffer.read_index + 1) % packet->recevie_buffer.size;  //没扫描到帧地址 
                    break;
                }
            }
        }
        case wait_id:
        {   
            if(packet->config.type_addr != 0){      //通讯协议是否有地址字段
                if (addr_control == 1){         //通讯协议有地址字段，帧地址控制为真
                    if(GetBufBitsNoRead(packet) >= packet->config.type_addr){    //等待缓冲区未读位数超过或等于  
                        if(memcmp(packet->recevie_buffer.padd_buffer + (packet->recevie_buffer.read_index) - packet->config.type_addr, &head, packet->config.type_addr) == 0){      //比对下一位帧地址
                            memcpy(packet->decoded_buffer.padd_buffer + packet->config.type_head, &addr, packet->config.type_addr);
                            packet->recevie_buffer.read_index = (packet->recevie_buffer.read_index + 1) % packet->recevie_buffer.size;

                            if(packet->config.type_length_data != 0){           //帧地址正确，根据通讯协议是否有帧长度确定下一状态
                                RecBufState = wait_length;
                            }
                            else{
                                RecBufState = wait_order;
                            }
                            read_rec_buffer (head, head_control, addr, addr_control); //继续下一状态
                            break;
                        }
                        else{                   //帧地址错误
                            
                            RecBufState = search_head;
                            packet->recevie_buffer.read_index = (packet->recevie_buffer.read_index + 1) % packet->recevie_buffer.size;
                            break;
                        }

                    }
                }
                else{                //通讯协议有地址字段，帧地址控制位为假
                    memcpy(packet->decoded_buffer.padd_buffer + packet->config.type_head, &addr, packet->config.type_addr);
                    if(packet->config.type_length_data != 0){           
                        RecBufState = wait_length;
                    }
                    else{
                        RecBufState = wait_order;
                    }
                    read_rec_buffer (head, head_control, addr, addr_control); //继续下一状态
                    break;
                }
            }
            else{               //通讯协议无地址字段
                if(packet->config.type_length_data != 0){           
                        RecBufState = wait_length;
                    }
                    else{
                        RecBufState = wait_order;
                    }
                    read_rec_buffer (head, head_control, addr, addr_control); //继续下一状态
                    break;
            }
        }

        case wait_length:
        {
            




        }


    }


    }