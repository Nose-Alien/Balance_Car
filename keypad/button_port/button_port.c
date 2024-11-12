#include "button_port.h"
#include "gpio.h"

// 初始化触摸按键数组
Button TouchKey[NUMBER_OF_TOUCH_KEY] = {0};

// 定义一个GPIO结构体，用于描述一个引脚的分组和序号
typedef struct
{
    GPIO_TypeDef *GPIOx; // GPIO组
    uint16_t GPIO_Pin;   // GPIO引脚
} GPIO_Struct, *GPIO_Struct_P;

// 初始化GPIO结构体数组，描述每个触摸按键的GPIO组和引脚
GPIO_Struct KeyList[NUMBER_OF_TOUCH_KEY] = {
        {.GPIOx = Key1_GPIO_Port, .GPIO_Pin = Key1_Pin},
};

// 回调函数：获取指定触摸按键的电平状态
static uint8_t TouchKey_GetLevelCallback(TouchKey_NameTypedef name)
{
    return HAL_GPIO_ReadPin(KeyList[name].GPIOx, KeyList[name].GPIO_Pin);
}

// 初始化触摸按键
void TouchKey_Init(void)
{
    for (int i = 0; i < NUMBER_OF_TOUCH_KEY; ++i) {
        // 初始化每个触摸按键
        button_init(&TouchKey[i], (BtnLevelCallback)TouchKey_GetLevelCallback, GPIO_PIN_RESET, i);
        button_start(&TouchKey[i]); // 启动按键检测
    }
}

// 取消所有触摸按键的事件回调函数
void TouchKey_DetachAllKey(void)
{
    for (int i = 0; i < NUMBER_OF_TOUCH_KEY; ++i) {
        button_detachAll(&TouchKey[i]); // 取消所有事件回调
    }
}
