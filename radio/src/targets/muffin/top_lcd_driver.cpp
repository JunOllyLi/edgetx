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
#include "sh1107.h"

//#define ENABLE_SH1107

EXT_RAM_BSS_ATTR static uint8_t oled_buf[OLED_W * OLED_H] = {0};
void toplcdInit()
{
#ifdef ENABLE_SH1107
  sh1107_init();
#endif
}

void toplcdOff()
{
}

void setTopFirstTimer(int32_t value)
{
}

void setTopRssiValue(uint32_t rssi)
{
}

void setTopRssiBar(uint32_t rssi)
{
}

void setTopRssi(uint32_t rssi)
{
}

void setTopBatteryState(int state, uint8_t blinking)
{
}

void setTopBatteryValue(uint32_t volts)
{
}

void setTopSecondTimer(uint32_t value)
{
}

void toplcdRefreshStart()
{
#ifdef ENABLE_SH1107
  const lv_area_t area = {
    .x1 = 0, .y1 = 0, .x2 = OLED_W - 1, .y2 = OLED_H - 1
  };
  sh1107_flush(NULL, &area, (lv_color_t *)oled_buf);
#endif
}

void toplcdRefreshEnd()
{
}
