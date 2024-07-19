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
//#include "nimble/nimble_port.h"
//#include "nimble/nimble_port_freertos.h"

#include "driver/i2c_master.h"

extern void flysky_hall_stick_init();
extern void ads1015_adc_init(void);

i2c_master_bus_handle_t i2c_0_bus_handle;
i2c_master_bus_handle_t lvgl_i2c_bus_handle;
i2c_master_bus_handle_t rtc_i2c_bus_handle;
i2c_master_bus_handle_t gpioext_i2c_bus_handle;
i2c_master_bus_handle_t toplcd_i2c_bus_handle;
i2c_master_bus_handle_t ads_i2c_bus_handle;
static void board_init_i2c(void) {
    i2c_master_bus_config_t i2c_bus_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = I2C_0_SDA,
        .scl_io_num = I2C_0_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags = {
          .enable_internal_pullup = 1
        }
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &i2c_0_bus_handle));
    lvgl_i2c_bus_handle = i2c_0_bus_handle;
    rtc_i2c_bus_handle = i2c_0_bus_handle;
    gpioext_i2c_bus_handle = i2c_0_bus_handle;
    toplcd_i2c_bus_handle = i2c_0_bus_handle;
    ads_i2c_bus_handle = i2c_0_bus_handle;
}
#if 0
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
#endif

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

  //nimble_port_init();

  board_init_i2c();
  lv_init();
  lvgl_driver_init(&disp_drv);

  keysInit();
  rtcInit();

  sdInit();

  //backlightInit();
  initWiFi();
  init2MhzTimer();

  flysky_hall_stick_init();
  audioInit();

  ads1015_adc_init();
  toplcdInit();
}

void boardOff()
{
  pwrOff();
}

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
