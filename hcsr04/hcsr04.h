//
// Created by nose on 24-4-29.
//

#ifndef HCSR04_GS90_HCSR04_H
#define HCSR04_GS90_HCSR04_H

#include <stdint.h>
#include "main.h"
#include "tim.h"
#include "gpio.h"

#define TRIG_PORT        GPIOB
#define TRIG_PIN        GPIO_PIN_8

#define ECHO_PORT         GPIOB
#define ECHO_PIN        GPIO_PIN_9

#define TRIG_PULL_1		HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN,GPIO_PIN_SET)
#define TRIG_PULL_0		HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET)

#define ECHO_ReadPin	HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN)

#define HTIM htim4//根据使用的定时器更改

uint16_t hcrs04_get_distance();

#endif //HCSR04_GS90_HCSR04_H
