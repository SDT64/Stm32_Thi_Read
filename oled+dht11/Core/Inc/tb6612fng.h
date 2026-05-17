#ifndef __TB6612FNG_H__
#define __TB6612FNG_H__

#include "main.h"

/**
 * @brief 初始化TB6612FNG驱动
 * @note  此函数会启动PWM通道并使能驱动器
 */
void TB6612_Init(void);

/**
 * @brief 设置电机A的转速
 * @param speed 速度值，范围 [-999, 999]。
 * - 正值: 正转
 * - 负值: 反转
 * - 0: 刹车
 */
void TB6612_MotorA_SetSpeed(int16_t speed);

/**
 * @brief 设置电机B的转速
 * @param speed 速度值，范围 [-999, 999]。
 * - 正值: 正转
 * - 负值: 反转
 * - 0: 刹车
 */
void TB6612_MotorB_SetSpeed(int16_t speed);

/**
 * @brief 使能驱动器 (退出待机模式)
 */
void TB6612_Enable(void);

/**
 * @brief 禁用驱动器 (进入待机模式，省电)
 */
void TB6612_Disable(void);

/**
 * @brief 停止所有电机 (刹车)
 */
void TB6612_StopAll(void);

#endif /* __TB6612FNG_H__ */