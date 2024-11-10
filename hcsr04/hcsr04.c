//
// Created by nose on 24-4-29.
//
#include "hcsr04.h"
#include "delay.h"
//uint16_t hcrs04_get_distance()
//{
//    uint16_t count;
//    uint16_t distance;
//    TRIG_PULL_1;
//    delay_us(20);
//    TRIG_PULL_0;
//    // 启动定时器
//    HAL_TIM_Base_Start(&HTIM);
//    // 等待 Echo 引脚变高电平
//    while (ECHO_ReadPin != GPIO_PIN_SET){}
//    // 重置定时器计数器
//            __HAL_TIM_SetCounter(&HTIM, 0);
//    // 等待 Echo 引脚变低电平
//    while (ECHO_ReadPin == GPIO_PIN_SET);
//
//    // 停止定时器
//    count = __HAL_TIM_GetCounter(&HTIM);
//    HAL_TIM_Base_Stop(&HTIM);
//
//    // 通过定时器计数器计算距离
//    distance = (uint16_t)(count * 0.017);
//
//    return distance;
//}

uint16_t hcrs04_get_distance()
{
    uint16_t count;
    uint16_t distance;
    uint32_t start_time;

    TRIG_PULL_1;
    delay_us(20);
    TRIG_PULL_0;

    // 启动定时器
    HAL_TIM_Base_Start(&HTIM);

    // 记录当前时间，开始计时
    start_time = HAL_GetTick();

    // 等待 Echo 引脚变高电平，带有超时检测
    while (ECHO_ReadPin != GPIO_PIN_SET) {
        if (HAL_GetTick() - start_time > 100) {  // 超时设为100ms，可根据需求调整
            HAL_TIM_Base_Stop(&HTIM);
            return 0;  // 返回0表示超时或错误
        }
    }

    // 重置定时器计数器
            __HAL_TIM_SetCounter(&HTIM, 0);

    // 记录时间以开始新的超时计时
    start_time = HAL_GetTick();

    // 等待 Echo 引脚变低电平，带有超时检测
    while (ECHO_ReadPin == GPIO_PIN_SET) {
        if (HAL_GetTick() - start_time > 100) {  // 超时设为100ms，可根据需求调整
            HAL_TIM_Base_Stop(&HTIM);
            return 0;  // 返回0表示超时或错误
        }
    }

    // 停止定时器
    count = __HAL_TIM_GetCounter(&HTIM);
    HAL_TIM_Base_Stop(&HTIM);

    // 通过定时器计数器计算距离
    distance = (uint16_t)(count * 0.017);

    return distance;
}
