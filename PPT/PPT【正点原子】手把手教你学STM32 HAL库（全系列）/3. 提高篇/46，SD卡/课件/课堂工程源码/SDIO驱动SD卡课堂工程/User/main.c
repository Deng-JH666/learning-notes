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
    
    HAL_Init();                             /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9);     /* 设置时钟, 72Mhz */
    delay_init(72);                         /* 延时初始化 */
    usart_init(115200);                     /* 串口初始化为115200 */
    usmart_dev.init(72);                    /* 初始化USMART */
    led_init();                             /* 初始化LED */
    lcd_init();                             /* 初始化LCD */
    key_init();                             /* 初始化按键 */
    sram_init();                            /* SRAM初始化 */
    my_mem_init(SRAMIN);                    /* 初始化内部SRAM内存池 */
    my_mem_init(SRAMEX);                    /* 初始化外部SRAM内存池 */
    
    sd_init();                              /* SD卡初始化 */


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






























