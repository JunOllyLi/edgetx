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

/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "lvgl_helpers.h"

#include "nvs_flash.h"
/* BLE */
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

extern void adruino_adc_init(void);
extern void flysky_hall_stick_init();

#if defined(__cplusplus)
extern "C" {
#endif

//#include "usb_dcd_int.h"
//#include "usb_bsp.h"
#if defined(__cplusplus)
}
#endif

HardwareOptions hardwareOptions;

void watchdogInit(unsigned int duration)
{
}

#if defined(SPORT_UPDATE_PWR_GPIO)
void sportUpdateInit()
{
}

void sportUpdatePowerOn()
{
}

void sportUpdatePowerOff()
{
}

void sportUpdatePowerInit()
{
  if (g_eeGeneral.sportUpdatePower == 1)
    sportUpdatePowerOn();
  else
    sportUpdatePowerOff();
}
#endif

// just to keep a reference of the layout so they do not get optimized out by compiler.
extern LayoutFactory Layout1P2;
extern LayoutFactory Layout1P3;
extern LayoutFactory layout1x1;
extern LayoutFactory Layout1x2;
extern LayoutFactory Layout1x3;
extern LayoutFactory Layout1x4;
extern LayoutFactory layout2P1;
extern LayoutFactory Layout2P3;
extern LayoutFactory Layout2x1;
extern LayoutFactory layout2x2;
extern LayoutFactory layout2x3;
extern LayoutFactory layout2x4;
extern LayoutFactory layout4P2;
LayoutFactory *layouts[20] = {
  &Layout1P2, &Layout1P3, &layout1x1, &Layout1x2, &Layout1x3, &Layout1x4,
  &layout2P1, &Layout2P3, &Layout2x1, &layout2x2, &layout2x3, &layout2x4,
  &layout4P2
};

lv_color_t* lcdbuf;
extern lv_disp_drv_t disp_drv;
void boardInit()
{
  /* Initialize NVS — it is used to store PHY calibration data */
  esp_err_t ret = nvs_flash_init();
  if  (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  nimble_port_init();

  audioInit();
#if !defined(CONFIG_LV_TFT_DISPLAY_CONTROLLER_ILI9488)
  lcdbuf = (lv_color_t*)heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t) * 2, MALLOC_CAP_DMA);
#endif

  lv_init();
  lvgl_driver_init(&disp_drv); // lvgl driver initializes I2C_0 as well.

  keysInit();

  sdInit();
  rtcInit();

  backlightInit();

  initWiFi();

  flysky_hall_stick_init();
  init2MhzTimer();
  init5msTimer();
  adruino_adc_init();

  toplcdInit();
}

void boardOff()
{
  pwrOff();
}

#if defined(AUDIO_SPEAKER_ENABLE_GPIO)
void initSpeakerEnable()
{
}

void enableSpeaker()
{
}

void disableSpeaker()
{
}
#endif

#if defined(HEADPHONE_TRAINER_SWITCH_GPIO)
void initHeadphoneTrainerSwitch()
{
}

void enableHeadphone()
{
}

void enableTrainer()
{
}
#endif

#if defined(JACK_DETECT_GPIO)
void initJackDetect(void)
{
}
#endif

int usbPlugged() {
  return 0;// TODO-feather
}

void enableVBatBridge() {

}
void disableVBatBridge() {

}
bool isVBatBridgeEnabled() {
  return false;
}
#ifndef ESP_PLATFORM
void NVIC_SystemReset(void) {}
#endif
