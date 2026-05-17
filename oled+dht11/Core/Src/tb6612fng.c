#include "tb6612fng.h"

// ========================== 用户配置 ==========================
// 用于PWM的定时器句柄 (请确保与您在CubeMX中配置的定时器一致)
extern TIM_HandleTypeDef htim3;

// PWM定时器通道
#define MOTOR_A_PWM_CHANNEL     TIM_CHANNEL_1
#define MOTOR_B_PWM_CHANNEL     TIM_CHANNEL_2

// PWM最大计数值 (ARR的值)
#define PWM_MAX_VALUE           999
// =============================================================

void TB6612_Init(void)
{
    // 启动电机A和B的PWM通道
    HAL_TIM_PWM_Start(&htim3, MOTOR_A_PWM_CHANNEL);
    HAL_TIM_PWM_Start(&htim3, MOTOR_B_PWM_CHANNEL);
    
    // 默认使能驱动器
    TB6612_Enable();
    
    // 默认停止所有电机
    TB6612_StopAll();
}

void TB6612_MotorA_SetSpeed(int16_t speed)
{
    // 限速
    if (speed > PWM_MAX_VALUE) speed = PWM_MAX_VALUE;
    if (speed < -PWM_MAX_VALUE) speed = -PWM_MAX_VALUE;

    if (speed > 0) // 正转
    {
        HAL_GPIO_WritePin(MOTOR_A_IN1_GPIO_Port, MOTOR_A_IN1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MOTOR_A_IN2_GPIO_Port, MOTOR_A_IN2_Pin, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim3, MOTOR_A_PWM_CHANNEL, speed);
    }
    else if (speed < 0) // 反转
    {
        HAL_GPIO_WritePin(MOTOR_A_IN1_GPIO_Port, MOTOR_A_IN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MOTOR_A_IN2_GPIO_Port, MOTOR_A_IN2_Pin, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(&htim3, MOTOR_A_PWM_CHANNEL, -speed);
    }
    else // 刹车
    {
        HAL_GPIO_WritePin(MOTOR_A_IN1_GPIO_Port, MOTOR_A_IN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MOTOR_A_IN2_GPIO_Port, MOTOR_A_IN2_Pin, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim3, MOTOR_A_PWM_CHANNEL, 0);
    }
}

void TB6612_MotorB_SetSpeed(int16_t speed)
{
    // 限速
    if (speed > PWM_MAX_VALUE) speed = PWM_MAX_VALUE;
    if (speed < -PWM_MAX_VALUE) speed = -PWM_MAX_VALUE;

    if (speed > 0) // 正转
    {
        HAL_GPIO_WritePin(MOTOR_B_IN1_GPIO_Port, MOTOR_B_IN1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MOTOR_B_IN2_GPIO_Port, MOTOR_B_IN2_Pin, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim3, MOTOR_B_PWM_CHANNEL, speed);
    }
    else if (speed < 0) // 反转
    {
        HAL_GPIO_WritePin(MOTOR_B_IN1_GPIO_Port, MOTOR_B_IN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MOTOR_B_IN2_GPIO_Port, MOTOR_B_IN2_Pin, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(&htim3, MOTOR_B_PWM_CHANNEL, -speed);
    }
    else // 刹车
    {
        HAL_GPIO_WritePin(MOTOR_B_IN1_GPIO_Port, MOTOR_B_IN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MOTOR_B_IN2_GPIO_Port, MOTOR_B_IN2_Pin, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim3, MOTOR_B_PWM_CHANNEL, 0);
    }
}

void TB6612_Enable(void)
{
    HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin, GPIO_PIN_SET);
}

void TB6612_Disable(void)
{
    HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin, GPIO_PIN_RESET);
}

void TB6612_StopAll(void)
{
    TB6612_MotorA_SetSpeed(0);
    TB6612_MotorB_SetSpeed(0);
}