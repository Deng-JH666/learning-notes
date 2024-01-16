/**
 ****************************************************************************************************
 * @file        btim.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-14
 * @brief       ������ʱ�� ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32H750������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20220105
 * ��һ�η���
 * V1.2 20220105
 * ����btim_tim7_int_init����
 *
 ****************************************************************************************************
 */

#include "./BSP/LED/led.h"
#include "./BSP/TIMER/btim.h"
#include "./SYSTEM/usart/usart.h"


TIM_HandleTypeDef g_timx_handle;      /* ��ʱ��x��� */
TIM_HandleTypeDef g_tim7_handle;      /* ��ʱ��7��� */

//extern uint8_t g_ov_frame;            /* ��dcmi.c���涨�� */

extern uint8_t g_frame;     /* videoplayer.c���涨�� */
extern uint8_t g_frameup;   /* videoplayer.c���涨�� */

/**
 * @brief       ������ʱ��TIMX��ʱ�жϳ�ʼ������
 * @note
 *              ������ʱ����ʱ������APB1,��D2PPRE1��2��Ƶ��ʱ��
 *              ������ʱ����ʱ��ΪAPB1ʱ�ӵ�2��, ��APB1Ϊ120M, ���Զ�ʱ��ʱ�� = 240Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ��
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void btim_timx_int_init(uint16_t arr, uint16_t psc)
{
    g_timx_handle.Instance = BTIM_TIMX_INT;                      /* ͨ�ö�ʱ��x */
    g_timx_handle.Init.Prescaler = psc;                          /* ��Ƶ */
    g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;         /* ��������ģʽ */
    g_timx_handle.Init.Period = arr;                             /* �Զ�װ��ֵ */
    HAL_TIM_Base_Init(&g_timx_handle);

    HAL_TIM_Base_Start_IT(&g_timx_handle);   /* ʹ�ܶ�ʱ��x�Ͷ�ʱ��x�����ж� */
}

/**
 * @brief       ������ʱ��TIM7��ʱ�жϳ�ʼ������
 * @note
 *              ������ʱ����ʱ������APB1,��D2PPRE1��2��Ƶ��ʱ��
 *              ������ʱ����ʱ��ΪAPB1ʱ�ӵ�2��, ��APB1Ϊ120M, ���Զ�ʱ��ʱ�� = 240Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ��
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void btim_tim7_int_init(uint16_t arr, uint16_t psc)
{
    __HAL_RCC_TIM7_CLK_ENABLE();                                     /* ʹ��TIM7ʱ�� */
    
    g_tim7_handle.Instance = TIM7;                                   /* ͨ�ö�ʱ��7 */
    g_tim7_handle.Init.Prescaler = psc;                              /* ��Ƶ */
    g_tim7_handle.Init.CounterMode = TIM_COUNTERMODE_UP;             /* ���ϼ����� */
    g_tim7_handle.Init.Period = arr;                                 /* �Զ�װ��ֵ */
    HAL_TIM_Base_Init(&g_tim7_handle);
    
    HAL_NVIC_SetPriority(TIM7_IRQn, 1, 3);  /* �����ж����ȼ�����ռ���ȼ�1�������ȼ�3 */
    HAL_NVIC_EnableIRQ(TIM7_IRQn);          /* ����ITM7�ж� */
    
    HAL_TIM_Base_Start_IT(&g_tim7_handle);  /* ʹ�ܶ�ʱ��x�Ͷ�ʱ��x�����ж� */
}

/**
 * @brief       ��ʱ���ײ�����������ʱ�ӣ������ж����ȼ�
                �˺����ᱻHAL_TIM_Base_Init()��������
 * @param       ��
 * @retval      ��
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BTIM_TIMX_INT)
    {
        BTIM_TIMX_INT_CLK_ENABLE();                      /* ʹ��TIMxʱ�� */
        HAL_NVIC_SetPriority(BTIM_TIMX_INT_IRQn, 1, 3);  /* �����ж����ȼ�����ռ���ȼ�1�������ȼ�3 */
        HAL_NVIC_EnableIRQ(BTIM_TIMX_INT_IRQn);          /* ����ITMx�ж� */
    }
}

/**
 * @brief       ��ʱ���жϷ�����
 * @param       ��
 * @retval      ��
 */
void BTIM_TIMX_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_handle);
}

/**
 * @brief       ��ʱ���жϷ�����
 * @param       ��
 * @retval      ��
 */
void TIM7_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim7_handle);
}

/**
 * @brief       ��ʱ�������жϻص�����
 * @param       htim:��ʱ�����ָ��
 * @retval      ��
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BTIM_TIMX_INT)
    {
        printf("frame:%d\r\n", g_frame);  /* ��ӡ֡�� */
        g_frame = 0;
    }
	else if (htim->Instance == TIM7)
    {
        g_frameup = 1;
    }
}










