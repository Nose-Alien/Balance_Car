
#ifndef LJY_USER_MAIN_H
#define LJY_USER_MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "main.h"
#include "stm32f1xx_hal.h"

#define BLC_KP  390//390
#define BLC_KD  1.5//1.5
#define SPD_KP  -130
#define SPD_KI  -0.65
#define TURN_KP   -20
#define TURN_KD   -0.5
int user_main();
void mode_oled(uint8_t mode);
void mpu6050_oled();
void Encoder_oled();
int16_t Read_Encoder(uint8_t X);
int velocity(int encoder_left,int encoder_right,int Target_Speed);
int balance_UP(float Angle,float Mechanical_balance,float Gyro);
int Turn_UP(int gyro_Z, int RC);
void Turn_Off(float angle, float voltage);
void Xianfu_Pwm(void);
void run(void);
void mode_job(int mode);

#ifdef __cplusplus
}
#endif

#endif //LJY_USER_MAIN_H
