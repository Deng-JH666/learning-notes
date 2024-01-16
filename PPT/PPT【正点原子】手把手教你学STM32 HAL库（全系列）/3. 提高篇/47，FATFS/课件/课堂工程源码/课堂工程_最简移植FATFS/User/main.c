/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-28
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
#include "./BSP/SRAM/sram.h"
#include "./MALLOC/malloc.h"
#include "./BSP/SDIO/sdio_sdcard.h"

#include "ff.h"			/* Declarations of FatFs API */

void sd_test_fatfs(void)
{
    FRESULT ret;
    FATFS *fs_obj;
    FIL *fil_obj;
    uint8_t rd_buf[255] = {0};
    uint16_t fsize = 0;
    uint16_t rd_conut, wd_conut;
    
    fs_obj = mymalloc(SRAMIN, sizeof(FATFS));
    fil_obj = mymalloc(SRAMIN, sizeof(FIL));

    /* 1 ����SD���豸 */
    ret = f_mount(fs_obj, "0:", 1);
    if (ret)
    {
        printf("mount fail %d \r\n", ret);
    }
    else
    {
        printf("mount OK\r\n");
    }
    
    /* 2 ���ļ�(��Ҫע��Ҫ�ر��ļ�f_close) */
    ret = f_open(fil_obj, "0:sd_test_test_test_test_test_test.txt", FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
    if (ret)
    {
        printf("open fail %d \r\n", ret);
    }
    else
    {
        printf("open OK\r\n");
    }
    
    /* 3 ��ȡ���� */
    fsize = f_size(fil_obj);
    f_read(fil_obj, rd_buf, fsize, (UINT *)&rd_conut);
    if (rd_conut != 0)
    {
        printf("rd_conut:%d rd_buf:%s \r\n",rd_conut, rd_buf);
    }
    
    /* 4 ����д�� */            /* ע�⣺��дָ�� */
    f_write(fil_obj, "333", 3, (UINT *)&wd_conut);
    f_printf(fil_obj, "hello_world");
    
    f_lseek(fil_obj, 0);
    fsize = f_size(fil_obj);
    f_read(fil_obj, rd_buf, fsize, (UINT *)&rd_conut);
    if (rd_conut != 0)
    {
        printf("rd_conut:%d rd_buf:%s \r\n",rd_conut, rd_buf);
    }
    
    /* 5 �ر��ļ� */
    f_close(fil_obj);
}

uint8_t mf_scan_files(uint8_t * path)
{
    FRESULT res;    
    DIR *dir;
    FILINFO *fileinfo;
    
    dir = mymalloc(SRAMIN, sizeof(DIR));
    fileinfo = mymalloc(SRAMIN, sizeof(FILINFO));

    res = f_opendir(dir, (const TCHAR*)path); //��һ��Ŀ¼
    
    if (res == FR_OK) 
    {
        printf("\r\n"); 
        
        while(1)
        {
            res = f_readdir(dir, fileinfo);   /* ��ȡĿ¼�µ�һ���ļ� */
            if (res != FR_OK || fileinfo->fname[0] == 0) break; /* ������/��ĩβ��,�˳� */
            //if (fileinfo.fname[0] == '.') continue;           /* �����ϼ�Ŀ¼ */
            printf("%s/", path);                /* ��ӡ·�� */
            printf("%s\r\n",fileinfo->fname);   /* ��ӡ�ļ��� */
        } 
    }
    f_closedir(dir);
    
    return res;
}


int main(void)
{
    uint8_t t = 0;
    uint8_t buf[20] = {0};
    
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
    
    sd_test_fatfs();
    
    mf_scan_files("0:");
    
    f_setlabel((const TCHAR *)"0:shuaige");
    f_getlabel((const TCHAR *)"0:", (TCHAR *)buf , 0);
    printf("label:%s \r\n", buf);
    
    while (1)
    {
        t++;
        delay_ms(10);

        if (t == 20)
        {
            LED0_TOGGLE();
            t = 0;
        }
    }
}






























