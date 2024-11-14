/**
 ****************************************************************************************************
 * @file        pid.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-14
 * @brief       PID算法代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 F407电机开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com/forum.php
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20211014
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "pid.h"

PID_TypeDef g_balance_pid;
PID_TypeDef g_turn_pid;
PID_TypeDef g_speed_pid;           /* 速度环PID参数结构体 */

/**
 * @brief       pid初始化
 * @param       无
 * @retval      无
 */
void pid_init(PID_TypeDef *PID, float kp, float ki, float kd)
{
    PID->SetPoint = 0.0f;
    PID->ActualValue = 0.0f;
    PID->SumError = 0.0f;
    PID->Error = 0.0f;
    PID->LastError = 0.0f;
    PID->PrevError = 0.0f;
    PID->Proportion = kp;
    PID->Integral = ki;
    PID->Derivative = kd;
}

/**
 * @brief       pid闭环控制
 * @param       当前控制量
 * @retval      目标控制量
 */
float increment_pid_ctrl(PID_TypeDef *PID, float Feedback_value, float Feedback_value2)
{
    PID->Error = Feedback_value - PID->SetPoint;                   /* 计算偏差 */
    if (PID->Error > 10000) PID->Error = 10000;
    if (PID->Error < -10000) PID->Error = -10000;
    PID->ActualValue =
            (PID->Proportion * PID->Error)
            + (PID->Integral * Feedback_value)
            + (PID->Derivative * Feedback_value2);

    return PID->ActualValue;                                   /* 返回计算后输出的数值 */

}