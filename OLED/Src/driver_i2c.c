
#include "driver_i2c.h"
#include "delay.h"


/**
 * @brief  使用软件方式延时(微 秒)
 * @param  t_microseconds
 */
void I2C_Delay(uint32_t t_microseconds)
{
    delay_us(t_microseconds);
}


/**
 * @brief  将模拟I2C的SCL和SDA引脚进行重新初始化
 */
void I2C_GPIO_ReInit(void)
{
    /* 1. 使用结构体定义硬件GPIO对象 */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 2. 将SCL和SDA的GPIO寄存器的值恢复为复位上电值 */
    HAL_GPIO_DeInit(SCL_PORT, SCL_PIN);
    HAL_GPIO_DeInit(SDA_PORT, SDA_PIN);
    
    /* 3. 使能SCL和SDA的GPIO的时钟，因为他们都是GPIOF组的，所以这里只使能GPIOF的时钟 */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    /* 4. 设置GPIO的模式为开漏输出模式，响应速度设置为快速响应 */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        
    /* 5. 选择要设置的GPIO引脚 */
    GPIO_InitStruct.Pin = SCL_PIN;
    
    /* 6. 调用init函数初始化GPIO */
    HAL_GPIO_Init(SCL_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = SDA_PIN;
    HAL_GPIO_Init(SDA_PORT, &GPIO_InitStruct);
}


/**
 * @brief  模拟发出I2C的起始信号-->在SCL的高电平下，SDA的电平由高到低变化
 */
void I2C_Start(void)
{
    SCL_H;
    SDA_H;
    I2C_Delay(5);
    SDA_L;
    I2C_Delay(5);
}


/**
 * @brief  模拟发出I2C的停止信号-->在SCL的高电平下，SDA的电平由低到高变化
 */
void I2C_Stop(void)
{
    SDA_L;
    SCL_H;
    I2C_Delay(2);
    SDA_H;
    I2C_Delay(2);
}

/**
 * @brief  模拟I2C发出一个应答信号，在第九个时钟将SDA拉低
 */
void I2C_ACK(void)
{
    SCL_L;
    SDA_L;
    I2C_Delay(2);
    SCL_H;
    I2C_Delay(5);
}

/**
 * @brief  模拟I2C发出一个非应答信号，在第九个时钟将SDA拉高
 */
void I2C_NACK(void)
{
    SCL_L;
    SDA_H;
    I2C_Delay(2);
    SCL_H;
    I2C_Delay(5);
}

/*
 *  函数名：I2C_GetAck
 *  功能描述：模拟I2C获取从设备的响应信号-->在SCL的高电平下，从设备将SDA拉低
 *  输入参数：无
 *  输出参数：无
 *  返回值：成功得到响应返回0，否则返回-1
*/

uint16_t I2C_GetAck(void)
{
    int i = 0;
    SCL_L;
    SDA_H;
    I2C_Delay(5);
    SCL_H;
    while(SDA_IN != 0)
    {
        i++;
        if(i == 10)
        {
            SCL_L;
            return -1;
        }
        I2C_Delay(10);
    }
    SCL_L;
    return 0;
}

/**
* @brief  模拟I2C发出一个字节的数据
* @param  data 要发送出去的数据，范围0~255
*/
void I2C_WriteByte(uint8_t data)
{
    uint8_t i = 0;
    for(i=0; i<8; i++)
    {
        SCL_L;
        I2C_Delay(2);
        if(data & 0x80) {
            SDA_H;
        }
        else {
            SDA_L;
        }
        data <<= 1; // 发出1bit数据后，要更新数据，将data的次高位移位到最高位
        SCL_H;
        I2C_Delay(2);
    }
    I2C_GetAck();
}


/**
* @brief  模拟I2C读取一个字节的数据
* @param  ack 根据此参数判断在读到一个字节之后是否发出应答信号
*/
uint8_t I2C_ReadByte(uint8_t ack)
{
    uint8_t i = 0;
    uint8_t data = 0;
    
    SDA_H;
    for(i=0; i<8; i++)
    {
        SCL_L;
        I2C_Delay(2);
        SCL_H;
        I2C_Delay(2);
        data <<= 1;  // 更新数据前，要将上一次数据左移1位用来保存接下来的这一位数据

        if(SDA_IN == 1) {
            data++;
        }
        else {
            data = data;
        }
    }
    
    // 根据ack决定是否发出应答
    if(ack == 0) {
        I2C_ACK();
    }
    else if(ack == 1) {
        I2C_NACK();
    }
    return data;
}

