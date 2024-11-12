#ifndef BUTTON_PORT_H
#define BUTTON_PORT_H

#include <multi_button.h>

// 定义触摸按键名称的枚举类型
typedef enum {
    TOUCH_KEY_1,          // 触摸按键 1
    NUMBER_OF_TOUCH_KEY, // 触摸按键总数
    TOUCH_KEY_NONE,      // 无触摸按键事件
} TouchKey_NameTypedef;

// 定义触摸按键数组
extern Button TouchKey[NUMBER_OF_TOUCH_KEY];

// 初始化触摸按键
void TouchKey_Init(void);

// 取消所有触摸按键的事件回调函数
void TouchKey_DetachAllKey(void);

#endif // BUTTON_PORT_H
