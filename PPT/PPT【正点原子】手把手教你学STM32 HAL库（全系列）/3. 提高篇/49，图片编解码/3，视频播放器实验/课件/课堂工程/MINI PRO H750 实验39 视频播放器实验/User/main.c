/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-05
 * @brief       ��Ƶ������ ʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� Mini Pro H750������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./USMART/usmart.h"
#include "./MALLOC/malloc.h"
#include "./FATFS/exfuns/exfuns.h"
#include "./TEXT/text.h"
#include "./PICTURE/piclib.h"
#include "./BSP/MPU/mpu.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SDMMC/sdmmc_sdcard.h"
#include "./BSP/TIMER/btim.h"
#include "./APP/videoplayer.h"


int main(void)
{
    sys_cache_enable();                 /* ��L1-Cache */
    HAL_Init();                         /* ��ʼ��HAL�� */
    sys_stm32_clock_init(240, 2, 2, 4); /* ����ʱ��, 480Mhz */
    delay_init(480);                    /* ��ʱ��ʼ�� */
    usart_init(115200);                 /* ���ڳ�ʼ��Ϊ115200 */
    usmart_dev.init(240);               /* ��ʼ��USMART */
    mpu_memory_protection();            /* ������ش洢���� */
    led_init();                         /* ��ʼ��LED */
    lcd_init();                         /* ��ʼ��LCD */
    key_init();                         /* ��ʼ������ */
    
    my_mem_init(SRAMIN);                /* ��ʼ���ڲ��ڴ��(AXI) */
    my_mem_init(SRAM12);                /* ��ʼ��SRAM12�ڴ��(SRAM1+SRAM2) */
    my_mem_init(SRAM4);                 /* ��ʼ��SRAM4�ڴ��(SRAM4) */
    my_mem_init(SRAMDTCM);              /* ��ʼ��DTCM�ڴ��(DTCM) */
    my_mem_init(SRAMITCM);              /* ��ʼ��ITCM�ڴ��(ITCM) */
    
    exfuns_init();                      /* Ϊfatfs��ر��������ڴ� */
    f_mount(fs[0], "0:", 1);            /* ����SD�� */
    f_mount(fs[1], "1:", 1);            /* ����FLASH */

    lcd_display_dir(1);                 /* ���óɺ��� */
    
    while (fonts_init())                /* ����ֿ� */
    {
        lcd_show_string(30, 50, 200, 16, 16, "Font Error!", RED);
        delay_ms(200);
        lcd_fill(30, 50, 240, 66, WHITE);   /* �����ʾ */
        delay_ms(200);
    }

    text_show_string(60, 50, 200, 16, "����ԭ��STM32������", 16, 0, RED);
    text_show_string(60, 70, 200, 16, "��Ƶ������ ʵ��", 16, 0, RED);
    text_show_string(60, 90, 200, 16, "KEY0:NEXT   KEY1:PREV", 16, 0, RED); 
    text_show_string(60, 110, 200, 16, "KEY_UP:FF", 16, 0, RED);
    delay_ms(1500);
    btim_timx_int_init(10000 - 1, 24000 - 1);   /* 10Khz����,1�����ж�һ�� */
    
    while(1)
    {
        video_play();
    }
}




