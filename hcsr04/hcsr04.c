//
// Created by nose on 24-4-29.
//
#include "hcsr04.h"
#include "delay.h"

//static void user_delaynus_tim(uint32_t nus,TIM_HandleTypeDef *htim)
//{
//
//    uint16_t  differ = 0xffff-nus-5;
//    //设置定时器2的技术初始值
//            __HAL_TIM_SetCounter(htim,differ);
//    //开启定时器
//    HAL_TIM_Base_Start(htim);
//
//    while( differ<0xffff-5)
//    {
//        differ = __HAL_TIM_GetCounter(htim);
//    };
//    //关闭定时器
//    HAL_TIM_Base_Stop(htim);
//}

uint16_t Hcsr04_Job()
{
    uint16_t count;
    uint16_t distance;
    TRIG_PULL_1;
    delay_us(20);
    TRIG_PULL_0;
    // 启动定时器
    HAL_TIM_Base_Start(&HTIM);
    // 等待 Echo 引脚变高电平
    while (ECHO_ReadPin != GPIO_PIN_SET){}
    // 重置定时器计数器
            __HAL_TIM_SetCounter(&HTIM, 0);
    // 等待 Echo 引脚变低电平
    while (ECHO_ReadPin == GPIO_PIN_SET);

    // 停止定时器
    count = __HAL_TIM_GetCounter(&HTIM);
    HAL_TIM_Base_Stop(&HTIM);

    // 通过定时器计数器计算距离
    distance = (uint16_t)(count * 0.017);

    return distance;
}