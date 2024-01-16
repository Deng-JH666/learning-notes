/**
 ****************************************************************************************************
 * @file        mjpeg.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-05-24
 * @brief       MJPEG��Ƶ���� ����
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

#include "./MJPEG/avi.h"
#include "./MJPEG/mjpeg.h"
#include "./MALLOC/malloc.h"
#include "./FATFS/source/ff.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LCD/lcd.h"



jpeg_codec_typedef mjpeg;               /* JPEGӲ������ṹ�� */

uint16_t g_img_offx, g_img_offy;        /* ͼ����LCD��Ļ��x,y�����ƫ���� */
uint16_t *p_rgb565buf;                  /* ������RGBͼ���Դ��ַ */
volatile uint32_t g_mjpeg_remain_size;  /* MJPEGһ֡ͼ���ʣ���С(�ֽ���) */
volatile uint8_t g_mjpeg_fileover = 0;  /* MJPEGͼƬ�ļ���ȡ��ɱ�־ */

//extern uint32_t *ltdc_framebuf[2];    /* LTDC LCD֡��������ָ��,��ltdc.c���涨�� */


/**
 * @brief       ��Ƶ����汾��jpeg_core_init����
 *   @note      ��Ƶ���벢����Ҫ�õ���� JPEG_DMA_INBUF_NB
 * @param       tjpeg   : JPEG�������ƽṹ��
 * @retval      ִ�н��
 *   @arg       0     , �ɹ�
 *   @arg       ����  , ʧ��
 */
uint8_t mjpeg_jpeg_core_init(jpeg_codec_typedef *tjpeg)
{
    RCC->AHB3ENR |= 1 << 5;     /* ʹ��Ӳ��jpegʱ�� */
    RCC->AHB3RSTR |= 1 << 5;    /* ��λӲ��jpeg������ */
    RCC->AHB3RSTR &= ~(1 << 5); /* ������λ */
    JPEG->CR = 0;               /* ������ */
    JPEG->CR |= 1 << 0;         /* ʹ��Ӳ��JPEG */
    JPEG->CONFR0 &= ~(1 << 0);  /* ֹͣJPEG�������� */
    JPEG->CR |= 1 << 13;        /* �������fifo */
    JPEG->CR |= 1 << 14;        /* ������fifo */
    JPEG->CFR = 3 << 5;         /* ��ձ�־ */
    HAL_NVIC_SetPriority(JPEG_IRQn,1,3); /* ��ռ1�������ȼ�3����2 */
    HAL_NVIC_EnableIRQ(JPEG_IRQn);
    JPEG->CONFR1 |= 1 << 8;     /* ʹ��header���� */
    return 0;
}

/**
 * @brief       ��Ƶ����汾��jpeg_core_destroy����
 * @param       tjpeg   : JPEG�������ƽṹ��
 * @retval      ��
 */
void mjpeg_jpeg_core_destroy(jpeg_codec_typedef *tjpeg)
{
    uint8_t i;
    jpeg_dma_stop();    /* ֹͣMDMA���� */

    for (i = 0; i < JPEG_DMA_OUTBUF_NB; i++)
    {
        myfree(SRAM12, tjpeg->outbuf[i].buf);   /* �ͷ��ڴ� */
    }
}

/**
 * @brief       JPEG����������,�ص�����,���ڻ�ȡJPEG�ļ�ԭʼ����
 *   @note      ÿ��JPEG DMA IN BUFΪ�յ�ʱ��,���øú���
 * @param       ��
 * @retval      ��
 */
void mjpeg_dma_in_callback(void)
{
    if (g_mjpeg_remain_size)    /* ����ʣ��������Ҫ���� */
    {
        mjpeg.inbuf[0].buf += JPEG_DMA_INBUF_LEN;       /* ƫ�Ƶ���һ��λ�� */

        if (g_mjpeg_remain_size < JPEG_DMA_INBUF_LEN)   /* ʣ�����ݱȽ���,һ�ξͿ��Դ������ */
        {
            mjpeg.inbuf[0].size = g_mjpeg_remain_size;  /* �����С����ʣ���ܴ�С */
            g_mjpeg_remain_size = 0;    /* һ�δ���Ϳ��Ը��� */
        }
        else     /* ͼƬ�Ƚϴ�,��Ҫ�ֶ�δ��� */
        {
            mjpeg.inbuf[0].size = JPEG_DMA_INBUF_LEN;   /* ������䳤��,�����δ��� */
            g_mjpeg_remain_size -= JPEG_DMA_INBUF_LEN;  /* ʣ�೤�ȵݼ� */
        }

        jpeg_in_dma_resume((uint32_t)mjpeg.inbuf[0].buf, mjpeg.inbuf[0].size);  /* ������һ��DMA���� */
    }
    else
    {
        g_mjpeg_fileover = 1;   /* �ļ���ȡ��� */
    }
}

/**
 * @brief       JPEG���������(YCBCR)�ص�����,�������YCbCr������
 * @param       ��
 * @retval      ��
 */
void mjpeg_dma_out_callback(void)
{
    uint32_t *pdata = 0;
    mjpeg.outbuf[mjpeg.outbuf_write_ptr].sta = 1;   /* ��buf���� */
    mjpeg.outbuf[mjpeg.outbuf_write_ptr].size = mjpeg.yuvblk_size - (MDMA_Channel6->CBNDTR & 0X1FFFF);  /* ��buf�������ݵĳ��� */

    if (mjpeg.state == JPEG_STATE_FINISHED)         /* ����ļ��Ѿ��������,��Ҫ��ȡDOR��������(<=32�ֽ�) */
    {
        pdata = (uint32_t *)(mjpeg.outbuf[mjpeg.outbuf_write_ptr].buf + mjpeg.outbuf[mjpeg.outbuf_write_ptr].size);

        while (JPEG->SR & (1 << 4))
        {
            *pdata = JPEG->DOR;
            pdata++;
            mjpeg.outbuf[mjpeg.outbuf_write_ptr].size += 4;
        }
    }

    mjpeg.outbuf_write_ptr++;   /* ָ����һ��buf */

    if (mjpeg.outbuf_write_ptr >= JPEG_DMA_OUTBUF_NB) mjpeg.outbuf_write_ptr = 0;    /* ���� */

    if (mjpeg.outbuf[mjpeg.outbuf_write_ptr].sta == 1)   /* ����Чbuf */
    {
        mjpeg.outdma_pause = 1; /* �����ͣ */
    }
    else     /* ��Ч��buf */
    {
        jpeg_out_dma_resume((uint32_t)mjpeg.outbuf[mjpeg.outbuf_write_ptr].buf, mjpeg.yuvblk_size); /* ������һ��DMA���� */
    }
}

/**
 * @brief       JPEG�����ļ�������ɻص�����
 * @param       ��
 * @retval      ��
 */
void mjpeg_endofcovert_callback(void)
{
    mjpeg.state = JPEG_STATE_FINISHED;  /* ���JPEG������� */
}

/**
 * @brief       JPEG header�����ɹ��ص�����
 * @param       ��
 * @retval      ��
 */
void mjpeg_hdrover_callback(void)
{
    mjpeg.state = JPEG_STATE_HEADEROK;  /* HEADER��ȡ�ɹ� */
    jpeg_get_info(&mjpeg);              /* ��ȡJPEG�����Ϣ,������С,ɫ�ʿռ�,������ */

    /* ��Ҫ��ȡJPEG������Ϣ�Ժ�,���ܸ���jpeg�����С�Ͳ�����ʽ,��������������С,���������MDMA */
    switch (mjpeg.Conf.ChromaSubsampling)
    {
        case JPEG_420_SUBSAMPLING:
            mjpeg.yuvblk_size = 24 * mjpeg.Conf.ImageWidth; /* YUV420,ÿ��YUV����ռ1.5���ֽ�.ÿ�����16��.16*1.5=24 */
            mjpeg.yuvblk_height = 16;   /* ÿ�����16�� */
            break;

        case JPEG_422_SUBSAMPLING:
            mjpeg.yuvblk_size = 16 * mjpeg.Conf.ImageWidth; /* YUV420,ÿ��YUV����ռ2���ֽ�.ÿ�����8��.8*2=16 */
            mjpeg.yuvblk_height = 8;    /* ÿ�����8�� */
            break;

        case JPEG_444_SUBSAMPLING:
            mjpeg.yuvblk_size = 24 * mjpeg.Conf.ImageWidth; /* YUV420,ÿ��YUV����ռ3���ֽ�.ÿ�����8��.8*3=24 */
            mjpeg.yuvblk_height = 8;    /* ÿ�����8�� */
            break;
    }

    mjpeg.yuvblk_curheight = 0;         /* ��ǰ�м��������� */

    if (mjpeg.outbuf[1].buf != NULL)    /* ����buf������OK */
    {
        jpeg_out_dma_init((uint32_t)mjpeg.outbuf[0].buf, mjpeg.yuvblk_size);    /* �������DMA */
        jpeg_out_dma_start();           /* ����DMA OUT����,��ʼ����JPEG���������� */
    }
}

/**
 * @brief       ��ʼ��MJPEG
 * @param       offx    : ��ʾͼ����LCD��x�����ƫ����
 * @param       offy    : ��ʾͼ����LCD��y�����ƫ����
 * @param       width   : ��ʾͼ��Ŀ��
 * @param       height  : ��ʾͼ��ĸ߶�
 * @retval      ִ�н��
 *   @arg       0     , �ɹ�
 *   @arg       ����  , ʧ��
 */
uint8_t mjpeg_init(uint16_t offx, uint16_t offy, uint32_t width, uint32_t height)
{
    uint8_t i;
    uint8_t res;
    
    res = mjpeg_jpeg_core_init(&mjpeg);   /* ��ʼ��JPEG�ں�,������IN BUF */

    if (res) return 1;

    for (i = 0; i < JPEG_DMA_OUTBUF_NB; i++)
    {
        /* �����ͼƬ��ȵ�24��,���⻹���ܻ����Ҫ32�ֽ��ڴ� */
        mjpeg.outbuf[i].buf = mymalloc(SRAM12, width * 24 + 32);

        if (mjpeg.outbuf[i].buf == NULL) return 2;
    }

    g_img_offx = offx;
    g_img_offy = offy;
    
    return 0;
}

/**
 * @brief       MJPEG�ͷ�����������ڴ�
 * @param       ��
 * @retval      ��
 */
void mjpeg_free(void)
{
    mjpeg_jpeg_core_destroy(&mjpeg);
}

/**
 * @brief       �����ɫ
 * @param       x, y    : ��ʼ����
 * @param       width   : ���
 * @param       height  : �߶�
 * @param       color   : ��ɫ����
 * @retval      ��
 */
void mjpeg_fill_color(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t *color)
{
    /* ��MCU������Ҫ���(RGB�����������!!, ��YUVת����ʱ��,ֱ�Ӿ������) */
    lcd_color_fill(x, y, x + width - 1, y + height - 1, color);
}

/**
 * @brief       ����һ��JPEGͼƬ
 *   @note      ע������:
 *              1, ������ͼƬ�ķֱ���,����С�ڵ�����Ļ�ķֱ���!
 *              2, �뱣֤ͼƬ�Ŀ����16�ı���,���������ֻ���.
 * @param       buf     : jpeg����������
 * @param       bsize   : �����С
 * @retval      ִ�н��
 *   @arg       0     , �ɹ�
 *   @arg       ����  , ʧ��
 */
uint8_t mjpeg_decode(uint8_t *buf, uint32_t bsize)
{
    volatile uint32_t timecnt = 0;

    if (bsize == 0) return 0;

    jpeg_decode_init(&mjpeg);       /* ��ʼ��Ӳ��JPEG������ */
    g_mjpeg_remain_size = bsize;    /* ��¼��ǰͼƬ�Ĵ�С(�ֽ���) */
    mjpeg.inbuf[0].buf = buf;       /* ָ��jpeg���������׵�ַ */
    g_mjpeg_fileover = 0;           /* ���δ���� */

    if (g_mjpeg_remain_size < JPEG_DMA_INBUF_LEN)   /* ͼƬ�Ƚ�С,һ�ξͿ��Դ������ */
    {
        //mjpeg.inbuf[0].size=g_mjpeg_remain_size;  /* �����С�����ܴ�С */
        //g_mjpeg_remain_size=0;                    /* һ�δ���Ϳ��Ը��� */
        return 0;   /* ͼƬ�ߴ�Ƚ�С��ֱ�Ӳ����� */
    }
    else    /* ͼƬ�Ƚϴ�,��Ҫ�ֶ�δ��� */
    {
        mjpeg.inbuf[0].size = JPEG_DMA_INBUF_LEN;   /* ������䳤��,�����δ��� */
        g_mjpeg_remain_size -= JPEG_DMA_INBUF_LEN;  /* ʣ�೤�� */
    }

    jpeg_in_dma_init((uint32_t)mjpeg.inbuf[0].buf, mjpeg.inbuf[0].size);    /* ��������DMA */
    jpeg_in_callback = mjpeg_dma_in_callback;       /* JPEG DMA��ȡ���ݻص����� */
    jpeg_out_callback = mjpeg_dma_out_callback;     /* JPEG DMA������ݻص����� */
    jpeg_eoc_callback = mjpeg_endofcovert_callback; /* JPEG ��������ص����� */
    jpeg_hdp_callback = mjpeg_hdrover_callback;     /* JPEG Header������ɻص����� */
    jpeg_in_dma_start();    /* ����DMA IN����,��ʼ����JPEGͼƬ */
    p_rgb565buf = 0;        /* ָ����� */

    while (1)
    {
        if (p_rgb565buf == 0 && mjpeg.state == JPEG_STATE_HEADEROK) /* p_rgb565buf��,��JPEG HEAD������� */
        {
            p_rgb565buf = mymalloc(SRAM12, mjpeg.Conf.ImageWidth * mjpeg.yuvblk_height * 2 + 32);   /* ���뵥����������ڴ� */

            if (p_rgb565buf == 0)return 1;
        }

        if (mjpeg.outbuf[mjpeg.outbuf_read_ptr].sta == 1)   /* buf����������Ҫ���� */
        {
            SCB_CleanInvalidateDCache();    /* ���D catch */

            jpeg_dma2d_yuv2rgb_conversion(&mjpeg, (uint32_t *)p_rgb565buf); /* ����DMA2D,��YUVͼ��ת��RGB565ͼ�� */

            mjpeg_fill_color(g_img_offx, g_img_offy + mjpeg.yuvblk_curheight, mjpeg.Conf.ImageWidth, mjpeg.yuvblk_height, p_rgb565buf);
            mjpeg.yuvblk_curheight += mjpeg.yuvblk_height;  /* ��ƫ�� */
            
            //SCB_CleanInvalidateDCache();  /* ���D catch */
            mjpeg.outbuf[mjpeg.outbuf_read_ptr].sta = 0;    /* ���bufΪ�� */
            mjpeg.outbuf[mjpeg.outbuf_read_ptr].size = 0;   /* ��������� */
            mjpeg.outbuf_read_ptr++;

            if (mjpeg.outbuf_read_ptr >= JPEG_DMA_OUTBUF_NB) mjpeg.outbuf_read_ptr = 0;  /* ���Ʒ�Χ */

            if (mjpeg.yuvblk_curheight >= mjpeg.Conf.ImageHeight) break;     /* ��ǰ�߶ȵ��ڻ��߳���ͼƬ�ֱ��ʵĸ߶�,��˵�����������,ֱ���˳� */
        }
        else if (mjpeg.outdma_pause == 1 && mjpeg.outbuf[mjpeg.outbuf_write_ptr].sta == 0)  /* out��ͣ,�ҵ�ǰwritebuf�Ѿ�Ϊ����,��ָ�out��� */
        {
            jpeg_out_dma_resume((uint32_t)mjpeg.outbuf[mjpeg.outbuf_write_ptr].buf, mjpeg.yuvblk_size); /* ������һ��DMA���� */
            mjpeg.outdma_pause = 0;
        }

        if (mjpeg.state == JPEG_STATE_ERROR)    /* �������,ֱ���˳� */
        {
            break;
        }

        if (mjpeg.state == JPEG_STATE_FINISHED) /* ���������,����Ƿ��쳣���� */
        {
            if (mjpeg.yuvblk_curheight < mjpeg.Conf.ImageHeight)
            {
                if (mjpeg.Conf.ImageHeight > (mjpeg.yuvblk_curheight + 16)) /* �����쳣,ֱ���˳� */
                {
                    mjpeg.state = JPEG_STATE_ERROR; /* ��Ǵ��� */
                    printf("early finished!\r\n");
                    break;
                }
            }
        }

        if (g_mjpeg_fileover)   /* �ļ�������,��ʱ�˳�,��ֹ��ѭ�� */
        {
            timecnt++;

            if (mjpeg.state == JPEG_STATE_NOHEADER) break;  /* ����JPEGͷʧ���� */

            if (timecnt > 0X3FFFF) break;   /* ��ʱ�˳� */
        }
    }

    myfree(SRAM12, p_rgb565buf);    /* �ͷ��ڴ� */
    return 0;
}

























