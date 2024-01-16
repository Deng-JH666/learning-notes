/**
 ****************************************************************************************************
 * @file        videoplayer.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-05-24
 * @brief       ��Ƶ������ Ӧ�ô���
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20220524
 * ��һ�η���
 *
 ****************************************************************************************************
 */


#include "string.h"
#include "./MJPEG/avi.h"
#include "./MJPEG/mjpeg.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"

#include "./BSP/KEY/key.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/TIMER/btim.h"

#include "./TEXT/text.h"
#include "./MALLOC/malloc.h"
#include "./APP/videoplayer.h"
#include "./FATFS/exfuns/exfuns.h"



uint8_t g_frame;            /* ͳ��֡�� */
volatile uint8_t g_frameup; /* ��Ƶ����ʱ϶���Ʊ���,������1��ʱ��,���Ը�����һ֡��Ƶ */


uint8_t *g_psaibuf;         /* ��Ƶ����֡,��1֡,5K�ֽ�,����û����ƵӲ��������ʵ�ʲ�û���õ�,�����ڶ�ȡID�� */



/**
 * @brief       �õ�path·����,Ŀ���ļ����ܸ���
 * @param       path    : ·��
 * @retval      ����Ч�ļ���
 */
uint16_t video_get_tnum(char *path)
{
    uint8_t res;
    uint16_t rval = 0;
    DIR tdir;           /* ��ʱĿ¼ */
    FILINFO *tfileinfo; /* ��ʱ�ļ���Ϣ */
    
    tfileinfo = (FILINFO *)mymalloc(SRAM12, sizeof(FILINFO));   /* �����ڴ� */
    res = f_opendir(&tdir, (const TCHAR *)path);    /* ��Ŀ¼ */

    if (res == FR_OK && tfileinfo)
    {
        while (1)   /* ��ѯ�ܵ���Ч�ļ��� */
        {
            res = f_readdir(&tdir, tfileinfo);      /* ��ȡĿ¼�µ�һ���ļ� */

            if (res != FR_OK || tfileinfo->fname[0] == 0)break; /* ������/��ĩβ��,�˳� */

            res = exfuns_file_type(tfileinfo->fname);

            if ((res & 0XF0) == 0X60)   /* ȡ����λ,�����ǲ�����Ƶ�ļ� */
            {
                rval++;     /* ��Ч�ļ�������1 */
            }
        }
    }

    myfree(SRAM12, tfileinfo);  /* �ͷ��ڴ� */
    return rval;
}

/**
 * @brief       ��ʾ��ǰ����ʱ��
 * @param       aviinfo : AVI���ƽṹ��
 * @retval      ��
 */
void video_time_show(FIL *favi, AVI_INFO *aviinfo)
{
    static uint32_t oldsec; /* ��һ�εĲ���ʱ�� */
    char *buf;
    uint32_t totsec = 0;    /* video�ļ���ʱ�� */
    uint32_t cursec;        /* ��ǰ����ʱ�� */
    totsec = (aviinfo->SecPerFrame / 1000) * aviinfo->TotalFrame;   /* �ܳ���(��λ:ms) */
    totsec /= 1000;         /* ������ */
    cursec = ((double)favi->fptr / favi->obj.objsize) * totsec;     /* ��ǰ���ŵ���������? */

    if (oldsec != cursec)   /* ��Ҫ������ʾʱ�� */
    {
        buf = mymalloc(SRAM12, 100);    /* ����100�ֽ��ڴ� */
        oldsec = cursec; 
        sprintf(buf, "����ʱ��:%02d:%02d:%02d/%02d:%02d:%02d", cursec / 3600, (cursec % 3600) / 60, cursec % 60, totsec / 3600, (totsec % 3600) / 60, totsec % 60);
        text_show_string(10, 90, lcddev.width - 10, 16, buf, 16, 0, BLUE);  /* ��ʾ�������� */
        myfree(SRAM12, buf);
    }
}

/**
 * @brief       ��ʾ��ǰ��Ƶ�ļ��������Ϣ
 * @param       aviinfo : AVI���ƽṹ��
 * @retval      ��
 */
void video_info_show(AVI_INFO *aviinfo)
{
    char *buf;
    buf = mymalloc(SRAM12, 100);    /* ����100�ֽ��ڴ� */ 
    sprintf(buf, "������:%d,������:%d", aviinfo->Channels, aviinfo->SampleRate * 10);
    text_show_string(10, 50, lcddev.width - 10, 16, buf, 16, 0, RED);   /* ��ʾ�������� */
    sprintf(buf, "֡��:%d֡", 1000 / (aviinfo->SecPerFrame / 1000));
    text_show_string(10, 70, lcddev.width - 10, 16, buf, 16, 0, RED);   /* ��ʾ�������� */
    myfree(SRAM12, buf);
}

/**
 * @brief       ��ʾ��Ƶ������Ϣ��ʾ
 * @param       name    : ��Ƶ����
 * @param       index   : ��ǰ����
 * @param       total   : ���ļ���
 * @retval      ��
 */
void video_bmsg_show(char *name, uint16_t index, uint16_t total)
{
    char *buf;
    buf = mymalloc(SRAM12, 100); /* ����100�ֽ��ڴ� */
    sprintf(buf, "�ļ���:%s", name);
    text_show_string(10, 10, lcddev.width - 10, 16, buf, 16, 0, RED);   /* ��ʾ�ļ��� */
    sprintf(buf, "����:%d/%d", index, total);
    text_show_string(10, 30, lcddev.width - 10, 16, buf, 16, 0, RED);   /* ��ʾ���� */
    myfree(SRAM12, buf);
}


/**
 * @brief       ������Ƶ
 * @param       ��
 * @retval      ��
 */
void video_play(void)
{
    uint8_t res;
    DIR vdir;               /* Ŀ¼ */
    FILINFO *vfileinfo;     /* �ļ���Ϣ */
    char *pname;            /* ��·�����ļ��� */
    uint16_t totavinum;     /* ��Ƶ�ļ����� */
    uint16_t curindex;      /* ��Ƶ�ļ���ǰ���� */
    uint8_t key;            /* ��ֵ */
    uint32_t temp;
    uint32_t *voffsettbl;   /* ��Ƶ�ļ�off block������ */

    while (f_opendir(&vdir, "0:/VIDEO"))    /* ����Ƶ�ļ��� */
    {
        text_show_string(60, 190, 240, 16, "VIDEO�ļ��д���!", 16, 0, RED);
        delay_ms(200);
        lcd_fill(60, 190, 240, 206, WHITE); /* �����ʾ */
        delay_ms(200);
    }

    totavinum = video_get_tnum("0:/VIDEO"); /* �õ�����Ч�ļ��� */

    while (totavinum == NULL)   /* ��Ƶ�ļ�����Ϊ0 */
    {
        text_show_string(60, 190, 240, 16, "û����Ƶ�ļ�!", 16, 0, RED);
        delay_ms(200);
        lcd_fill(60, 190, 240, 146, WHITE); /* �����ʾ */
        delay_ms(200);
    }

    vfileinfo = (FILINFO *)mymalloc(SRAM12, sizeof(FILINFO));   /* Ϊ���ļ������������ڴ� */
    pname = mymalloc(SRAM12, 2 * FF_MAX_LFN + 1);               /* Ϊ��·�����ļ��������ڴ� */
    voffsettbl = mymalloc(SRAM12, 4 * totavinum);               /* ����4*totavinum���ֽڵ��ڴ�,���ڴ����Ƶ�ļ����� */

    while (vfileinfo == NULL || pname == NULL || voffsettbl == NULL)    /* �ڴ������� */
    {
        text_show_string(60, 190, 240, 16, "�ڴ����ʧ��!", 16, 0, RED);
        delay_ms(200);
        lcd_fill(60, 190, 240, 146, WHITE); /* �����ʾ */
        delay_ms(200);
    }

    /* ��¼���� */
    res = f_opendir(&vdir, "0:/VIDEO"); /* ��Ŀ¼ */

    if (res == FR_OK)
    {
        curindex = 0;   /* ��ǰ����Ϊ0 */

        while (1)       /* ȫ����ѯһ�� */
        {
            temp = vdir.dptr;                   /* ��¼��ǰoffset */
            res = f_readdir(&vdir, vfileinfo);  /* ��ȡĿ¼�µ�һ���ļ� */

            if (res != FR_OK || vfileinfo->fname[0] == 0)break; /* ������/��ĩβ��,�˳� */

            res = exfuns_file_type(vfileinfo->fname);

            if ((res & 0XF0) == 0X60)           /* ȡ����λ,�����ǲ��������ļ� */
            {
                voffsettbl[curindex] = temp;    /* ��¼���� */
                curindex++;
            }
        }
    }

    curindex = 0;   /* ��0��ʼ��ʾ */
    res = f_opendir(&vdir, (const TCHAR *)"0:/VIDEO");  /* ��Ŀ¼ */

    while (res == FR_OK)        /* �򿪳ɹ� */
    {
        dir_sdi(&vdir, voffsettbl[curindex]);   /* �ı䵱ǰĿ¼���� */
        res = f_readdir(&vdir, vfileinfo);      /* ��ȡĿ¼�µ�һ���ļ� */

        if (res != FR_OK || vfileinfo->fname[0] == 0) break;    /* ������/��ĩβ��,�˳� */

        strcpy((char *)pname, "0:/VIDEO/");     /* ����·��(Ŀ¼) */
        strcat((char *)pname, (const char *)vfileinfo->fname);  /* ���ļ������ں��� */
        lcd_clear(WHITE);       /* ������ */
        video_bmsg_show(vfileinfo->fname, curindex + 1, totavinum);  /* ��ʾ����,��������Ϣ */
        key = video_play_mjpeg(pname);          /* ���������Ƶ�ļ� */

        if (key == KEY1_PRES)   /* ��һ�� */
        {
            if (curindex)
            {
                curindex--;
            }
            else
            {
                curindex = totavinum - 1;
            }
        }
        else if (key == KEY0_PRES || key == 3)      /* ��һ�� key=3 */
        {
            curindex++;

            if (curindex >= totavinum)curindex = 0; /* ��ĩβ��ʱ��,�Զ���ͷ��ʼ */
        }
        else
        {
            break;  /* �����˴��� */
        }
    }

    myfree(SRAM12, vfileinfo);  /* �ͷ��ڴ� */
    myfree(SRAM12, pname);      /* �ͷ��ڴ� */
    myfree(SRAM12, voffsettbl); /* �ͷ��ڴ� */
}

/**
 * @brief       ����һ��MJPEG�ļ�
 * @param       pname   : �ļ���
 * @retval      ִ�н��
 *   @arg       KEY0_PRES , ��һ��
 *   @arg       KEY1_PRES , ��һ��
 *   @arg       ����      , ����
 */
uint8_t video_play_mjpeg(char *pname)
{
    uint8_t *framebuf;  /* ��Ƶ����buf */
    uint8_t *pbuf;      /* bufָ�� */
    FIL *favi;
    uint8_t  res = 0;
    uint32_t offset = 0;
    uint32_t nr;
    uint8_t key;
    
    g_psaibuf = mymalloc(SRAM4, AVI_AUDIO_BUF_SIZE);    /* ������Ƶ�ڴ� */
    framebuf = mymalloc(SRAMIN, AVI_VIDEO_BUF_SIZE);    /* ������Ƶbuf */
    favi = (FIL *)mymalloc(SRAM12, sizeof(FIL));        /* ����favi�ڴ� */
    memset(g_psaibuf, 0, AVI_AUDIO_BUF_SIZE);

    if (!g_psaibuf || !framebuf || !favi)
    {
        printf("memory error!\r\n");
        res = 0XFF;
    }

    while (res == 0)
    {
        res = f_open(favi, (char *)pname, FA_READ);     /* ��avi�ļ� */
        if (res == 0)
        {
            pbuf = framebuf;
            res = f_read(favi, pbuf, AVI_VIDEO_BUF_SIZE, &nr);  /* ��ʼ��ȡ */
            if (res)
            {
                printf("fread error:%d\r\n", res);
                break;
            }

            /* ��ʼavi���� */
            res = avi_init(pbuf, AVI_VIDEO_BUF_SIZE);   /* avi���� */
            if (res || g_avix.Width > lcddev.width)
            {
                printf("avi err:%d\r\n", res);
                res = KEY0_PRES;
                break;
            }

            video_info_show(&g_avix);
            btim_tim7_int_init(g_avix.SecPerFrame / 100 - 1, 24000 - 1);    /* 10Khz����Ƶ��,��1��100us */
            offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "movi");       /* Ѱ��movi ID */
            avi_get_streaminfo(pbuf + offset + 4);      /* ��ȡ����Ϣ */
            f_lseek(favi, offset + 12);                 /* ������־ID,����ַƫ�Ƶ������ݿ�ʼ�� */

            if (lcddev.height <= g_avix.Height)
            {
                res = mjpeg_init((lcddev.width - g_avix.Width) / 2, (lcddev.height - g_avix.Height) / 2, g_avix.Width, g_avix.Height);              /* JPG�����ʼ�� */
            }
            else
            {
                res = mjpeg_init((lcddev.width - g_avix.Width) / 2, 110 + (lcddev.height - 110 - g_avix.Height) / 2, g_avix.Width, g_avix.Height);  /* JPG�����ʼ�� */
            }

            if (res)
            {
                mjpeg_free(); 
                break;
            }

            if (g_avix.SampleRate)    /* ����Ƶ��Ϣ,�ų�ʼ�� */
            {
                /* û����ƵӲ��,���Բ���������Ƶ���� */
            }

            while (1)   /* ����ѭ�� */
            {
                if (g_avix.StreamID == AVI_VIDS_FLAG) /* ��Ƶ�� */
                {
                    pbuf = framebuf;
                    f_read(favi, pbuf, g_avix.StreamSize + 8, &nr); /* ������֡+��һ������ID��Ϣ */
                    res = mjpeg_decode(pbuf, g_avix.StreamSize);

                    if (res)
                    {
                        printf("decode error!\r\n");
                    }

                    while (g_frameup == 0); /* �ȴ�ʱ�䵽��(��TIM7���ж���������Ϊ1) */

                    g_frameup = 0;  /* ��־���� */
                    g_frame++;
                }
                else if (g_avix.StreamID == AVI_AUDS_FLAG)  /* ��Ƶ�� */
                {
                    video_time_show(favi, &g_avix);         /* ��ʾ��ǰ����ʱ�� */
                    f_read(favi, g_psaibuf, g_avix.StreamSize + 8, &nr);  /* ���g_psaibuf */
                    pbuf = g_psaibuf;
                }

                key = key_scan(0);
                if (key == KEY0_PRES || key == KEY1_PRES)   /* KEY0/KEY1����,������һ��/��һ����Ƶ */
                {
                    res = key;
                    break;
                }
                else if (key == KEY1_PRES || key == WKUP_PRES)
                {
                    video_seek(favi, &g_avix, framebuf);
                    pbuf = framebuf;
                }

                if (avi_get_streaminfo(pbuf + g_avix.StreamSize))   /* ��ȡ��һ֡ ����־ */
                {
                    pbuf = framebuf;
                    res = f_read(favi, pbuf, AVI_VIDEO_BUF_SIZE, &nr);  /* ��ʼ��ȡ */

                    if (res == 0 && nr == AVI_VIDEO_BUF_SIZE)           /* ��ȡ�ɹ�,�Ҷ�ȡ��ָ�����ȵ����� */
                    {
                        offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "00dc");   /* Ѱ��AVI_VIDS_FLAG,00dc */
                        avi_get_streaminfo(pbuf + offset);              /* ��ȡ����Ϣ */

                        if (offset) f_lseek(favi, (favi->fptr - AVI_VIDEO_BUF_SIZE) + offset + 8);
                    }
                    else
                    {
                        printf("g_frame error \r\n");
                        res = KEY0_PRES;
                        break;
                    }
                }
            }

            TIM7->CR1 &= ~(1 << 0); /* �رն�ʱ��7 */
            lcd_set_window(0, 0, lcddev.width, lcddev.height);  /* �ָ����� */
            mjpeg_free();           /* �ͷ��ڴ� */
            f_close(favi);
        }
    }

    myfree(SRAM4, g_psaibuf);
    myfree(SRAMIN, framebuf);
    myfree(SRAM12, favi);
    return res;
}

/**
 * @brief       AVI�ļ�����
 * @param       favi    : AVI�ļ�
 * @param       aviinfo : AVI��Ϣ�ṹ��
 * @param       mbuf    : ���ݻ�����
 * @retval      ִ�н��
 *   @arg       0    , �ɹ�
 *   @arg       ���� , ����
 */
uint8_t video_seek(FIL *favi, AVI_INFO *aviinfo, uint8_t *mbuf)
{
    uint32_t fpos = favi->fptr;
    uint8_t *pbuf;
    uint32_t offset;
    uint32_t br;
    uint32_t delta;
    uint32_t totsec;
    uint8_t key;
    totsec = (aviinfo->SecPerFrame / 1000) * aviinfo->TotalFrame;
    totsec /= 1000; /* ������ */
    delta = (favi->obj.objsize / totsec) * 5;   /* ÿ��ǰ��5���ӵ������� */

    while (1)
    {
        key = key_scan(1);

        if (key == WKUP_PRES)   /* ��� */
        {
            if (fpos < favi->obj.objsize)fpos += delta;

            if (fpos > (favi->obj.objsize - AVI_VIDEO_BUF_SIZE))
            {
                fpos = favi->obj.objsize - AVI_VIDEO_BUF_SIZE;
            }
        }
        else if (key == KEY1_PRES)  /* ���� */
        {
            if (fpos > delta)fpos -= delta;
            else fpos = 0;
        }
        else if (g_avix.StreamID == AVI_VIDS_FLAG)break;

        f_lseek(favi, fpos);
        f_read(favi, mbuf, AVI_VIDEO_BUF_SIZE, &br);    /* ������֡+��һ������ID��Ϣ */
        pbuf = mbuf;

        if (fpos == 0)  /* ��0��ʼ,����Ѱ��movi ID */
        {
            offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "movi");
        }
        else
        {
            offset = 0;
        }
        
        offset += avi_srarch_id(pbuf + offset, AVI_VIDEO_BUF_SIZE, g_avix.VideoFLAG); /* Ѱ����Ƶ֡ */
        avi_get_streaminfo(pbuf + offset);  /* ��ȡ����Ϣ */
        f_lseek(favi, fpos + offset + 8);   /* ������־ID,����ַƫ�Ƶ������ݿ�ʼ�� */

        if (g_avix.StreamID == AVI_VIDS_FLAG)
        {
            f_read(favi, mbuf, g_avix.StreamSize + 8, &br);   /* ������֡ */
            mjpeg_decode(mbuf, g_avix.StreamSize);            /* ��ʾ��Ƶ֡ */
        }
        else
        {
            printf("error flag");
        }

        video_time_show(favi, &g_avix);
    }

    return 0;
}






















