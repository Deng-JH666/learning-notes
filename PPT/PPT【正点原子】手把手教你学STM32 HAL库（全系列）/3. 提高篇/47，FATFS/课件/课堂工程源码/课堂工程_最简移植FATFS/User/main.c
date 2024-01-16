/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-28
 * @brief       SD卡 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
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

    /* 1 挂载SD卡设备 */
    ret = f_mount(fs_obj, "0:", 1);
    if (ret)
    {
        printf("mount fail %d \r\n", ret);
    }
    else
    {
        printf("mount OK\r\n");
    }
    
    /* 2 打开文件(需要注意要关闭文件f_close) */
    ret = f_open(fil_obj, "0:sd_test_test_test_test_test_test.txt", FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
    if (ret)
    {
        printf("open fail %d \r\n", ret);
    }
    else
    {
        printf("open OK\r\n");
    }
    
    /* 3 读取数据 */
    fsize = f_size(fil_obj);
    f_read(fil_obj, rd_buf, fsize, (UINT *)&rd_conut);
    if (rd_conut != 0)
    {
        printf("rd_conut:%d rd_buf:%s \r\n",rd_conut, rd_buf);
    }
    
    /* 4 数据写入 */            /* 注意：读写指针 */
    f_write(fil_obj, "333", 3, (UINT *)&wd_conut);
    f_printf(fil_obj, "hello_world");
    
    f_lseek(fil_obj, 0);
    fsize = f_size(fil_obj);
    f_read(fil_obj, rd_buf, fsize, (UINT *)&rd_conut);
    if (rd_conut != 0)
    {
        printf("rd_conut:%d rd_buf:%s \r\n",rd_conut, rd_buf);
    }
    
    /* 5 关闭文件 */
    f_close(fil_obj);
}

uint8_t mf_scan_files(uint8_t * path)
{
    FRESULT res;    
    DIR *dir;
    FILINFO *fileinfo;
    
    dir = mymalloc(SRAMIN, sizeof(DIR));
    fileinfo = mymalloc(SRAMIN, sizeof(FILINFO));

    res = f_opendir(dir, (const TCHAR*)path); //打开一个目录
    
    if (res == FR_OK) 
    {
        printf("\r\n"); 
        
        while(1)
        {
            res = f_readdir(dir, fileinfo);   /* 读取目录下的一个文件 */
            if (res != FR_OK || fileinfo->fname[0] == 0) break; /* 错误了/到末尾了,退出 */
            //if (fileinfo.fname[0] == '.') continue;           /* 忽略上级目录 */
            printf("%s/", path);                /* 打印路径 */
            printf("%s\r\n",fileinfo->fname);   /* 打印文件名 */
        } 
    }
    f_closedir(dir);
    
    return res;
}


int main(void)
{
    uint8_t t = 0;
    uint8_t buf[20] = {0};
    
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






























