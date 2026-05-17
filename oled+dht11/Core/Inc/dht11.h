#ifndef __DHT11_H__
#define __DHT11_H__

#include "main.h" // 引入main.h以使用CubeMX生成的宏定义和HAL库

/**
 * @brief 初始化DHT11传感器。
 * @note 此函数应在GPIO和用于微秒延迟的定时器初始化之后调用。
 */
void DHT11_Init(void);

/**
 * @brief 从DHT11传感器测量温度和湿度。
 * @note 这是一个健壮的版本，包含了超时机制以防止程序卡死。
 * @return HAL_StatusTypeDef:
 * - HAL_OK: 测量成功且校验和正确。
 * - HAL_TIMEOUT: 传感器响应超时，请检查接线。
 * - HAL_ERROR: 校验和错误，数据传输可能被干扰。
 */
HAL_StatusTypeDef DHT11_Measure(void);

/**
 * @brief 获取最后一次成功测量的温度。
 * @return 温度，单位为摄氏度。
 */
float DHT11_Temperature(void);

/**
 * @brief 获取最后一次成功测量的湿度。
 * @return 相对湿度，单位为百分比。
 */
float DHT11_Humidity(void);

#endif /* __DHT11_H__ */