#include "string.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/SDIO/sdio_sdcard.h"

SD_HandleTypeDef g_sdcard_handler;              /* SD����� */

/**
 * @brief       ��ʼ��SD��
 * @param       ��
 * @retval      ����ֵ:0 ��ʼ����ȷ������ֵ����ʼ������
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
 * @brief       SDIO�ײ�������ʱ��ʹ�ܣ���������
                �˺����ᱻHAL_SD_Init()����
 * @param       hsd:SD�����
 * @retval      ��
 */
void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
    GPIO_InitTypeDef gpio_init_struct;

    __HAL_RCC_SDIO_CLK_ENABLE();    /* ʹ��SDIOʱ�� */
    SD_D0_GPIO_CLK_ENABLE();        /* D0����IOʱ��ʹ�� */
    SD_D1_GPIO_CLK_ENABLE();        /* D1����IOʱ��ʹ�� */
    SD_D2_GPIO_CLK_ENABLE();        /* D2����IOʱ��ʹ�� */
    SD_D3_GPIO_CLK_ENABLE();        /* D3����IOʱ��ʹ�� */
    SD_CLK_GPIO_CLK_ENABLE();       /* CLK����IOʱ��ʹ�� */
    SD_CMD_GPIO_CLK_ENABLE();       /* CMD����IOʱ��ʹ�� */

    gpio_init_struct.Pin = SD_D0_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;            /* ���츴�� */
    gpio_init_struct.Pull = GPIO_PULLUP;                /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;      /* ���� */
    HAL_GPIO_Init(SD_D0_GPIO_PORT, &gpio_init_struct);  /* ��ʼ��D0���� */

    gpio_init_struct.Pin = SD_D1_GPIO_PIN;
    HAL_GPIO_Init(SD_D1_GPIO_PORT, &gpio_init_struct);  /* ��ʼ��D1���� */
    
    gpio_init_struct.Pin = SD_D2_GPIO_PIN;
    HAL_GPIO_Init(SD_D2_GPIO_PORT, &gpio_init_struct);  /* ��ʼ��D2���� */

    gpio_init_struct.Pin = SD_D3_GPIO_PIN;
    HAL_GPIO_Init(SD_D3_GPIO_PORT, &gpio_init_struct);  /* ��ʼ��D3���� */

    gpio_init_struct.Pin = SD_CLK_GPIO_PIN;
    HAL_GPIO_Init(SD_CLK_GPIO_PORT, &gpio_init_struct); /* ��ʼ��CLK���� */

    gpio_init_struct.Pin = SD_CMD_GPIO_PIN;
    HAL_GPIO_Init(SD_CMD_GPIO_PORT, &gpio_init_struct); /* ��ʼ��CMD���� */
}
