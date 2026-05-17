/**
 * @file dht11.c
 * @brief 健壮的DHT11温湿度传感器驱动。
 * @version 2.0
 * @date 2025-09-04
 * @license MIT License
 *
 * @attention
 * v2.0更新:
 * - 为所有等待引脚电平变化的while循环增加了超时机制，防止程序在传感器无响应时卡死。
 * - 明确了DHT11_Measure函数的错误返回值 (HAL_TIMEOUT, HAL_ERROR)。
 */
#include "dht11.h"

// ========================== 用户配置 ==========================
// DHT11数据引脚定义
#define DHT11_PORT       DHT11_DATA_GPIO_Port // 使用CubeMX中生成的宏定义
#define DHT11_PIN        DHT11_DATA_Pin       // 使用CubeMX中生成的宏定义

// 用于微秒延迟的定时器句柄 (请确保与您在CubeMX中配置的定时器一致)
extern TIM_HandleTypeDef htim1;

// 等待传感器响应的超时时间 (单位: 微秒)
#define DHT11_TIMEOUT_US 100
// =============================================================

// 温湿度数据暂存变量
static float Temperature;
static float Humidity;

/**
 * @brief 微秒级延迟
 */
static void delay_us(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim1, 0);
    while (__HAL_TIM_GET_COUNTER(&htim1) < us);
}

/**
 * @brief 设置GPIO引脚为输出模式
 */
static void DHT11_Set_Pin_Output(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

/**
 * @brief 设置GPIO引脚为输入模式
 */
static void DHT11_Set_Pin_Input(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL; // 通常外部已有上拉，或模块自带
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

/**
 * @brief 从DHT11读取一个字节 (8位) 数据
 * @param timeout_ms 指向超时状态的指针，如果发生超时则会被置1
 * @return 读取到的字节
 */
static uint8_t DHT11_Read_Byte(uint8_t* timeout_flag)
{
    uint8_t i, byte = 0;
    uint16_t timeout_counter;

    for (i = 0; i < 8; i++)
    {
        // 等待数据位开始信号 (低电平) 结束
        timeout_counter = 0;
        while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET)
        {
            delay_us(1);
            if (++timeout_counter > DHT11_TIMEOUT_US)
            {
                *timeout_flag = 1;
                return 0;
            }
        }
        
        // 延时30us后检测电平，如果为高电平则该位为1
        delay_us(30);
        if (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET)
        {
            byte |= (1 << (7 - i));
        }

        // 等待该数据位的高电平结束
        timeout_counter = 0;
        while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET)
        {
            delay_us(1);
            if (++timeout_counter > DHT11_TIMEOUT_US)
            {
                *timeout_flag = 1;
                return 0;
            }
        }
    }
    return byte;
}

void DHT11_Init()
{
    HAL_TIM_Base_Start(&htim1);
    // 初始时拉高总线，并延时等待传感器稳定
    DHT11_Set_Pin_Output();
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
    HAL_Delay(1000);
}

HAL_StatusTypeDef DHT11_Measure()
{
    uint8_t data[5] = {0};
    uint16_t timeout_counter = 0;
    uint8_t read_timeout_flag = 0;

    // 1. 主机发送起始信号
    DHT11_Set_Pin_Output();
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET); // 拉低总线
    HAL_Delay(20); // 至少18ms
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET); // 拉高总线
    delay_us(30);
    DHT11_Set_Pin_Input();

    // 2. 等待传感器响应信号
    // 等待传感器拉低总线
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET)
    {
        delay_us(1);
        if (++timeout_counter > DHT11_TIMEOUT_US) return HAL_TIMEOUT;
    }

    // 等待传感器拉高总线
    timeout_counter = 0;
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET)
    {
        delay_us(1);
        if (++timeout_counter > DHT11_TIMEOUT_US) return HAL_TIMEOUT;
    }

    // 等待传感器再次拉低总线，准备开始发送数据
    timeout_counter = 0;
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET)
    {
        delay_us(1);
        if (++timeout_counter > DHT11_TIMEOUT_US) return HAL_TIMEOUT;
    }

    // 3. 接收40位数据 (5个字节)
    for (int i = 0; i < 5; i++)
    {
        data[i] = DHT11_Read_Byte(&read_timeout_flag);
        if (read_timeout_flag) return HAL_TIMEOUT;
    }
    
    // 4. 校验和检查
    if (data[4] == (uint8_t)(data[0] + data[1] + data[2] + data[3]))
    {
        // 校验成功，更新数据
        Humidity = (float)data[0] + ((float)data[1] / 10.0f);
        Temperature = (float)data[2] + ((float)data[3] / 10.0f);
        return HAL_OK;
    }
    else
    {
        // 校验失败
        return HAL_ERROR;
    }
}

float DHT11_Temperature()
{
    return Temperature;
}

float DHT11_Humidity()
{
    return Humidity;
}