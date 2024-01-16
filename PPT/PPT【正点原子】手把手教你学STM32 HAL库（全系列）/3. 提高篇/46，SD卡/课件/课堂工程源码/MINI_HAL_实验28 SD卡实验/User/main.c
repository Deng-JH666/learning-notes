/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-06-02
 * @brief       SD�� ʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
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
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./MALLOC/malloc.h"
#include "./BSP/SDMMC/spi_sdcard.h"


/**
 * @brief       ����SD���Ķ�ȡ
 *   @note      ��secaddr��ַ��ʼ,��ȡseccnt������������
 * @param       secaddr : ������ַ
 * @param       seccnt  : ������
 * @retval      ��
 */
void sd_test_read(uint32_t secaddr, uint32_t seccnt)
{
    uint32_t i;
    uint8_t *buf;
    uint8_t sta = 0;
    
    buf = mymalloc(SRAMIN, seccnt * 512);       /* �����ڴ�,��SRAM�����ڴ� */
    sta = sd_read_disk(buf, secaddr, seccnt);   /* ��ȡsecaddr������ʼ������ */

    if (sta == 0)
    {
        printf("SECTOR %d DATA:\r\n", secaddr);

        for (i = 0; i < seccnt * 512; i++)
        {
            printf("%x ", buf[i]);  /* ��ӡsecaddr��ʼ���������� */
        }
        
        printf("\r\nDATA ENDED\r\n");
    }
    else
    {
        printf("err:%d\r\n", sta);
    }

    myfree(SRAMIN, buf);    /* �ͷ��ڴ� */
}

/**
 * @brief       ����SD����д��
 *   @note      ��secaddr��ַ��ʼ,д��seccnt������������
 *              ����!! ���дȫ��0XFF������,���������SD��.
 *
 * @param       secaddr : ������ַ
 * @param       seccnt  : ������
 * @retval      ��
 */
void sd_test_write(uint32_t secaddr, uint32_t seccnt)
{
    uint32_t i;
    uint8_t *buf;
    uint8_t sta = 0;
    
    buf = mymalloc(SRAMIN, seccnt * 512);       /* ��SRAM�����ڴ� */

    for (i = 0; i < seccnt * 512; i++)          /* ��ʼ��д�������,��3�ı���. */
    {
        buf[i] = i * 3;
    }

    sta = sd_write_disk(buf, secaddr, seccnt);  /* ��secaddr������ʼд��seccnt���������� */

    if (sta == 0)
    {
        printf("Write over!\r\n");
    }
    else printf("err:%d\r\n", sta);

    myfree(SRAMIN, buf);    /* �ͷ��ڴ� */
}

int main(void)
{
    uint8_t key;
    uint32_t sd_size;
    uint8_t t = 0;
    uint8_t *buf;

    HAL_Init();                             /* ��ʼ��HAL�� */
    sys_stm32_clock_init(RCC_PLL_MUL9);     /* ����ʱ��, 72Mhz */
    delay_init(72);                         /* ��ʱ��ʼ�� */
    usart_init(115200);                     /* ���ڳ�ʼ��Ϊ115200 */
    usmart_dev.init(72);                    /* ��ʼ��USMART */
    led_init();                             /* ��ʼ��LED */
    lcd_init();                             /* ��ʼ��LCD */
    key_init();                             /* ��ʼ������ */
    my_mem_init(SRAMIN);                    /* ��ʼ���ڲ�SRAM�ڴ�� */

    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "SD TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "KEY0:Read Sector 0", RED);

    while (sd_init())   /* ��ⲻ��SD�� */
    {
        lcd_show_string(30, 130, 200, 16, 16, "SD Card Error!", RED);
        delay_ms(500);
        lcd_show_string(30, 130, 200, 16, 16, "Please Check! ", RED);
        delay_ms(500);
        LED0_TOGGLE();  /* �����˸ */
    }
    
    /* ���SD���ɹ� */
    lcd_show_string(30, 130, 200, 16, 16, "SD Card OK    ", BLUE);
    lcd_show_string(30, 150, 200, 16, 16, "SD Card Size:     MB", BLUE);

    sd_size = sd_get_sector_count();  /* �õ������� */
    lcd_show_num(30 + 13 * 8, 150, sd_size >> 11, 5, 16, BLUE); /* ��ʾSD������, ת����MB��λ */

    while (1)
    {
        key = key_scan(0);

        if (key == KEY0_PRES)   /* KEY0������ */
        {
            buf = mymalloc(SRAMIN, 512);    /* �����ڴ� */
            key = sd_read_disk(buf, 0, 1);

            if (key == 0)       /* ��ȡ0���������� */
            {
                lcd_show_string(30, 170, 200, 16, 16, "USART1 Sending Data...", BLUE);
                printf("SECTOR 0 DATA:\r\n");

                for (sd_size = 0; sd_size < 512; sd_size++)
                {
                    printf("%x ", buf[sd_size]);    /* ��ӡ0�������� */
                }
                
                printf("\r\nDATA ENDED\r\n");
                lcd_show_string(30, 170, 200, 16, 16, "USART1 Send Data Over!", BLUE);
            }
            else
            {
                printf("err:%d\r\n", key);
            }

            myfree(0, buf); /* �ͷ��ڴ� */
        }
        
        if (key == KEY1_PRES)
        {
            sd_test_write(0, 1);
        }

        t++;
        delay_ms(10);

        if (t == 20)
        {
            LED0_TOGGLE();  /* �����˸ */
            t = 0;
        }
    }
}


































