/**
  * @file user_main.c
  * @brief 用户主程序的入口文件
  * @author
  * @date 2024/9/6
  */

#include <stdio.h>
#include "user_main.h"
#include "usart.h"

double Moto1 = 0, Moto2 = 0;///计算出来的最终赋给电机的PWM
float pit, rol, yaw;
int Encoder_Left, Encoder_Right;//左右编码器的脉冲计数
int line_1 = 0, line_2 = 12, line_3 = 24,
        line_4 = 36, line_5 = 48, line_6 = 60;  // 显示列间距变量，控制显示内容的列间隔
int distance = 0;  // 超声波测距结果
int8_t distance_sign = 0; //超声波标志位
int8_t obstacle_sign = 0; //避障标志位
int mode = 1;  // 模式选择变量
uint8_t Uart_Receive = 0;

#define SPEED_Y 90//俯仰(前后)最大设定速度
#define SPEED_Z 85//偏航(左右)最大设定速度

float Balance_Pwm, Velocity_Pwm;
float Turn_Pwm;
int16_t gyr_x, gyr_y, gyr_z;

float Mechanical_angle = 0;
float Target_Speed = 0;    //期望速度（俯仰）。用于控制小车前进后退及其速度。
float Turn_Speed = 0;        //期望速度（偏航）

float balance_KP = BLC_KP;     // 小车直立环PD参数
float balance_KD = BLC_KD;

float velocity_KP = SPD_KP;     // 小车速度环PI参数
float velocity_KI = SPD_KI;

float Turn_Kd = TURN_KD;//转向环KP、KD
float Turn_KP = TURN_KP;
float Voltage; //电池电压


/**
  * @brief 主程序入口函数，初始化按键检测和运行逻辑
  * @return int
  */
_Noreturn void user_main()
{
    delay_init(72);
    OLED_Init();
    my_mpu6050_init();
    // 启动 TIM1 CH1 和 CH4 的 PWM 输出
    __HAL_RCC_TIM1_CLK_ENABLE();
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
    __HAL_TIM_MOE_ENABLE(&htim1);
    HAL_UART_Receive_IT(&huart2, &Uart_Receive, 1);

    HAL_TIM_Base_Start_IT(&htim4);
    TouchKey_Init();  // 初始化触摸按键功能
    Run_Enter();

    pid_init(&g_balance_pid, BLC_KP, 0.0f, BLC_KD); // 初始化直立环 PD
//    pid_init(&g_speed_pid, 0.0f, 0.0f, 0.0f); // 初始化速度环 PID
//    pid_init(&g_turn_pid, TURN_KP, 0.0f, TURN_KD); // 初始化转向环 PD
    while (1) {
        OLED_Clear(0x00);  // 清空OLED显示屏
        Mode_Oled(mode);    // 显示模式选择
        Run_Key();
        Git_Ultrasonic(distance_sign);
        Mode_State(mode);
        OLED_Refresh_Gram();  // 刷新OLED显示内容
    }
}

/**
  * @brief 获取超声波距离
  * @param sign 工作标志位
  */
void Git_Ultrasonic(int8_t sign)
{
    if (sign == 1) {
        distance = hcrs04_get_distance();
        sign = 0;
    }
}

/**
  * @brief 模式状态
  * @param Mode 模式
  */
void Mode_State(int Mode)
{
    if (mode == 1) {
        if(Uart_Receive== 0||Uart_Receive== 90) {
            Target_Speed = 0, Turn_Speed = 0;
        }
    }
    if (Mode == 3) {
        if (obstacle_sign == 1) {
            Target_Speed = -30;
            Turn_Speed = -2;
            delay_ms(500);
            Target_Speed = 0;
            Turn_Speed = -80;
            obstacle_sign = 0;
        } else if (obstacle_sign == 0) {
            Target_Speed = 20, Turn_Speed = -3;
        }
    }
}
// 串口中断回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        HAL_UART_Receive_IT(&huart2, &Uart_Receive, 1); // 再次使能接收中断
        BluetoothCMD(Uart_Receive); // 处理接收到的数据
    }
}
void BluetoothCMD(int uart_Receive)
{
    switch(uart_Receive)
    {
        case 90://停止 Z
            Target_Speed = 0, Turn_Speed = 0;
            break;
        case 65://前进 A
            Target_Speed = 40, Turn_Speed = -3;
            break;
        case 72://左前 H
            Target_Speed = 40, Turn_Speed = 30;
            break;
        case 66://右前 B
            Target_Speed = 40, Turn_Speed = -30;
            break;
        case 71://左转 G
            Turn_Speed = 100;
            break;
        case 67://右转 C
            Turn_Speed = -100;
            break;
        case 69://后退 E
            Target_Speed = -40, Turn_Speed = 3;
            break;
        case 70://左后,向右旋 F
            Target_Speed = -40, Turn_Speed = -30;
            break;
        case 68://右后，向左旋 D
            Target_Speed = -40, Turn_Speed = 30;
            break;
        default://停止 Z
            Target_Speed = 0, Turn_Speed = 0;
            break;

    }
}

/**
  * @brief 外部中断回调函数
  * @param GPIO_Pin 外部中断引脚
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_5) {//mpu6050，10ms发一次数据
        Run();
    }
}

void Run(void)//10ms工作一次
{
    static uint32_t tick_10ms = 0;  // 静态变量，跟踪时间滴答
    tick_10ms++;  // 每次中断时增加计数

    if (tick_10ms % 1 == 0) { //10ms执行一次
        button_ticks();//更新按键状态
        mode_select(mode); //工作模式
        atk_ms6050_dmp_get_data(&pit, &rol, &yaw);
        atk_ms6050_get_gyroscope(&gyr_x, &gyr_y, &gyr_z);

        Encoder_Left = Read_Encoder(2);                            //读取编码器的值
        Encoder_Right = -Read_Encoder(3);                          //读取编码器的值
        if (Target_Speed > SPEED_Y) Target_Speed = SPEED_Y;  //速度环前后速度限幅
        if (Target_Speed < -SPEED_Y) Target_Speed = -SPEED_Y;
        if (Turn_Speed > SPEED_Z) Turn_Speed = SPEED_Z;  //转向环转向速度限幅
        if (Turn_Speed < -SPEED_Z) Turn_Speed = -SPEED_Z;

        Balance_Pwm = balance_UP(pit, Mechanical_angle, gyr_y);//===直立环PID控制
        Velocity_Pwm = velocity_UP(Encoder_Left, Encoder_Right*2 , Target_Speed);//===速度环PID控制
        Turn_Pwm = Turn_UP(gyr_z, Turn_Speed);//===转向环PID控制
        Moto1 = Balance_Pwm - Velocity_Pwm + Turn_Pwm;//===计算左轮电机最终PWM
        Moto2 = Balance_Pwm - Velocity_Pwm - Turn_Pwm;//===计算右轮电机最终PWM
        Xianfu_Pwm();//===PWM限幅
        Turn_Off(pit, Voltage);//===检查角度以及电压是否正常
        Set_Pwm(Moto1, Moto2);//===赋值给PWM寄存器
    }

    if (tick_10ms % 20 == 0) { //200ms执行一次
        distance_sign = distance_sign == 0 ? 1 : 0;
    }
    if (tick_10ms % 50 == 0) { //500ms执行一次
        HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    }
    if (tick_10ms % 100 == 0) { //1s执行一次
        tick_10ms = 0;     // 重置计时器计数
    }
}

void Mode_Oled(uint8_t Mode)
{
    Voltage = Get_Voltage();
    switch (Mode) {
        case 1: {
            Bluetooth_Oled();
            mpu6050_oled();
            Encoder_oled();

            break;
        }
        case 2: {
            NRF24L01_Oled();
            mpu6050_oled();
            Encoder_oled();
            break;
        }
        case 3: {
            Obstacle_Oled();
            mpu6050_oled();
            Encoder_oled();
            break;
        }
        default: {
            break;
        }
    }
}

void NRF24L01_Oled()
{
    OLED_DrawStr(28, line_1, "NRF24L01", MEDIUM, 0);
    OLED_DrawChar(0, line_2, 'V', MEDIUM, 0);
    OLED_DrawChar(6, line_2, ':', MEDIUM, 0);
    OLED_DrawNum0(12, line_2, (int) Voltage, MEDIUM, 0);
    OLED_DrawChar(28, line_2, '.', MEDIUM, 0);
    OLED_DrawNum(32, line_2, (uint16_t) (Voltage * 10) % 10, MEDIUM, 0);  // 第一位小数
}
void Bluetooth_Oled()
{
    OLED_DrawStr(28, line_1, "Bluetooth", MEDIUM, 0);
    OLED_DrawChar(0, line_2, 'V', MEDIUM, 0);
    OLED_DrawChar(6, line_2, ':', MEDIUM, 0);
    OLED_DrawNum0(12, line_2, (int) Voltage, MEDIUM, 0);
    OLED_DrawChar(28, line_2, '.', MEDIUM, 0);
    OLED_DrawNum(32, line_2, (uint16_t) (Voltage * 10) % 10, MEDIUM, 0);  // 第一位小数
}

void Obstacle_Oled()
{
    OLED_DrawStr(28, line_1, "Obstacle", MEDIUM, 0);
    OLED_DrawChar(0, line_2, 'V', MEDIUM, 0);
    OLED_DrawChar(6, line_2, ':', MEDIUM, 0);
    OLED_DrawNum0(12, line_2, (int) Voltage, MEDIUM, 0);
    OLED_DrawChar(28, line_2, '.', MEDIUM, 0);
    OLED_DrawNum(32, line_2, (uint16_t) (Voltage * 10) % 10, MEDIUM, 0);  // 第一位小数
}

void mpu6050_oled()
{
    OLED_DrawStr(0, line_3, "P:", MEDIUM, 0);
    OLED_ShowSignedNum(16, line_3, (int) pit, MEDIUM, 0);
    OLED_DrawStr(0, line_4, "R:", MEDIUM, 0);
    OLED_ShowSignedNum(16, line_4, (int) rol, MEDIUM, 0);
    OLED_DrawStr(0, line_5, "Y:", MEDIUM, 0);
    OLED_ShowSignedNum(16, line_5, (int) yaw, MEDIUM, 0);
}

void Encoder_oled()
{
    OLED_DrawStr(56, line_2, "L:", MEDIUM, 0);
    OLED_ShowSignedNum(72, line_2, Encoder_Left, MEDIUM, 0);
    OLED_DrawStr(56, line_3, "R:", MEDIUM, 0);
    OLED_ShowSignedNum(72, line_3, Encoder_Right, MEDIUM, 0);
    OLED_DrawNum(56, line_4, distance, MEDIUM, 0);
    OLED_DrawNum(56, line_5, Uart_Receive, MEDIUM, 0);
}

/**************************************************************************
函数功能：单位时间读取编码器计数
入口参数：定时器编号
返回  值：速度值
**************************************************************************/
int16_t Read_Encoder(uint8_t X)
{
    int16_t Speed_Value = 0;
    switch (X) {
        case 2:
            Speed_Value = __HAL_TIM_GET_COUNTER(&htim2); // 读取TIM2的计数值
            __HAL_TIM_SET_COUNTER(&htim2, 0); // 清零计数器
            break;
        case 3:
            Speed_Value = __HAL_TIM_GET_COUNTER(&htim3); // 读取TIM3的计数值
            __HAL_TIM_SET_COUNTER(&htim3, 0); // 清零计数器
            break;
        default:
            Speed_Value = 0;
    }

    return Speed_Value;
}

/**************************************************************************
函数功能：直立PD控制
入口参数：角度、机械平衡角度（机械中值）、角速度
返回  值：直立控制PWM
**************************************************************************/
float balance_UP(float Angle, float Mechanical_balance, float Gyro)
{
    g_balance_pid.SetPoint = Mechanical_balance;
    return increment_pid_ctrl(&g_balance_pid, Angle, Gyro);
}

/**************************************************************************
函数功能：速度PI控制
入口参数：电机编码器的值
返回  值：速度控制PWM
**************************************************************************/

float velocity_UP(int encoder_left, int encoder_right, float target_Speed)
{
    static float Velocity, Encoder_Least, Encoder;
    static float Encoder_Integral;
    Encoder_Least = (float) (encoder_left +
                             encoder_right);//-target;                    //===获取最新速度偏差==测量速度（左右编码器之和）-目标速度
    Encoder *= (float) 0.8;                                                        //===一阶低通滤波器
    Encoder += (float) (Encoder_Least * 0.2);                                        //===一阶低通滤波器
    Encoder_Integral += Encoder;                                       //===积分出位移 积分时间：5ms

    if (pit < -40 || pit > 40) {
        target_Speed = 0;
        Encoder_Integral = 0;
        Encoder = 0;
    }

    Encoder_Integral = Encoder_Integral - target_Speed;                       //===接收遥控器数据，控制前进后退
    if (Encoder_Integral > 10000) Encoder_Integral = 10000;             //===积分限幅
    if (Encoder_Integral < -10000) Encoder_Integral = -10000;            //===积分限幅
    Velocity = Encoder_Integral * velocity_KI + Encoder * velocity_KP;
    if (pit < -40 || pit > 70) Encoder_Integral = 0;                            //===电机关闭后清除积分

    return Velocity;
}
/**************************************************************************
函数功能：转向PD控制
入口参数：电机编码器的值、Z轴角速度
返回  值：转向控制PWM
**************************************************************************/

float Turn_UP(int gyro_Z, float RC)
{
    float PWM_out;
    if (RC == 0)Turn_Kd = TURN_KD;//若无左右转向指令，则开启转向约束
    else Turn_Kd = 0;//若左右转向指令接收到，则去掉转向约束
    PWM_out = Turn_Kd * (float) gyro_Z + Turn_KP * RC;
    return PWM_out;
}

//float Turn_UP(int gyro_Z, float RC)
//{
//    if (RC == 0) {//若无左右转向指令，则开启转向约束
//        Turn_Kd = TURN_KD;
//    }
//    else{
//        Turn_Kd = 0;//若左右转向指令接收到，则去掉转向约束
//    }
//    g_turn_pid.Derivative = Turn_Kd;
//    return increment_pid_ctrl(&g_turn_pid,
//                              RC,
//                              (float) gyro_Z);;
//}

/**************************************************************************
函数功能：限制PWM赋值
入口参数：无
返回  值：无
**************************************************************************/
void Xianfu_Pwm(void)
{
    //===PWM满幅是7200 限制在7000
    if (Moto1 < -7000) Moto1 = -7000;
    if (Moto1 > 7000) Moto1 = 7000;
    if (Moto2 < -7000) Moto2 = -7000;
    if (Moto2 > 7000) Moto2 = 7000;
}

/**************************************************************************
函数功能：异常关闭电机
入口参数：倾角和电压
返回  值：无
**************************************************************************/
void Turn_Off(float angle, float voltage)
{
    if (angle < -40 || angle > 70 || voltage < 11)     //电池电压低于11V关闭电机
    {                                       //===倾角大于40度关闭电机
        Moto1 = 0;
        Moto2 = 0;
    }
}

/**************************************************************************
函数功能：模式选择
入口参数：模式
返回  值：无
**************************************************************************/
void mode_select(int Mode)
{
    static uint8_t SR04_Counter = 0;
    static uint32_t Count = 0;
    switch (Mode) {
        case 1: {//蓝牙模式
        }
            break;
//        case 2:  //NRF24L01模式
//            if(Buf[3]==0) Target_Speed=0;
//            if(Buf[3]==1) Target_Speed+=1;//前进
//            if(Buf[3]==2) Target_Speed-=1;//后退
//
//            if(Buf[2]==0) Turn_Speed=0;
//            if(Buf[2]==1) Turn_Speed-=10;//左转
//            if(Buf[2]==2) Turn_Speed+=10;//右转
//            break;

        case 3: {//避障模式
            if (distance <= 50) obstacle_sign = obstacle_sign == 0 ? 1 : 0;
            break;
        }
        default: {
            break;
        }
    }
}

// 结构体变量，存储运行时的按键事件信息
struct run event = {
        .Event = 0,  // 按键事件标志，默认为0表示无事件
};

// 按键事件处理函数
static void Run_KeyEventHandler(void *button)
{
    // 获取触摸按键的ID
    TouchKey_NameTypedef name = (((Button *) button)->button_id);

    // 根据按键ID触发对应的按键事件
    switch (name) {
        case TOUCH_KEY_1: {  // 按键1被按下，设置run.keyEvent为1
            event.Event = 1;
            break;
        }
        default: {
            break;
        }
    }
}

// 进入运行模式时绑定按键事件处理函数
void Run_Enter()
{
    // 为每个触摸按键绑定单击事件，处理函数为Run_KeyEventHandler
    button_attach(&TouchKey[TOUCH_KEY_1], SINGLE_CLICK, Run_KeyEventHandler);
}

// 退出运行模式时解绑按键事件处理函数
void Run_Exit()
{
    // 解绑所有按键的事件处理函数
    button_detachAll(&TouchKey[TOUCH_KEY_1]);;
}

void Run_Key()
{
    enum {
        KEY_EVENT_1 = 1
    };

    // 检查是否有按键事件发生
    if (event.Event != 0) {
        switch (event.Event) {
            case KEY_EVENT_1: {
                mode = mode == 1 ? 3 : 1;
                break;
            }
            default: {
                break;
            }
        }
        event.Event = 0;// 重置按键事件标志
    }
}