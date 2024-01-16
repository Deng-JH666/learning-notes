/**
 ****************************************************************************************************
 * @file        gtim.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.4
 * @date        2020-03-14
 * @brief       ͨ�ö�ʱ�� ��������
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
 * V1.0 20200314
 * ��һ�η���
 * V1.1 20200317
 * ����gtim_timx_pwm_chy_init����
 * V1.2 20200318
 * 1,����gtim_timx_cap_chy_init����
 * V1.3 20200329
 * 1,֧���ⲿ�����������
 * 2,����gtim_timx_cnt_chy_init,gtim_timx_cnt_chy_get_count��gtim_timx_cnt_chy_restart��������
 * V1.4 20200329
 * 1,����gtim_timx_npwm_chy_init����
 * 2,����gtim_timx_npwm_chy_set����
 *
 ****************************************************************************************************
 */

#ifndef __GTIM_H
#define __GTIM_H

#include "./SYSTEM/sys/sys.h"

/******************************************************************************************/
/* ͨ�ö�ʱ�� ���� */

/* TIMX �ж϶���
 * Ĭ�������TIM2~TIM5, TIM12~TIM17.
 * ע��: ͨ���޸���4���궨��,����֧��TIM1~TIM17����һ����ʱ��.
 */
#define GTIM_TIMX_INT                       TIM3
#define GTIM_TIMX_INT_IRQn                  TIM3_IRQn
#define GTIM_TIMX_INT_IRQHandler            TIM3_IRQHandler
#define GTIM_TIMX_INT_CLK_ENABLE()          do{ RCC->APB1LENR |= 1 << 1; }while(0)      /* TIM3 ʱ��ʹ�� */

/* TIMX PWM�������
 * ���������PWM����LED0(RED)������
 * Ĭ�������TIM2~TIM5, TIM12~TIM17.
 * ע��: ͨ���޸���8���궨��,����֧��TIM1~TIM17����һ����ʱ��,����һ��IO�����PWM
 */
#define GTIM_TIMX_PWM_CHY_GPIO_PORT         GPIOB
#define GTIM_TIMX_PWM_CHY_GPIO_PIN          SYS_GPIO_PIN14
#define GTIM_TIMX_PWM_CHY_GPIO_AF           2                           /* AF����ѡ�� */
#define GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE() do{ RCC->AHB4ENR |= 1 << 1; }while(0)       /* PB��ʱ��ʹ�� */

#define GTIM_TIMX_PWM                       TIM12
#define GTIM_TIMX_PWM_CHY                   1                           /* ͨ��Y,  1<= Y <=4 */
#define GTIM_TIMX_PWM_CHY_CCRX              TIM12->CCR1                  /* ͨ��Y������ȽϼĴ��� */
#define GTIM_TIMX_PWM_CHY_CLK_ENABLE()      do{ RCC->APB1LENR |= 1 << 6; }while(0)      /* TIM12 ʱ��ʹ�� */

///* ���������PWM����LED0(RED)������ */
//#define GTIM_TIMX_PWM_CHY_GPIO_PORT         GPIOB
//#define GTIM_TIMX_PWM_CHY_GPIO_PIN          SYS_GPIO_PIN4
//#define GTIM_TIMX_PWM_CHY_GPIO_AF           2                           /* AF����ѡ�� */
//#define GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE() do{ RCC->AHB4ENR |= 1 << 1; }while(0)       /* PB��ʱ��ʹ�� */

//#define GTIM_TIMX_PWM                       TIM3
//#define GTIM_TIMX_PWM_CHY                   1                           /* ͨ��Y,  1<= Y <=4 */
//#define GTIM_TIMX_PWM_CHY_CCRX              TIM3->CCR1                  /* ͨ��Y������ȽϼĴ��� */
//#define GTIM_TIMX_PWM_CHY_CLK_ENABLE()      do{ RCC->APB1LENR |= 1 << 1; }while(0)      /* TIM3 ʱ��ʹ�� */

///* ���������PWM����LED1(GREEN)������ */
//#define GTIM_TIMX_PWM_CHY_GPIO_PORT         GPIOE
//#define GTIM_TIMX_PWM_CHY_GPIO_PIN          SYS_GPIO_PIN6
//#define GTIM_TIMX_PWM_CHY_GPIO_AF           4                           /* AF����ѡ�� */
//#define GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE() do{ RCC->AHB4ENR |= 1 << 4; }while(0)     /* PE��ʱ��ʹ�� */

//#define GTIM_TIMX_PWM                       TIM15
//#define GTIM_TIMX_PWM_CHY                   2                           /* ͨ��Y,  1<= Y <=4 */
//#define GTIM_TIMX_PWM_CHY_CCRX              TIM15->CCR2                 /* ͨ��Y������ȽϼĴ��� */
//#define GTIM_TIMX_PWM_CHY_CLK_ENABLE()      do{ RCC->APB2ENR |= 1 << 16; }while(0)    /* TIM15 ʱ��ʹ�� */


///* ���������PWM����LED2(BLUE)������ */
//#define GTIM_TIMX_PWM_CHY_GPIO_PORT         GPIOE
//#define GTIM_TIMX_PWM_CHY_GPIO_PIN          SYS_GPIO_PIN5
//#define GTIM_TIMX_PWM_CHY_GPIO_AF           4                           /* AF����ѡ�� */
//#define GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE() do{ RCC->AHB4ENR |= 1 << 4; }while(0)     /* PE��ʱ��ʹ�� */

//#define GTIM_TIMX_PWM                       TIM15
//#define GTIM_TIMX_PWM_CHY                   1                           /* ͨ��Y,  1<= Y <=4 */
//#define GTIM_TIMX_PWM_CHY_CCRX              TIM15->CCR1                 /* ͨ��Y������ȽϼĴ��� */
//#define GTIM_TIMX_PWM_CHY_CLK_ENABLE()      do{ RCC->APB2ENR |= 1 << 16; }while(0)    /* TIM15 ʱ��ʹ�� */


/* TIMX ���ָ������PWM ����
* ���������PWM����LED2(BLUE)������,����һ�α�ʾһ��PWM��
* Ĭ�������TIM2~TIM5, TIM12~TIM17.
* ע��: ͨ���޸���9���궨��,����֧��TIM1~TIM17����һ����ʱ��,����һ��IO�����PWM
*/
#define GTIM_TIMX_NPWM_CHY_GPIO_PORT            GPIOE
#define GTIM_TIMX_NPWM_CHY_GPIO_PIN             SYS_GPIO_PIN5
#define GTIM_TIMX_NPWM_CHY_GPIO_AF              4                          /* AF����ѡ�� */
#define GTIM_TIMX_NPWM_CHY_GPIO_CLK_ENABLE()    do{ RCC->AHB4ENR |= 1 << 4; }while(0)   /* PE��ʱ��ʹ�� */

#define GTIM_TIMX_NPWM                          TIM15
#define GTIM_TIMX_NPWM_IRQn                     TIM15_IRQn
#define GTIM_TIMX_NPWM_IRQHandler               TIM15_IRQHandler
#define GTIM_TIMX_NPWM_CHY                      1                           /* ͨ��Y,  1<= Y <=4 */
#define GTIM_TIMX_NPWM_CHY_CCRX                 TIM15->CCR1                 /* ͨ��Y������ȽϼĴ��� */
#define GTIM_TIMX_NPWM_CHY_CLK_ENABLE()         do{ RCC->APB2ENR |= 1 << 16; }while(0)  /* TIMX ʱ��ʹ�� */

/* TIMX ���벶����
* ��������벶��ʹ�ö�ʱ��TIM5_CH1,����WK_UP����������
* Ĭ�������TIM2~TIM5, TIM12~TIM17.
* ע��: ͨ���޸���10���궨��,����֧��TIM1~TIM17����һ����ʱ��,����һ��IO�������벶��
*       �ر�Ҫע��:Ĭ���õ�PA0,���õ�����������!���������IO,��Ӧ����������ʽ/AF���ܵ�Ҳ�ø�!
*/
#define GTIM_TIMX_CAP_CHY_GPIO_PORT            GPIOA
#define GTIM_TIMX_CAP_CHY_GPIO_PIN             SYS_GPIO_PIN0
#define GTIM_TIMX_CAP_CHY_GPIO_AF              2                          /* AF����ѡ�� */
#define GTIM_TIMX_CAP_CHY_GPIO_CLK_ENABLE()    do{ RCC->AHB4ENR |= 1 << 0; }while(0)   /* PA��ʱ��ʹ�� */

#define GTIM_TIMX_CAP                          TIM5
#define GTIM_TIMX_CAP_IRQn                     TIM5_IRQn
#define GTIM_TIMX_CAP_IRQHandler               TIM5_IRQHandler
#define GTIM_TIMX_CAP_CHY                      1                            /* ͨ��Y,  1<= Y <=4 */
#define GTIM_TIMX_CAP_CHY_CCRX                 TIM5->CCR1                   /* ͨ��Y������ȽϼĴ��� */
#define GTIM_TIMX_CAP_CHY_CLK_ENABLE()         do{ RCC->APB1LENR |= 1 << 3; }while(0)   /* TIMX ʱ��ʹ�� */

/* TIMX �����������
* ������������ʹ�ö�ʱ��TIM2_CH1,����WK_UP����������
* Ĭ�������TIM2~TIM5, TIM12~TIM17.  ֻ��CH1��CH2ͨ�����������������, CH3/CH4��֧��!
* ע��: ͨ���޸���9���궨��,����֧��TIM1~TIM17����һ����ʱ��,CH1/CH2��ӦIO�����������
*       �ر�Ҫע��:Ĭ���õ�PA0,���õ�����������!���������IO,��Ӧ����������ʽ/AF���ܵ�Ҳ�ø�!
*/
#define GTIM_TIMX_CNT_CHY_GPIO_PORT            GPIOA
#define GTIM_TIMX_CNT_CHY_GPIO_PIN             SYS_GPIO_PIN0
#define GTIM_TIMX_CNT_CHY_GPIO_AF              1                          /* AF����ѡ�� */
#define GTIM_TIMX_CNT_CHY_GPIO_CLK_ENABLE()    do{ RCC->AHB4ENR |= 1 << 0; }while(0)   /* PA��ʱ��ʹ�� */

#define GTIM_TIMX_CNT                          TIM2
#define GTIM_TIMX_CNT_IRQn                     TIM2_IRQn
#define GTIM_TIMX_CNT_IRQHandler               TIM2_IRQHandler
#define GTIM_TIMX_CNT_CHY                      1                            /* ͨ��Y,  1<= Y <=2 */
#define GTIM_TIMX_CNT_CHY_CLK_ENABLE()         do{ RCC->APB1LENR |= 1 << 0; }while(0)   /* TIMX ʱ��ʹ�� */

/******************************************************************************************/

void gtim_timx_int_init(uint16_t arr, uint16_t psc);        /* ͨ�ö�ʱ�� ��ʱ�жϳ�ʼ������ */
void gtim_timx_pwm_chy_init(uint16_t arr, uint16_t psc);    /* ͨ�ö�ʱ�� PWM��ʼ������ */
void gtim_timx_npwm_chy_init(uint16_t arr, uint16_t psc);   /* ͨ�ö�ʱ�� ���ָ������PWM��ʼ������ */
void gtim_timx_npwm_chy_set(uint32_t npwm);                 /* ͨ�ö�ʱ�� �������PWM�ĸ��� */
void gtim_timx_cap_chy_init(uint16_t arr, uint16_t psc);    /* ͨ�ö�ʱ�� ���벶���ʼ������ */
void gtim_timx_cnt_chy_init(uint16_t psc);                  /* ͨ�ö�ʱ�� ���������ʼ������ */
uint32_t gtim_timx_cnt_chy_get_count(void);                 /* ͨ�ö�ʱ�� ��ȡ������� */
void gtim_timx_cnt_chy_restart(void);                       /* ͨ�ö�ʱ�� ���������� */
#endif

















