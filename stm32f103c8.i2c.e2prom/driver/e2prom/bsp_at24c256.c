#include "bsp_at24c256.h"
#include "bsp_systick.h"
#include "bsp_usart.h"
#include "bsp_i2c.h"

/**
 * @brief 在指定的地址写一个字节
 * @param address 指定要写入的地址
 * @param buffer 要写入数据的数据缓存区
 * @return int 返回程序执行的状态
 */
int at24c256_byte_write(u16 address, u8* buffer)
{
        u8 address_high = (u8)((address >> 8) & 0x00ff);
        u8 address_low = (u8)(address & 0x00ff);
        
        printf("[%s][%s][%d]: address_high = %02x\r\n", __FILE__, __FUNCTION__, __LINE__, address_high);
        printf("[%s][%s][%d]: address_low = %02x\r\n", __FILE__, __FUNCTION__, __LINE__, address_low);

        i2c_start(&i2c_bus_1);

        i2c_send_byte(&i2c_bus_1, AT24C256_WRITE);
        if(i2c_wait_response(&i2c_bus_1))
        {
                printf("[%s][%s][%d]: device address mapping fail\r\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
        }

        i2c_send_byte(&i2c_bus_1, address_high);
        if(i2c_wait_response(&i2c_bus_1))
        {
                printf("[%s][%s][%d]: write address fail\r\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
        }
        i2c_send_byte(&i2c_bus_1, address_low);
        if(i2c_wait_response(&i2c_bus_1))
        {
                printf("[%s][%s][%d]: write address fail\r\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
        }

        i2c_send_byte(&i2c_bus_1, *buffer);
        if(i2c_wait_response(&i2c_bus_1))
        {
                printf("[%s][%s][%d]: write data fail\r\n", __FILE__, __FUNCTION__, __LINE__);
                i2c_stop(&i2c_bus_1);
                return -1;
        }

        i2c_stop(&i2c_bus_1);

        return 0;
}

/**
 * @brief 在指定的地址连续写入字节
 * @param address 指定要写入的首地址
 * @param buffer 要写入数据的数据缓存区
 * @param length 指定要写入的字节个数
 * @return int 返回程序执行的状态
 * @note 此函数为页写入函数, 对于 at24c256 一页为 64 个字节
 */
int at24c256_page_write(u16 address, u8* buffer, u8 length)
{
        u8 address_high = (u8)((address >> 8) & 0x00ff);
        u8 address_low = (u8)(address & 0x00ff);

        i2c_start(&i2c_bus_1);

        if((length <= 0) || (length > 64))
        {
                printf("[%s][%s][%d]: length parameter values are not specification\r\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
        }

        i2c_send_byte(&i2c_bus_1, AT24C256_WRITE);
        if(i2c_wait_response(&i2c_bus_1))
        {
                printf("[%s][%s][%d]: device address mapping fail\r\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
        }

        i2c_send_byte(&i2c_bus_1, address_high);
        if(i2c_wait_response(&i2c_bus_1))
        {
                printf("[%s][%s][%d]: write address fail\r\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
        }
        i2c_send_byte(&i2c_bus_1, address_low);
        if(i2c_wait_response(&i2c_bus_1))
        {
                printf("[%s][%s][%d]: write address fail\r\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
        }

        for(int i = 0; i < length; i++)
        {
                i2c_send_byte(&i2c_bus_1, buffer[i]);
                if(i2c_wait_response(&i2c_bus_1))
                {
                        printf("[%s][%s][%d]: write data error at %d\r\n", __FILE__, __FUNCTION__, __LINE__, i);
                        return -1;
                }
        }

        i2c_stop(&i2c_bus_1);

        return 0;
}

/**
 * @brief 直接读取当前地址的内容
 * @param buffer 保存读取到的数据的缓存区
 * @return int 返回程序执行的状态
 */
int at24c256_current_address_read(u8* buffer)
{
        i2c_start(&i2c_bus_1);

        i2c_send_byte(&i2c_bus_1, AT24C256_READ);
        if(i2c_wait_response(&i2c_bus_1))
        {
                printf("[%s][%s][%d]: device address mapping fail\r\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
        }

        buffer[0] = i2c_receive_byte(&i2c_bus_1);

        i2c_no_response(&i2c_bus_1);

        i2c_stop(&i2c_bus_1);

        return 0;
}

/**
 * @brief 在指定的地址读取一片连续的字节
 * @param address 指定要读取的首地址
 * @param buffer 保存读取到的数据的缓存区
 * @return int 返回程序执行的状态
 */
int at24c02_random_read(u16 address, u8* buffer)
{
        u8 address_high = (u8)((address >> 8) & 0x00ff);
        u8 address_low = (u8)(address & 0x00ff);

        i2c_start(&i2c_bus_1);

        i2c_send_byte(&i2c_bus_1, AT24C256_WRITE);
        if(i2c_wait_response(&i2c_bus_1))
        {
                printf("[%s][%s][%d]: device address mapping fail\r\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
        }
        
        i2c_send_byte(&i2c_bus_1, address_high);
        if(i2c_wait_response(&i2c_bus_1))
        {
                printf("[%s][%s][%d]: write address fail\r\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
        }
        i2c_send_byte(&i2c_bus_1, address_low);
        if(i2c_wait_response(&i2c_bus_1))
        {
                printf("[%s][%s][%d]: write address fail\r\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
        }
        
        return at24c256_current_address_read(buffer);
}
