#include <sys/cdefs.h>

#ifndef LJY_USER_MAIN_H
#define LJY_USER_MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "main.h"
#include "stm32f1xx_hal.h"
#include "driver_ssd1306.h"
#include "delay.h"
#include "adc.h"
#include "tim.h"
#include "hcsr04.h"
#include "motor.h"
#include "atk_ms6050.h"
#include "inv_mpu.h"
#include "multi_button.h"
#include "button_port.h"
#include "pid.h"
#define BLC_KP  390//390
#define BLC_KD  1.5//1.5
#define SPD_KP  -130
#define SPD_KI  -0.65
#define TURN_KP   -20
#define TURN_KD   -0.5

_Noreturn void user_main();
void Mode_Oled(uint8_t Mode);
void mpu6050_oled();
void Encoder_oled();
int16_t Read_Encoder(uint8_t X);
float velocity_UP(int encoder_left, int encoder_right, float target_Speed);
float balance_UP(float Angle,float Mechanical_balance,float Gyro);
float Turn_UP(int gyro_Z, float RC);
void Turn_Off(float angle, float voltage);
void Xianfu_Pwm(void);
void Run(void);
void mode_select(int Mode);
void Run_Enter();
void Run_Key();
void Mode_State(int Mode);
void Git_Ultrasonic(int8_t sign);
void Obstacle_Oled();
void NRF24L01_Oled();
struct run {
    uint16_t Event; // 按键事件标志
};
#ifdef __cplusplus
}
#endif

#endif //LJY_USER_MAIN_H
