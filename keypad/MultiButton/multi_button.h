#ifndef _MULTI_BUTTON_H_
#define _MULTI_BUTTON_H_

#include "stdint.h"
#include "string.h"

// 根据需要修改常量
#define TICKS_INTERVAL    5   // 定时器间隔（毫秒）
#define DEBOUNCE_TICKS    3   // 去抖动计数（最大值为8）
#define SHORT_TICKS       (300 / TICKS_INTERVAL)  // 短按的时间阈值
#define LONG_TICKS        (1000 / TICKS_INTERVAL) // 长按的时间阈值

// 按键事件回调函数类型定义
typedef void (*BtnCallback)(void *);
// 按键电平回调函数类型定义
typedef uint8_t (*BtnLevelCallback)(uint8_t);

// 按键事件枚举定义
typedef enum {
    PRESS_DOWN = 0,       // 按下
    PRESS_UP,             // 松开
    PRESS_REPEAT,        // 持续按下
    SINGLE_CLICK,        // 单击
    DOUBLE_CLICK,        // 双击
    LONG_PRESS_START,    // 长按开始
    LONG_PRESS_HOLD,     // 长按保持
    number_of_event,     // 事件数量
    NONE_PRESS           // 无按键事件
} PressEvent;

// 按键结构体定义
typedef struct Button {
    uint16_t ticks;                  // 当前计时器值
    uint8_t  repeat : 4;             // 按键重复状态
    uint8_t  event : 4;              // 当前事件类型
    uint8_t  state : 3;              // 按键状态
    uint8_t  debounce_cnt : 3;       // 去抖动计数
    uint8_t  active_level : 1;       // 按键激活电平
    uint8_t  button_level : 1;       // 按键电平状态
    uint8_t  button_id;              // 按键ID
    BtnLevelCallback hal_button_Level; // 按键电平回调函数
    BtnCallback  cb[number_of_event]; // 事件回调函数数组
    struct Button* next;             // 指向下一个按钮的指针
} Button;

#ifdef __cplusplus
extern "C" {
#endif

// 初始化按钮
void button_init(struct Button* handle, BtnLevelCallback pin_level, uint8_t active_level, uint8_t button_id);

// 附加事件回调函数
void button_attach(struct Button* handle, PressEvent event, BtnCallback cb);

// 取消附加事件回调函数
void button_detach(struct Button* handle, PressEvent event);

// 取消附加所有事件回调函数
void button_detachAll(struct Button* handle);

// 获取按钮事件
PressEvent get_button_event(struct Button* handle);

// 启动按钮处理
int button_start(struct Button* handle);

// 停止按钮处理
void button_stop(struct Button* handle);

// 更新按钮计时器（需在系统定时器中调用）
void button_ticks(void);

#ifdef __cplusplus
}
#endif

#endif
