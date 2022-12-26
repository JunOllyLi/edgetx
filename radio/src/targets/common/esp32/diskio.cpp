/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x 
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "opentx.h"
#include "FatFs/diskio.h"
#include "FatFs/ff.h"

#include "driver/gpio.h"
#include "soc/soc_caps.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"

static bool card_present = false; // default to consider it as present until mount failed
static sdmmc_host_t config = SDSPI_HOST_DEFAULT();
static sdspi_dev_handle_t handle;
static sdspi_device_config_t dev_config = SDSPI_DEVICE_CONFIG_DEFAULT();
static sdmmc_card_t sdcard;
static sdmmc_card_t* card = &sdcard;

bool SD_CARD_PRESENT(void) {
  return card_present;
}

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
        BYTE drv                /* Physical drive number (0) */
)
{
  return card_present ? 0 : STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (
        BYTE drv                /* Physical drive number (0) */
)
{
  return 0;
}

RTOS_MUTEX_HANDLE spiMutex;
/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
        BYTE drv,                       /* Physical drive number (0) */
        BYTE *buff,                     /* Pointer to the data buffer to store read data */
        DWORD sector,           /* Start sector number (LBA) */
        UINT count                      /* Sector count (1..255) */
)
{
  DRESULT res = RES_ERROR;
  if (0 == sdmmc_read_sectors(card, buff, sector, count)) {
    res = RES_OK;
  }
  return res;
}

DRESULT disk_write (
        BYTE drv,                       /* Physical drive number (0) */
        const BYTE *buff,       /* Pointer to the data to be written */
        DWORD sector,           /* Start sector number (LBA) */
        UINT count                      /* Sector count (1..255) */
)
{
  DRESULT res = RES_ERROR;
  if (0 == sdmmc_write_sectors(card, buff, sector, count)) {
    res = RES_OK;
  }
  return res;
}


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
        BYTE drv,               /* Physical drive number (0) */
        BYTE ctrl,              /* Control code */
        void *buff              /* Buffer to send/receive control data */
)
{
    assert(card);
    switch(ctrl) {
        case CTRL_SYNC:
            return RES_OK;
        case GET_SECTOR_COUNT:
            *((DWORD*) buff) = card->csd.capacity;
            return RES_OK;
        case GET_SECTOR_SIZE:
            *((WORD*) buff) = card->csd.sector_size;
            return RES_OK;
        case GET_BLOCK_SIZE:
            return RES_ERROR;
    }
    return RES_ERROR;
}


/*-----------------------------------------------------------------------*/
/* Device Timer Interrupt Procedure  (Platform dependent)                */
/*-----------------------------------------------------------------------*/
/* This function must be called in period of 10ms                        */

void sdPoll10ms()
{
}

bool _g_FATFS_init = false;
FATFS g_FATFS_Obj __DMA; // this is in uninitialised section !!!

void sdMount()
{
  if (!card_present) {
    TRACE("No card to mount");
  } else if (_g_FATFS_init) {
    TRACE("Card already mounted");
  } else {
    if (f_mount(&g_FATFS_Obj, "", 1) == FR_OK) {
      // call sdGetFreeSectors() now because f_getfree() takes a long time first time it's called
      _g_FATFS_init = true;
    }
  }
}

static RTOS_MUTEX_HANDLE ioMutex;

void sdInit(void)
{
  if (!card_present) {
#ifdef SD_DEDICATED_SPI
    spi_bus_config_t bus_config = {
        .mosi_io_num = SDSPI_MOSI,
        .miso_io_num = SDSPI_MISO,
        .sclk_io_num = SDSPI_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    config.slot = SD_SPI_HOST;
    spi_bus_initialize((spi_host_device_t)config.slot, &bus_config, SPI_DMA_CH_AUTO);
#endif

    dev_config.host_id = (spi_host_device_t)config.slot;

    RTOS_CREATE_MUTEX(ioMutex);

    dev_config.gpio_cs = (gpio_num_t)SDCARD_CS_GPIO;
    sdspi_host_init();
    sdspi_host_init_device(&dev_config, &handle);

    config.slot = handle;
    if (0 == sdmmc_card_init(&config, card)) {
      card_present = true;
      sdMount();
    } else {
      sdspi_host_remove_device(handle);
      sdspi_host_deinit();
    }
  }
}

void sdDone()
{
  TRACE("+++++%s", __FUNCTION__);
}

uint32_t sdMounted()
{
  return _g_FATFS_init;
}

uint32_t sdIsHC()
{
  return 0;//(sdcard_type(sddisk) == CARD_SDHC);
}

uint32_t sdGetSpeed()
{
  TRACE("+++++%s", __FUNCTION__);
  return 330000;
}

uint32_t ioMutexReq = 0, ioMutexRel = 0;
int ff_cre_syncobj (BYTE vol, FF_SYNC_t *mutex)
{
  *mutex = ioMutex;
  return 1;
}

int ff_req_grant (FF_SYNC_t mutex)
{
  ioMutexReq += 1;
  RTOS_LOCK_MUTEX(mutex);
  return 1;
}

void ff_rel_grant (FF_SYNC_t mutex)
{
  ioMutexRel += 1;
  RTOS_UNLOCK_MUTEX(mutex);
}

int ff_del_syncobj (FF_SYNC_t mutex)
{
  return 1;
}
