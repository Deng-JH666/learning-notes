/**
 ****************************************************************************************************
 * @file        pwr.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.3
 * @date        2020-03-26
 * @brief       �͹���ģʽ ��������
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
 * V1.0 20200325
 * ��һ�η���
 * V1.1 20200325
 * ֧�ֽ���˯��ģʽ������
 * ����pwr_wkup_key_init��pwr_enter_sleep����
 * V1.2 20200325
 * ֧�ֽ���ֹͣģʽ������
 * ����pwr_enter_stop����
 * V1.3 20200326
 * ֧�ֽ������ģʽ������
 * ����pwr_enter_standby����
 *
 ****************************************************************************************************
 */

#include "./BSP/PWR/pwr.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"


/**
 * @brief       ��ʼ��PVD��ѹ������
 * @param       pls: ��ѹ�ȼ�
 *   @arg       000,1.95V;  001,2.1V
 *   @arg       010,2.25V;  011,2.4V;
 *   @arg       100,2.55V;  101,2.7V;
 *   @arg       110,2.85V;  111,ʹ��PVD_IN���ϵĵ�ѹ(��Vrefint�Ƚ�)
 * @retval      ��
 */
void pwr_pvd_init(uint8_t pls)
{
    PWR->CR1 &= ~(3 << 5);      /* PLS[2:0]���� */
    PWR->CR1 |=  pls << 5;      /* PLS[2:0] = pls,ע�ⲻҪ����Χ! */
    PWR->CR1 |= 1 << 4;         /* PVDE = 1,ʹ��PVD��� */

    EXTI_D1->IMR1 |= 1 << 16;   /* ����line16 �ϵ��ж�(PVD & AVD) */
    EXTI->FTSR1 |= 1 << 16;     /* line16 ���¼��½��ش��� */
    EXTI->RTSR1 |= 1 << 16;     /* line16 ���¼��������ش��� */

    sys_nvic_init(3, 3, PVD_AVD_IRQn, 2); /* ��2��������ȼ� */
}

/**
 * @brief       PVD/AVD�жϷ�����
 * @param       ��
 * @retval      ��
 */
void PVD_AVD_IRQHandler(void)
{
    if (PWR->CSR1 & (1 << 4))   /* ��ѹ��PLS��ѡ��ѹ���� */
    {
        lcd_show_string(30, 130, 200, 16, 16, "PVD Low Voltage!", RED); /* LCD��ʾ��ѹ�� */
        LED1(0);                                                        /* �����̵�, ������ѹ���� */
    }
    else
    {
        lcd_show_string(30, 130, 200, 16, 16, "PVD Voltage OK! ", BLUE);/* LCD��ʾ��ѹ���� */
        LED1(1);                                                        /* ����̵� */
    }

    EXTI->PR1 |= 1 << 16;   /* ���line16���жϱ�־ */
}

/**
 * @brief       WK_UP���� �ⲿ�жϷ������
 * @param       ��
 * @retval      ��
 */
void PWR_WKUP_INT_IRQHandler(void)
{
    EXTI_D1->PR1 = PWR_WKUP_GPIO_PIN;   /* ���WKUP�����ж��� ���жϱ�־λ */
}

/**
 * @brief       �͹���ģʽ�µİ�����ʼ��(���ڻ���˯��ģʽ/ֹͣģʽ)
 * @param       ��
 * @retval      ��
 */
void pwr_wkup_key_init(void)
{
    PWR_WKUP_GPIO_CLK_ENABLE();     /* WKUPʱ��ʹ�� */

    sys_gpio_set(PWR_WKUP_GPIO_PORT, PWR_WKUP_GPIO_PIN,
                 SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PD);    /* WKUP����ģʽ����,�������� */

    sys_nvic_ex_config(PWR_WKUP_GPIO_PORT, PWR_WKUP_GPIO_PIN, SYS_GPIO_RTIR);   /* WKUP����Ϊ�����ش����ж� */ 
    sys_nvic_init( 2, 2, PWR_WKUP_INT_IRQn, 2); /* ��ռ2�������ȼ�2����2 */
}

/**
 * @brief       ����CPU˯��ģʽ
 * @param       ��
 * @retval      ��
 */
void pwr_enter_sleep(void)
{
    EXTI_D1->PR1 = PWR_WKUP_GPIO_PIN;   /* ���WKUP�ϵ��жϱ�־λ */ 
    sys_wfi_set();  /* ִ��WFIָ��, �������ģʽ */
}

/**
 * @brief       ����ֹͣģʽ
 * @param       ��
 * @retval      ��
 */
void pwr_enter_stop(void)
{
    EXTI_D1->PR1 = PWR_WKUP_GPIO_PIN;   /* ���WKUP�ϵ��жϱ�־λ */

    /* �ر�VOS0
     * 480M�汾H750оƬ��V�汾������Scale0����,�ڽ���ֹͣ/����ģʽ֮ǰ,�������˳�Scale0ģʽ���� 
     */
    sys_clock_set(200, 2, 2, 4);/* ����ʱ��,400Mhz����Ƶ */
    RCC->APB4ENR |= 1 << 1;     /* ʹ��SYSCFGENλ */
    SYSCFG->PWRCR &= ~(1 << 0); /* ����ODENλΪ0,�ر�Overdrive,��ʱVCORE=1.2V */
    
    while ((PWR->D3CR & (1 << 13)) == 0);   /* �ȴ���ѹ�ȶ� */

    PWR->CR1 |= 1 << 0;         /* ��SVOS3����ֹͣģʽʱ, ��ѹ�����ڵ͹���ģʽ. */
    
    /* ȷ������ָ������ */
    __DSB ();
    __ISB ();

    SCB->SCR |= 1 << 2;         /* ʹ��SLEEPDEEPλ */
    PWR->CPUCR &= ~(7 << 0);    /* PDDS_D1/D2/D3 = 0, ����D1/D2/D3�������˯�ߺ�,����ֹͣģʽ(PDDS=0) */

    sys_wfi_set();              /* ִ��WFIָ��, ����ֹͣģʽ */
    
    SCB->SCR &= ~(1 << 2);      /* �ر�SLEEPDEEPλ */
}

/**
 * @brief       �������ģʽ
 * @param       ��
 * @retval      ��
 */
void pwr_enter_standby(void)
{
    uint32_t tempreg;   /* ��ʱ�洢�Ĵ���ֵ�� */

    EXTI_D1->PR1 = PWR_WKUP_GPIO_PIN;   /* ���WKUP�ϵ��жϱ�־λ */

    /* STM32F4/F7/H7,��������RTC����жϺ�,�����ȹر�RTC�ж�,�����жϱ�־λ,Ȼ����������
     * RTC�ж�,�ٽ������ģʽ�ſ�����������,�����������.
     */
    PWR->CR1 |= 1 << 8;         /* ������дʹ�� */
    /* �ر�RTC�Ĵ���д���� */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;
    tempreg = RTC->CR & (0X0F << 12);   /* ��¼ԭ����RTC�ж����� */
    RTC->CR &= ~(0XF << 12);    /* �ر�RTC�����ж� */
    RTC->ISR &= ~(0X3F << 8);   /* �������RTC�жϱ�־. */
    RTC->CR |= tempreg;         /* ��������RTC�ж� */
    RTC->WPR = 0xFF;            /* ʹ��RTC�Ĵ���д���� */
    
    /* �ر�VOS0
     * 480M�汾H750оƬ��V�汾������Scale0����,�ڽ���˯��ģʽ֮ǰ,�������˳�Scale0ģʽ���� 
     */
    sys_clock_set(200, 2, 2, 4);    /* ����ʱ��,400Mhz����Ƶ */
    RCC->APB4ENR |= 1 << 1;         /* ʹ��SYSCFGENλ */
    SYSCFG->PWRCR &= ~(1 << 0);     /* ����ODENλΪ0,�ر�Overdrive,��ʱVCORE=1.2V */
    while ((PWR->D3CR & (1 << 13)) == 0);   /* �ȴ���ѹ�ȶ� */
    
    sys_standby();  /* �������ģʽ */
}










