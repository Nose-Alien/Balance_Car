#include "motor.h"
#include "tim.h"

/**************************************************************************
函数功能：赋值给PWM寄存器
入口参数：左轮PWM、右轮PWM
返回  值：无
**************************************************************************/
void Set_Pwm(int moto1, int moto2)
{
    if (moto1 > 0) {
        AIN1_0;
        AIN2_1;
    } else {
        AIN1_1;
        AIN2_0;
    }
    if (moto2 > 0) {
        BIN1_0;
        BIN2_1;
    } else {
        BIN1_1,
        BIN2_0;
    }
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, myabs(moto1));
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, myabs(moto2));
}


/**************************************************************************
函数功能：绝对值函数
入口参数：int
返回  值：unsigned int
目    的：经过直立环和速度环以及转向环计算出来的PWM有可能为负值
		 而只能赋给定时器PWM寄存器只能是正值。故需要对PWM进行绝对值处理
**************************************************************************/
int myabs(int a)
{
    int temp;
    if (a < 0) {
        temp = -a;
    } else {
        temp = a;
    }
    return temp;
}


	
