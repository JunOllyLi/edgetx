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

extern "C" {
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"

/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "lvgl_helpers.h"
}

#if !defined(ESP_PLATFORM)
pixel_t LCD_FIRST_FRAME_BUFFER[DISPLAY_BUFFER_SIZE] __SDRAM;
pixel_t LCD_SECOND_FRAME_BUFFER[DISPLAY_BUFFER_SIZE] __SDRAM;
BitmapBuffer lcdBuffer1(BMP_RGB565, LCD_W, LCD_H, (uint16_t *)LCD_FIRST_FRAME_BUFFER);
BitmapBuffer lcdBuffer2(BMP_RGB565, LCD_W, LCD_H, (uint16_t *)LCD_SECOND_FRAME_BUFFER);
BitmapBuffer * lcdFront = &lcdBuffer1;
BitmapBuffer * lcd = &lcdBuffer2;
#else
extern uint32_t _ext_ram_bss_start;
pixel_t *LCD_FIRST_FRAME_BUFFER;
pixel_t *LCD_SECOND_FRAME_BUFFER;

//BitmapBuffer * lcdFront = NULL;
BitmapBuffer * lcd = NULL;
#endif


extern BitmapBuffer * lcdFront;
extern BitmapBuffer * lcd;

lv_disp_drv_t disp_drv;
static lv_disp_draw_buf_t disp_buf;
static lv_disp_t* disp = nullptr;
static lv_indev_drv_t indev_drv;
extern lv_color_t* lcdbuf;
void lcdInitDisplayDriver()
{
#if defined(CONFIG_LV_TFT_DISPLAY_CONTROLLER_ILI9488)
    lv_color_t* buf1 = (lv_color_t*)malloc(DISP_BUF_SIZE * sizeof(lv_color_t));
#else
    lv_color_t* buf1 = lcdbuf;
#endif
    assert(buf1 != NULL);

    /* Use double buffered when not working with monochrome displays */
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
#if defined(CONFIG_LV_TFT_DISPLAY_CONTROLLER_ILI9488)
    lv_color_t* buf2 = (lv_color_t*)malloc(DISP_BUF_SIZE * sizeof(lv_color_t));
#else
    lv_color_t* buf2 = &lcdbuf[DISP_BUF_SIZE];
#endif
    assert(buf2 != NULL);
#else
    static lv_color_t *buf2 = NULL;
#endif

    uint32_t size_in_px = DISP_BUF_SIZE;

#if defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_IL3820         \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_JD79653A    \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_UC8151D     \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_SSD1306

    /* Actual size in pixels, not bytes. */
    size_in_px *= 8;
#endif

    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;

#if defined CONFIG_DISPLAY_ORIENTATION_PORTRAIT || defined CONFIG_DISPLAY_ORIENTATION_PORTRAIT_INVERTED
    disp_drv.rotated = 1;
#endif

    /* When using a monochrome display we need to register the callbacks:
     * - rounder_cb
     * - set_px_cb */
#ifdef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb = disp_driver_set_px;
#endif

    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.draw_buf = &disp_buf;
    disp = lv_disp_drv_register(&disp_drv);

    lv_disp_set_default(disp);

    // remove all styles on default screen (makes it transparent as well)
    lv_obj_remove_style_all(lv_scr_act());

    // transparent background:
    //  - this prevents LVGL overwritting things drawn directly into the bitmap buffer
    lv_disp_set_bg_opa(disp, LV_OPA_TRANSP);

    // allow drawing at any moment
    _lv_refr_set_disp_refreshing(disp);

#if defined(ESP_PLATFORM)
  //lcdFront = new BitmapBuffer(BMP_RGB565, LCD_W, LCD_H);
  lcd = new BitmapBuffer(BMP_RGB565, LCD_W, LCD_H,
      (uint16_t *)malloc(align32(LCD_W * LCD_H * sizeof(uint16_t))));
#endif

  lv_indev_drv_init(&indev_drv);
  indev_drv.read_cb = touch_driver_read;
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  lv_indev_drv_register(&indev_drv);
}

#define DISPBUF_LINES (DISP_BUF_SIZE / LV_HOR_RES_MAX)
#define PIXEL_EACHBLOCK (LV_HOR_RES_MAX * DISPBUF_LINES)
void lcdRefresh() {
  lv_color_t *p = (lv_color_t *)lcd->getData();
  lv_area_t area = {.x1 = 0, .y1 = 0, .x2 = LV_HOR_RES_MAX - 1, .y2 = DISPBUF_LINES - 1};
  while (area.y2 < LV_VER_RES_MAX) {
    int lines = LV_VER_RES_MAX - area.y1;
    if (lines > DISPBUF_LINES) lines = DISPBUF_LINES;

    disp_drv.flush_cb(&disp_drv, &area, p);
    area.y1 += lines;
    area.y2 += lines;
    p += PIXEL_EACHBLOCK; // OK to be added pass the end at the end of drawing
  }
}

void lcdInitDirectDrawing() {
}

/*
  Starts LCD initialization routine. It should be called as
  soon as possible after the reset because LCD takes a lot of
  time to properly power-on.

  Make sure that delay_ms() is functional before calling this function!
*/
void lcdInit()
{
}

static TouchState internalTouchState = {0};
struct TouchState getInternalTouchState() {
  return internalTouchState;
}

static int state = 0;

struct TouchState touchPanelRead() {
  return internalTouchState;
}

bool touchPanelEventOccured() {
  bool ret = false;
  return ret;
}

bool touchPanelInit(void) {
    /* Register an input device when enabled on the menuconfig */
  //lv_indev_drv_t indev_drv;
  //lv_indev_drv_init(&indev_drv);
  //indev_drv.read_cb = touch_driver_read;
  //indev_drv.type = LV_INDEV_TYPE_POINTER;
  //lv_indev_drv_register(&indev_drv);

  return true;
}

void DMAWait()
{
}

void DMACopyBitmap(uint16_t *dest, uint16_t destw, uint16_t desth, uint16_t x,
                   uint16_t y, const uint16_t *src, uint16_t srcw,
                   uint16_t srch, uint16_t srcx, uint16_t srcy, uint16_t w,
                   uint16_t h)
{
  for (int i = 0; i < h; i++) {
    memcpy(dest + (y + i) * destw + x, src + (srcy + i) * srcw + srcx, 2 * w);
  }
}

// 'src' has ARGB4444
// 'dest' has RGB565
void DMACopyAlphaBitmap(uint16_t *dest, uint16_t destw, uint16_t desth,
                        uint16_t x, uint16_t y, const uint16_t *src,
                        uint16_t srcw, uint16_t srch, uint16_t srcx,
                        uint16_t srcy, uint16_t w, uint16_t h)
{
  for (coord_t line = 0; line < h; line++) {
    uint16_t *p = dest + (y + line) * destw + x;
    const uint16_t *q = src + (srcy + line) * srcw + srcx;
    for (coord_t col = 0; col < w; col++) {
      uint8_t alpha = *q >> 12;
      uint8_t red =
          ((((*q >> 8) & 0x0f) << 1) * alpha + (*p >> 11) * (0x0f - alpha)) /
          0x0f;
      uint8_t green = ((((*q >> 4) & 0x0f) << 2) * alpha +
                       ((*p >> 5) & 0x3f) * (0x0f - alpha)) /
                      0x0f;
      uint8_t blue = ((((*q >> 0) & 0x0f) << 1) * alpha +
                      ((*p >> 0) & 0x1f) * (0x0f - alpha)) /
                     0x0f;
      *p = (red << 11) + (green << 5) + (blue << 0);
      p++;
      q++;
    }
  }
}

// 'src' has A8/L8?
// 'dest' has RGB565
void DMACopyAlphaMask(uint16_t *dest, uint16_t destw, uint16_t desth,
                      uint16_t x, uint16_t y, const uint8_t *src, uint16_t srcw,
                      uint16_t srch, uint16_t srcx, uint16_t srcy, uint16_t w,
                      uint16_t h, uint16_t fg_color)
{
  RGB_SPLIT(fg_color, red, green, blue);

  for (coord_t line = 0; line < h; line++) {
    uint16_t *p = dest + (y + line) * destw + x;
    const uint8_t *q = src + (srcy + line) * srcw + srcx;
    for (coord_t col = 0; col < w; col++) {
      uint16_t opacity = *q >> 4;  // convert to 4 bits (stored in 8bit for DMA)
      uint8_t bgWeight = OPACITY_MAX - opacity;
      RGB_SPLIT(*p, bgRed, bgGreen, bgBlue);
      uint16_t r = (bgRed * bgWeight + red * opacity) / OPACITY_MAX;
      uint16_t g = (bgGreen * bgWeight + green * opacity) / OPACITY_MAX;
      uint16_t b = (bgBlue * bgWeight + blue * opacity) / OPACITY_MAX;
      *p = RGB_JOIN(r, g, b);
      p++;
      q++;
    }
  }
}
