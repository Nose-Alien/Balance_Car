#ifndef __PID_H
#define __PID_H
#include "main.h"

/******************************************************************************************/
/* PID相关参数 */

#define INCR_LOCT_SELECT  0         /* 0选择位置式，1：增量式控制 */

#if INCR_LOCT_SELECT

/* 默认 PID 参数宏 */
#define KP      8.50f               /* 默认 P 参数 */
#define KI      5.00f               /* 默认 I 参数 */
#define KD      0.10f               /* 默认 D 参数 */
#define SMAPLSE_PID_SPEED  10       /* 采样周期 单位 ms */

#else

/* 位置式 PID 参数宏 */
#define KP      10.0f               /* P 参数 */
#define KI      6.00f               /* I 参数 */
#define KD      0.5f                /* D 参数 */
#define SMAPLSE_PID_SPEED  10       /* 采样周期 单位 ms */

#endif

/* PID 结构体 */
typedef struct
{
    __IO float SetPoint;           /* 设定目标 */
    __IO float ActualValue;        /* 实际值 */
    __IO float SumError;           /* 误差累计 */
    __IO float Proportion;         /* 比例常数 P */
    __IO float Integral;           /* 积分常数 I */
    __IO float Derivative;         /* 微分常数 D */
    __IO float Error;              /* 当前误差 */
    __IO float LastError;          /* 前一次误差 */
    __IO float PrevError;          /* 前两次误差 */
} PID_TypeDef;

/* 外部声明 */
extern PID_TypeDef g_speed_pid;     /* 速度环 PID 参数结构体 */
extern PID_TypeDef g_balance_pid;   /* 直立环 PID 参数结构体 */
extern PID_TypeDef g_turn_pid;      /* 转向环 PID 参数结构体 */

/******************************************************************************************/

/* 函数声明 */
void pid_init(PID_TypeDef *PID, float kp, float ki, float kd);  /* PID 初始化 */
//void pid_init(PID_TypeDef *PID);
//int32_t increment_pid_ctrl(PID_TypeDef *PID, float Feedback_value);  /* PID 闭环控制 */
float increment_pid_ctrl(PID_TypeDef *PID,float Feedback_value,float Feedback_value2);

#endif
