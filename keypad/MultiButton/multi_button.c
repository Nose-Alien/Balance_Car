#include "multi_button.h"

#define EVENT_CB(ev)   if(handle->cb[ev])handle->cb[ev]((Button*)handle)

// 按钮句柄链表头
static struct Button *head_handle = NULL;

/**
  * @brief  初始化按钮句柄结构体
  * @param  handle: 按钮句柄结构体
  * @param  pin_level: 读取按钮连接的 GPIO 电平的回调函数
  * @param  active_level: 按钮按下时的 GPIO 电平
  * @param  button_id: 按钮的 ID
  * @retval 无
  */
void button_init(struct Button *handle, BtnLevelCallback pin_level, uint8_t active_level, uint8_t button_id)
{
    memset(handle, 0, sizeof(struct Button));
    handle->event = (uint8_t) NONE_PRESS;
    handle->hal_button_Level = pin_level;
    handle->button_level = handle->hal_button_Level(button_id);
    handle->active_level = active_level;
    handle->button_id = button_id;
}

/**
  * @brief  附加按钮事件的回调函数
  * @param  handle: 按钮句柄结构体
  * @param  event: 触发的事件类型
  * @param  cb: 回调函数
  * @retval 无
  */
void button_attach(struct Button *handle, PressEvent event, BtnCallback cb)
{

    handle->cb[event] = cb;
}

/**
  * @brief  分离按钮事件的回调函数
  * @param  handle: 按钮句柄结构体
  * @param  event: 触发的事件类型
  * @retval 无
  */
void button_detach(struct Button *handle, PressEvent event)
{
    handle->cb[event] = NULL;
}

void button_detachAll(struct Button *handle)
{
    for (int i = 0; i < number_of_event; ++i) handle->cb[i] = NULL;
}

/**
  * @brief  查询按钮事件
  * @param  handle: 按钮句柄结构体
  * @retval 按钮事件
  */
PressEvent get_button_event(struct Button *handle)
{
    return (PressEvent) (handle->event);
}

/**
  * @brief  按钮驱动核心函数，状态机实现
  * @param  handle: 按钮句柄结构体
  * @retval 无
  */
void button_handler(struct Button *handle)
{
    uint8_t read_gpio_level = handle->hal_button_Level(handle->button_id);

    // 计时器工作
    if ((handle->state) > 0) handle->ticks++;

    /*------------按钮去抖处理---------------*/
    if (read_gpio_level != handle->button_level) { // 与之前的状态不同
        // 连续读取 3 次相同的新的电平变化
        if (++(handle->debounce_cnt) >= DEBOUNCE_TICKS) {
            handle->button_level = read_gpio_level;
            handle->debounce_cnt = 0;
        }
    } else { // 电平未变化，计数器复位
        handle->debounce_cnt = 0;
    }

    /*-----------------状态机处理-------------------*/
    switch (handle->state) {
        case 0:
            if (handle->button_level == handle->active_level) {    // 按钮按下
                handle->event = (uint8_t) PRESS_DOWN;
                EVENT_CB(PRESS_DOWN);
                handle->ticks = 0;
                handle->repeat = 1;
                handle->state = 1;
            } else {
                handle->event = (uint8_t) NONE_PRESS;
            }
            break;

        case 1:
            if (handle->button_level != handle->active_level) { // 按钮释放
                handle->event = (uint8_t) PRESS_UP;
                EVENT_CB(PRESS_UP);
                handle->ticks = 0;
                handle->state = 2;

            } else if (handle->ticks > LONG_TICKS) {
                handle->event = (uint8_t) LONG_PRESS_START;
                EVENT_CB(LONG_PRESS_START);
                handle->state = 5;
            }
            break;

        case 2:
            if (handle->button_level == handle->active_level) { // 再次按下
                handle->event = (uint8_t) PRESS_DOWN;
                EVENT_CB(PRESS_DOWN);
                handle->repeat++;
                EVENT_CB(PRESS_REPEAT); // 重复按下
                handle->ticks = 0;
                handle->state = 3;
            } else if (handle->ticks > SHORT_TICKS) { // 释放超时
                if (handle->repeat == 1) {
                    handle->event = (uint8_t) SINGLE_CLICK;
                    EVENT_CB(SINGLE_CLICK);
                } else if (handle->repeat == 2) {
                    handle->event = (uint8_t) DOUBLE_CLICK;
                    EVENT_CB(DOUBLE_CLICK); // 双击
                }
                handle->state = 0;
            }
            break;

        case 3:
            if (handle->button_level != handle->active_level) { // 按钮释放
                handle->event = (uint8_t) PRESS_UP;
                EVENT_CB(PRESS_UP);
                if (handle->ticks < SHORT_TICKS) {
                    handle->ticks = 0;
                    handle->state = 2; // 重复按下
                } else {
                    handle->state = 0;
                }
            } else if (handle->ticks > SHORT_TICKS) { // 长按释放
                handle->state = 0;
            }
            break;

        case 5:
            if (handle->button_level == handle->active_level) {
                // 持续按下
                handle->event = (uint8_t) LONG_PRESS_HOLD;
                EVENT_CB(LONG_PRESS_HOLD);

            } else { // 释放
                handle->event = (uint8_t) PRESS_UP;
                EVENT_CB(PRESS_UP);
                handle->state = 0; // 状态复位
            }
            break;
        default:handle->state = 0; // 状态复位
            break;
    }
}

/**
  * @brief  开始按钮工作，将按钮加入工作链表
  * @param  handle: 目标按钮句柄结构体
  * @retval 0: 成功，-1: 已经存在
  */
int button_start(struct Button *handle)
{
    struct Button *target = head_handle;
    while (target) {
        if (target == handle) return -1;    // 已经存在
        target = target->next;
    }
    handle->next = head_handle;
    head_handle = handle;
    return 0;
}

/**
  * @brief  停止按钮工作，将按钮从工作链表移除
  * @param  handle: 目标按钮句柄结构体
  * @retval 无
  */
void button_stop(struct Button *handle)
{
    struct Button **curr;
    for (curr = &head_handle; *curr;) {
        struct Button *entry = *curr;
        if (entry == handle) {
            *curr = entry->next;
            return; // glacier 添加 2021-8-18
        } else
            curr = &entry->next;
    }
}

/**
  * @brief  后台定时器，定时调用间隔 5ms
  * @param  无
  * @retval 无
  */
void button_ticks()
{
    struct Button *target;
    for (target = head_handle; target; target = target->next) {
        button_handler(target);
    }
}
