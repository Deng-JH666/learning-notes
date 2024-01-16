/**
 ****************************************************************************************************
 * @file        videoplayer.h
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

#ifndef __VIDEOPLAYER_H
#define __VIDEOPLAYER_H

#include "./MJPEG/avi.h"
#include "./SYSTEM/sys/sys.h"
#include "./FATFS/source/ff.h"


#define AVI_AUDIO_BUF_SIZE      1024*5          /* ����avi����ʱ,��Ƶbuf��С */
#define AVI_VIDEO_BUF_SIZE      1024*260        /* ����avi����ʱ,��Ƶbuf��С.һ�����AVI_MAX_FRAME_SIZE�Ĵ�С */


void video_play(void);
uint8_t video_play_mjpeg(char *pname); 
void video_time_show(FIL *favi,AVI_INFO *aviinfo);
void video_info_show(AVI_INFO *aviinfo);
void video_bmsg_show(char* name,uint16_t index,uint16_t total);
uint16_t video_get_tnum(char *path);
uint8_t video_seek(FIL *favi,AVI_INFO *aviinfo, uint8_t *mbuf);

#endif

