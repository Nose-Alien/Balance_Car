/**
 ****************************************************************************************************
 * @file        atk_ms6050.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MS6050模块驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************

 ****************************************************************************************************
 */

#include "atk_ms6050.h"
//#include "atk_ms6050_iic.h"
#include "delay.h"
#include "stdio.h"
#include "inv_mpu.h"
#include "i2c.h"

/**
 * @brief       往ATK-MS6050的指定寄存器连续写入指定数据
 * @param       addr: ATK-MS6050的IIC通讯地址
 *              reg : ATK-MS6050寄存器地址
 *              len : 写入的长度
 *              dat : 写入的数据
 * @retval      ATK_MS6050_EOK : 函数执行成功
 *              ATK_MS6050_EACK: IIC通讯ACK错误，函数执行失败
 */
uint8_t atk_ms6050_write(uint8_t addr,uint8_t reg, uint8_t len, uint8_t *dat)
{
    HAL_StatusTypeDef status;
    uint8_t buffer[len + 1];

    buffer[0] = reg;  // 第一个字节是寄存器地址
    for (uint8_t i = 0; i < len; i++) {
        buffer[i + 1] = dat[i];  // 将数据复制到缓冲区
    }

    // 使用HAL库的I2C写函数进行数据传输
    status = HAL_I2C_Master_Transmit(&hi2c1, addr << 1, buffer, len + 1, HAL_MAX_DELAY);

    if (status == HAL_OK) {
        return ATK_MS6050_EOK;
    } else {
        return ATK_MS6050_EACK;
    }

//    uint8_t i;
//
//    atk_ms6050_iic_start();
//    atk_ms6050_iic_send_byte((addr << 1) | 0);
//    if (atk_ms6050_iic_wait_ack() == 1)
//    {
//        atk_ms6050_iic_stop();
//        return ATK_MS6050_EACK;
//    }
//    atk_ms6050_iic_send_byte(reg);
//    if (atk_ms6050_iic_wait_ack() == 1)
//    {
//        atk_ms6050_iic_stop();
//        return ATK_MS6050_EACK;
//    }
//    for (i=0; i<len; i++)
//    {
//        atk_ms6050_iic_send_byte(dat[i]);
//        if (atk_ms6050_iic_wait_ack() == 1)
//        {
//            atk_ms6050_iic_stop();
//            return ATK_MS6050_EACK;
//        }
//    }
//    atk_ms6050_iic_stop();
//
//    return ATK_MS6050_EOK;
}

/**
 * @brief       往ATK-MS6050的指定寄存器写入一字节数据
 * @param       addr: ATK-MS6050的IIC通讯地址
 *              reg : ATK-MS6050寄存器地址
 *              dat : 写入的数据
 * @retval      ATK_MS6050_EOK : 函数执行成功
 *              ATK_MS6050_EACK: IIC通讯ACK错误，函数执行失败
 */
uint8_t atk_ms6050_write_byte(uint8_t addr, uint8_t reg, uint8_t dat)
{
    return atk_ms6050_write(addr, reg, 1, &dat);
}

/**
 * @brief       连续读取ATK-MS6050指定寄存器的值
 * @param       addr: ATK-MS6050的IIC通讯地址
 *              reg : ATK-MS6050寄存器地址
 *              len: 读取的长度
 *              dat: 存放读取到的数据的地址
 * @retval      ATK_MS6050_EOK : 函数执行成功
 *              ATK_MS6050_EACK: IIC通讯ACK错误，函数执行失败
 */
uint8_t atk_ms6050_read(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *dat)
{

    HAL_StatusTypeDef status;

    // 发送寄存器地址
    status = HAL_I2C_Master_Transmit(&hi2c1, addr << 1, &reg, 1, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        return ATK_MS6050_EACK;
    }

    // 读取数据
    status = HAL_I2C_Master_Receive(&hi2c1, addr << 1, dat, len, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        return ATK_MS6050_EACK;
    }

    return ATK_MS6050_EOK;

//    atk_ms6050_iic_start();
//    atk_ms6050_iic_send_byte((addr << 1) | 0);
//    if (atk_ms6050_iic_wait_ack() == 1)
//    {
//        atk_ms6050_iic_stop();
//        return ATK_MS6050_EACK;
//    }
//    atk_ms6050_iic_send_byte(reg);
//    if (atk_ms6050_iic_wait_ack() == 1)
//    {
//        atk_ms6050_iic_stop();
//        return ATK_MS6050_EACK;
//    }
//    atk_ms6050_iic_start();
//    atk_ms6050_iic_send_byte((addr << 1) | 1);
//    if (atk_ms6050_iic_wait_ack() == 1)
//    {
//        atk_ms6050_iic_stop();
//        return ATK_MS6050_EACK;
//    }
//    while (len)
//    {
//        *dat = atk_ms6050_iic_read_byte((len > 1) ? 1 : 0);
//        len--;
//        dat++;
//    }
//    atk_ms6050_iic_stop();
//    return ATK_MS6050_EOK;
}

/**
 * @brief       读取ATK-MS6050指定寄存器的值
 * @param       addr: ATK-MS6050的IIC通讯地址
 *              reg : ATK-MS6050寄存器地址
 *              dat: 读取到的寄存器的值
 * @retval      ATK_MS6050_EOK : 函数执行成功
 *              ATK_MS6050_EACK: IIC通讯ACK错误，函数执行失败
 */
uint8_t atk_ms6050_read_byte(uint8_t addr, uint8_t reg, uint8_t *dat)
{
    return atk_ms6050_read(addr, reg, 1, dat);
}

/**
 * @brief       ATK-MS6050软件复位
 * @param       无
 * @retval      无
 */
void atk_ms6050_sw_reset(void)
{
    atk_ms6050_write_byte(ATK_MS6050_IIC_ADDR, MPU_PWR_MGMT1_REG, 0x80);
    delay_ms(100);
    atk_ms6050_write_byte(ATK_MS6050_IIC_ADDR, MPU_PWR_MGMT1_REG, 0x00);
}

/**
 * @brief       ATK-MS6050设置陀螺仪传感器量程范围
 * @param       frs: 0 --> ±250dps
 *                   1 --> ±500dps
 *                   2 --> ±1000dps
 *                   3 --> ±2000dps
 * @retval      ATK_MS6050_EOK : 函数执行成功
 *              ATK_MS6050_EACK: IIC通讯ACK错误，函数执行失败
 */
uint8_t atk_ms6050_set_gyro_fsr(uint8_t fsr)
{
    return atk_ms6050_write_byte(ATK_MS6050_IIC_ADDR, MPU_GYRO_CFG_REG, fsr << 3);
}

/**
 * @brief       ATK-MS6050设置加速度传感器量程范围
 * @param       frs: 0 --> ±2g
 *                   1 --> ±4g
 *                   2 --> ±8g
 *                   3 --> ±16g
 * @retval      ATK_MS6050_EOK : 函数执行成功
 *              ATK_MS6050_EACK: IIC通讯ACK错误，函数执行失败
 */
uint8_t atk_ms6050_set_accel_fsr(uint8_t fsr)
{
    return atk_ms6050_write_byte(ATK_MS6050_IIC_ADDR, MPU_ACCEL_CFG_REG, fsr << 3);
}

/**
 * @brief       ATK-MS6050设置数字低通滤波器频率
 * @param       lpf: 数字低通滤波器的频率（Hz）
 * @retval      ATK_MS6050_EOK : 函数执行成功
 *              ATK_MS6050_EACK: IIC通讯ACK错误，函数执行失败
 */
uint8_t atk_ms6050_set_lpf(uint16_t lpf)
{
    uint8_t dat;

    if (lpf >= 188)
    {
        dat = 1;
    }
    else if (lpf >= 98)
    {
        dat = 2;
    }
    else if (lpf >= 42)
    {
        dat = 3;
    }
    else if (lpf >= 20)
    {
        dat = 4;
    }
    else if (lpf >= 10)
    {
        dat = 5;
    }
    else
    {
        dat = 6;
    }

    return atk_ms6050_write_byte(ATK_MS6050_IIC_ADDR, MPU_CFG_REG, dat);
}

/**
 * @brief       ATK-MS6050设置采样率
 * @param       rate: 采样率（4~1000Hz）
 * @retval      ATK_MS6050_EOK : 函数执行成功
 *              ATK_MS6050_EACK: IIC通讯ACK错误，函数执行失败
 */
uint8_t atk_ms6050_set_rate(uint16_t rate)
{
    uint8_t ret;
    uint8_t dat;

    if (rate > 1000)
    {
        rate = 1000;
    }

    if (rate < 4)
    {
        rate = 4;
    }

    dat = 1000 / rate - 1;
    ret = atk_ms6050_write_byte(ATK_MS6050_IIC_ADDR, MPU_SAMPLE_RATE_REG, dat);
    if (ret != ATK_MS6050_EOK)
    {
        return ret;
    }

    ret = atk_ms6050_set_lpf(rate >> 1);
    if (ret != ATK_MS6050_EOK)
    {
        return ret;
    }

    return ATK_MS6050_EOK;
}

/**
 * @brief       ATK-MS6050获取温度值
 * @param       temperature: 获取到的温度值（扩大了100倍）
 * @retval      ATK_MS6050_EOK : 函数执行成功
 *              ATK_MS6050_EACK: IIC通讯ACK错误，函数执行失败
 */
uint8_t atk_ms6050_get_temperature(int16_t *temp)
{
    uint8_t dat[2];
    uint8_t ret;
    int16_t raw = 0;

    ret = atk_ms6050_read(ATK_MS6050_IIC_ADDR, MPU_TEMP_OUTH_REG, 2, dat);
    if (ret == ATK_MS6050_EOK)
    {
        raw = ((uint16_t)dat[0] << 8) | dat[1];
        *temp = (int16_t)((36.53f + ((float)raw / 340)) * 100);
    }

    return ret;
}

/**
 * @brief       ATK-MS6050获取陀螺仪值
 * @param       gx，gy，gz: 陀螺仪x、y、z轴的原始度数（带符号）
 * @retval      ATK_MS6050_EOK : 函数执行成功
 *              ATK_MS6050_EACK: IIC通讯ACK错误，函数执行失败
 */
uint8_t atk_ms6050_get_gyroscope(int16_t *gx, int16_t *gy, int16_t *gz)
{
    uint8_t dat[6];
    uint8_t ret;

    ret =  atk_ms6050_read(ATK_MS6050_IIC_ADDR, MPU_GYRO_XOUTH_REG, 6, dat);
    if (ret == ATK_MS6050_EOK)
    {
        *gx = ((uint16_t)dat[0] << 8) | dat[1];
        *gy = ((uint16_t)dat[2] << 8) | dat[3];
        *gz = ((uint16_t)dat[4] << 8) | dat[5];
    }

    return ret;
}

/**
 * @brief       ATK-MS6050获取加速度值
 * @param       ax，ay，az: 加速度x、y、z轴的原始度数（带符号）
 * @retval      ATK_MS6050_EOK : 函数执行成功
 *              ATK_MS6050_EACK: IIC通讯ACK错误，函数执行失败
 */
uint8_t atk_ms6050_get_accelerometer(int16_t *ax, int16_t *ay, int16_t *az)
{
    uint8_t dat[6];
    uint8_t ret;

    ret =  atk_ms6050_read(ATK_MS6050_IIC_ADDR, MPU_ACCEL_XOUTH_REG, 6, dat);
    if (ret == ATK_MS6050_EOK)
    {
        *ax = ((uint16_t)dat[0] << 8) | dat[1];
        *ay = ((uint16_t)dat[2] << 8) | dat[3];
        *az = ((uint16_t)dat[4] << 8) | dat[5];
    }

    return ret;
}

/**
 * @brief       ATK-MS6050初始化
 * @param       无
 * @retval      ATK_MS6050_EOK: 函数执行成功
 *              ATK_MS6050_EID: 获取ID错误，函数执行失败
 */
uint8_t atk_ms6050_init(void)
{
    uint8_t id;
//    atk_ms6050_iic_init();                                                  /* 初始化IIC接口 */
    atk_ms6050_sw_reset();                                                  /* ATK-MS050软件复位 */
    atk_ms6050_set_gyro_fsr(3);                                             /* 陀螺仪传感器，±2000dps */
    atk_ms6050_set_accel_fsr(0);                                            /* 加速度传感器，±2g */
    atk_ms6050_set_rate(200);                                                /* 采样率，200Hz */
    atk_ms6050_write_byte(ATK_MS6050_IIC_ADDR, MPU_INT_EN_REG, 0X00);       /* 关闭所有中断 */
    atk_ms6050_write_byte(ATK_MS6050_IIC_ADDR, MPU_USER_CTRL_REG, 0X00);    /* 关闭IIC主模式 */
    atk_ms6050_write_byte(ATK_MS6050_IIC_ADDR, MPU_FIFO_EN_REG, 0X00);      /* 关闭FIFO */
    atk_ms6050_write_byte(ATK_MS6050_IIC_ADDR, MPU_INTBP_CFG_REG, 0X80);    /* INT引脚低电平有效 */
    atk_ms6050_read_byte(ATK_MS6050_IIC_ADDR, MPU_DEVICE_ID_REG, &id);      /* 读取设备ID */
    if (id != ATK_MS6050_IIC_ADDR)
    {
        return ATK_MS6050_EID;
    }

    atk_ms6050_write_byte(ATK_MS6050_IIC_ADDR, MPU_PWR_MGMT1_REG, 0x01);    /* 设置CLKSEL，PLL X轴为参考 */
    atk_ms6050_write_byte(ATK_MS6050_IIC_ADDR, MPU_PWR_MGMT2_REG, 0x00);    /* 加速度与陀螺仪都工作 */
    atk_ms6050_set_rate(200);                                                /* 采样率，200Hz */

    return ATK_MS6050_EOK;
}


void my_mpu6050_init(void )
{
//    printf("my_init running !\r\n");
    uint8_t ret;
    delay_init(72);
    /* 初始化ATK-MS6050 */
    ret = atk_ms6050_init();
    if (ret != 0)
    {
//        printf("ATK-MS6050 init failed!\r\n");
        while (atk_ms6050_init())
        {
//            printf("ATK-MS6050 init failed!\r\n");
            delay_ms(1000);
        }
    }
//    else  printf("ATK-MS6050 init successed! \r\n");

    /* 初始化ATK-MS6050 DMP */
    ret = atk_ms6050_dmp_init();
    if (ret != 0)
    {
//        printf("ATK-MS6050 DMP init failed!\r\n");
        while (atk_ms6050_dmp_init())
        {
//            printf("ATK-MS6050 DMP init failed!\r\n");
            delay_ms(1000);
        }
    }
//    else printf("ATK-MS6050 DMP init successed! \r\n");
}