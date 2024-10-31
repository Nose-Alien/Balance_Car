#ifndef __MOTOR_H
#define __MOTOR_H

#include "main.h"

#define AIN2_PORT        GPIOB
#define AIN2_PIN        GPIO_PIN_12
#define AIN1_PORT        GPIOB
#define AIN1_PIN        GPIO_PIN_13

#define BIN1_PORT        GPIOB
#define BIN1_PIN        GPIO_PIN_14
#define BIN2_PORT        GPIOB
#define BIN2_PIN        GPIO_PIN_15

#define AIN1_1        HAL_GPIO_WritePin(AIN1_PORT, AIN1_PIN,GPIO_PIN_SET)
#define AIN1_0        HAL_GPIO_WritePin(AIN1_PORT, AIN1_PIN, GPIO_PIN_RESET)

#define AIN2_1        HAL_GPIO_WritePin(AIN2_PORT, AIN2_PIN,GPIO_PIN_SET)
#define AIN2_0        HAL_GPIO_WritePin(AIN2_PORT, AIN2_PIN, GPIO_PIN_RESET)

#define BIN1_1        HAL_GPIO_WritePin(BIN1_PORT, BIN1_PIN,GPIO_PIN_SET)
#define BIN1_0        HAL_GPIO_WritePin(BIN1_PORT, BIN1_PIN, GPIO_PIN_RESET)

#define BIN2_1        HAL_GPIO_WritePin(BIN2_PORT, BIN2_PIN,GPIO_PIN_SET)
#define BIN2_0        HAL_GPIO_WritePin(BIN2_PORT, BIN2_PIN, GPIO_PIN_RESET)


void Motor_Init(void);

void Set_Pwm(int moto1, int moto2);

int myabs(int a);


#endif
