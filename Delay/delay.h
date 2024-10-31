
 
#ifndef __DELAY_H
#define __DELAY_H


#include "stm32f1xx.h"
#include "core_cm3.h"
#include "stm32f1xx_hal.h"

/*******************************************************************************************************/

void delay_init(uint16_t sysclk);           /* 初始化延迟函数 */
void delay_ms(uint16_t nms);                /* 延时nms */
void delay_us(uint32_t nus);                /* 延时nus */

void HAL_Delay(uint32_t Delay);             /* HAL库的延时函数，SDIO等需要用到 */

#endif

