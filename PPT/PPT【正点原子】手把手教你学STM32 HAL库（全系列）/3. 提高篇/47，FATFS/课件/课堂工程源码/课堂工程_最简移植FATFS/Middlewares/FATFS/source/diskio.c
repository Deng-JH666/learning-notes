/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "./BSP/SDIO/sdio_sdcard.h"

/* Definitions of physical drive number for each drive */
#define DEV_SD		0	/* Example: Map SD card to physical drive 1 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
/* f_mount */
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	int result;
    
    if (pdrv == DEV_SD)
    {
        result = sd_init();
        if (result)
        {
            return STA_NOINIT;
        }
    }
    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	int result;

    if (pdrv == DEV_SD)
    {
        result = sd_read_disk(buff, sector, count);
        if (result)
        {
            return RES_PARERR;
        }
    }
    
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	int result;

    if (pdrv == DEV_SD)
    {
        result = sd_write_disk((uint8_t *)buff, sector, count);
        if (result)
        {
            return RES_PARERR;
        }
    }
    
	return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
    
    if (pdrv == DEV_SD)
    {
        switch (cmd) {
            case CTRL_SYNC :
                res = RES_OK;
                break;

            case GET_SECTOR_SIZE :
                *(DWORD *)buff = 512;
                break;

            case GET_BLOCK_SIZE :
                *(DWORD *)buff  = g_sdcard_handler.SdCard.BlockSize;
                res = RES_OK;
                break;
                
            case GET_SECTOR_COUNT :
                *(DWORD *)buff  = ((long long)g_sdcard_handler.SdCard.BlockNbr * g_sdcard_handler.SdCard.BlockSize) / 512;
                res = RES_OK;
                break;
                
            default:
                res = RES_PARERR;
                break;
        }
    }
    else
    {
        res = RES_ERROR;
    }

    return res;
}

