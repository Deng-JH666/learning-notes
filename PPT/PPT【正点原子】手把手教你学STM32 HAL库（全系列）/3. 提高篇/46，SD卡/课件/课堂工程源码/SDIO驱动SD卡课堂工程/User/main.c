#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./USMART/usmart.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SRAM/sram.h"
#include "./MALLOC/malloc.h"
#include "./BSP/SDIO/sdio_sdcard.h"


int main(void)
{
    uint8_t key;
    uint8_t t = 0;
    uint8_t operation_ret = 0;
    uint8_t rec_buf[512];
    uint8_t send_buf[512];
    
    HAL_Init();                             /* ��ʼ��HAL�� */
    sys_stm32_clock_init(RCC_PLL_MUL9);     /* ����ʱ��, 72Mhz */
    delay_init(72);                         /* ��ʱ��ʼ�� */
    usart_init(115200);                     /* ���ڳ�ʼ��Ϊ115200 */
    usmart_dev.init(72);                    /* ��ʼ��USMART */
    led_init();                             /* ��ʼ��LED */
    lcd_init();                             /* ��ʼ��LCD */
    key_init();                             /* ��ʼ������ */
    sram_init();                            /* SRAM��ʼ�� */
    my_mem_init(SRAMIN);                    /* ��ʼ���ڲ�SRAM�ڴ�� */
    my_mem_init(SRAMEX);                    /* ��ʼ���ⲿSRAM�ڴ�� */
    
    sd_init();                              /* SD����ʼ�� */


    while (1)
    {
        key = key_scan(0);

        if (key == KEY0_PRES)
        {
            operation_ret =  HAL_SD_ReadBlocks(&g_sdcard_handler, (uint8_t *)rec_buf, 0, 1, SD_TIMEOUT);
            
            for (uint16_t i = 0; i < 512; i++)
            {
                printf("%x ", rec_buf[i]);
            }
            printf("READ DATA OK \r\n");
        }
        
        if (key == KEY1_PRES)
        {
            for (uint16_t i = 0; i < 512; i++)
            {
                send_buf[i] = i;
                printf("%x ", send_buf[i]);
            }
            
            operation_ret =  HAL_SD_WriteBlocks(&g_sdcard_handler, (uint8_t *)send_buf, 0, 1, SD_TIMEOUT);
            printf("SEND DATA OK \r\n");
        }
        
        t++;
        delay_ms(10);

        if (t == 20)
        {
            LED0_TOGGLE();
            t = 0;
        }
    }
}






























