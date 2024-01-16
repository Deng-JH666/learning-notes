#include "string.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/SDIO/sdio_sdcard.h"

SD_HandleTypeDef g_sdcard_handler;              /* SD卡句柄 */

/**
 * @brief       初始化SD卡
 * @param       无
 * @retval      返回值:0 初始化正确；其他值，初始化错误
 */
uint8_t sd_init(void)
{
    uint8_t SD_Error;
    
    g_sdcard_handler.Instance = SDIO;
    g_sdcard_handler.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
    g_sdcard_handler.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
    g_sdcard_handler.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
    g_sdcard_handler.Init.BusWide = SDIO_BUS_WIDE_1B;
    g_sdcard_handler.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_ENABLE;
    g_sdcard_handler.Init.ClockDiv = SDIO_TRANSFER_CLK_DIV;

    SD_Error = HAL_SD_Init(&g_sdcard_handler);
    
    if (SD_Error != HAL_OK)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief       SDIO底层驱动，时钟使能，引脚配置
                此函数会被HAL_SD_Init()调用
 * @param       hsd:SD卡句柄
 * @retval      无
 */
void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
    GPIO_InitTypeDef gpio_init_struct;

    __HAL_RCC_SDIO_CLK_ENABLE();    /* 使能SDIO时钟 */
    SD_D0_GPIO_CLK_ENABLE();        /* D0引脚IO时钟使能 */
    SD_D1_GPIO_CLK_ENABLE();        /* D1引脚IO时钟使能 */
    SD_D2_GPIO_CLK_ENABLE();        /* D2引脚IO时钟使能 */
    SD_D3_GPIO_CLK_ENABLE();        /* D3引脚IO时钟使能 */
    SD_CLK_GPIO_CLK_ENABLE();       /* CLK引脚IO时钟使能 */
    SD_CMD_GPIO_CLK_ENABLE();       /* CMD引脚IO时钟使能 */

    gpio_init_struct.Pin = SD_D0_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;            /* 推挽复用 */
    gpio_init_struct.Pull = GPIO_PULLUP;                /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;      /* 高速 */
    HAL_GPIO_Init(SD_D0_GPIO_PORT, &gpio_init_struct);  /* 初始化D0引脚 */

    gpio_init_struct.Pin = SD_D1_GPIO_PIN;
    HAL_GPIO_Init(SD_D1_GPIO_PORT, &gpio_init_struct);  /* 初始化D1引脚 */
    
    gpio_init_struct.Pin = SD_D2_GPIO_PIN;
    HAL_GPIO_Init(SD_D2_GPIO_PORT, &gpio_init_struct);  /* 初始化D2引脚 */

    gpio_init_struct.Pin = SD_D3_GPIO_PIN;
    HAL_GPIO_Init(SD_D3_GPIO_PORT, &gpio_init_struct);  /* 初始化D3引脚 */

    gpio_init_struct.Pin = SD_CLK_GPIO_PIN;
    HAL_GPIO_Init(SD_CLK_GPIO_PORT, &gpio_init_struct); /* 初始化CLK引脚 */

    gpio_init_struct.Pin = SD_CMD_GPIO_PIN;
    HAL_GPIO_Init(SD_CMD_GPIO_PORT, &gpio_init_struct); /* 初始化CMD引脚 */
}
