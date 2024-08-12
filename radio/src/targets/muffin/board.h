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

#ifndef _BOARD_H_
#define _BOARD_H_

#include "board_common.h"

const etx_serial_port_t* auxSerialGetPort(int port_nr);

//++++++++++++++++++++++++++++++++++++++++++++++++++++TODO-MUFFIN

#define pwrOffPressed() false

#define BACKLIGHT_LEVEL_MAX     100
#define BACKLIGHT_LEVEL_MIN     10

#define MB                              *1024*1024
#define LUA_MEM_EXTRA_MAX               (2 MB)    // max allowed memory usage for Lua bitmaps (in bytes)
#define LUA_MEM_MAX                     (6 MB)    // max allowed memory usage for complete Lua  (in bytes), 0 means unlimited

#define PERI1_FREQUENCY               30000000
#define PERI2_FREQUENCY               60000000

// Board driver
void boardInit();
void boardOff();

// TODO-Muffin cleanup
#define TRAINER_PPM_OUT_TASK_CORE 0
#define MIXER_TASK_CORE 0
#define PULSES_TASK_CORE 0
#define MENU_TASK_CORE 1
#define AUDIO_TASK_CORE 1

/*
From Kconfig
  LCD D0 - D7: 21, 14, 13, 12, 11, 10, 9, 46
  LCD CS 45
  LCD DC 48
  LCD WR 47

//  MOSI -1
//  MISO -1
//  RESET -1
//  SCLK -1
//  TOUCH CS -1
//
*/
#define TOUCH_IRQ GPIO_NUM_38

#define I2C_0_SCL GPIO_NUM_7
#define I2C_0_SDA GPIO_NUM_6

#define BACKLITE_PIN GPIO_NUM_3

#define EXTMOD_UART_PORT UART_NUM_1
#define EXTMOD_UART_TX GPIO_NUM_8  // EXTMOD_RX
#define EXTMOD_UART_RX GPIO_NUM_15 // EXTMOD_TX
// Use RMT for Flysky UART
#define FLYSKY_UART_RX_PIN GPIO_NUM_4  // GIMBLE_TX
#define FLYSKY_UART_TX_PIN GPIO_NUM_5  // GIMBLE_RX

#define INTMOD_UART_PORT UART_NUM_2
#define INTMOD_ESP_UART_TX GPIO_NUM_2 // INTMOD_RX
#define INTMOD_ESP_UART_RX GPIO_NUM_1 // INTMOD_TX

#define I2C_MASTER_NUM I2C_NUM_0

#define SD_DEDICATED_SPI
#ifdef SD_DEDICATED_SPI
#define SD_SPI_HOST SPI2_HOST
#define SDSPI_CLK GPIO_NUM_40
#define SDSPI_MOSI GPIO_NUM_41
#define SDSPI_MISO GPIO_NUM_39
#endif
#define SDCARD_CS_GPIO GPIO_NUM_42

#define I2S_DOUT GPIO_NUM_16
#define I2S_BCLK GPIO_NUM_17
#define I2S_LRCLK GPIO_NUM_18

#define SOFT_PWR_CTRL
uint32_t pwrCheck();
void pwrOn();
void pwrOff();
bool pwrPressed();

void INTERNAL_MODULE_ON(void);
void INTERNAL_MODULE_OFF(void);
void EXTERNAL_MODULE_ON(void);
void EXTERNAL_MODULE_OFF(void);

struct TouchState getInternalTouchState();
struct TouchState touchPanelRead();
bool touchPanelEventOccured();

#define BATTERY_WARN                  35 // 3.5V
#define BATTERY_MIN                   34 // 3.4V
#define BATTERY_MAX                   42 // 4.2V
#define BATTERY_TYPE_FIXED

void backlightInit();
#define BACKLIGHT_FORCED_ON 101
void backlightDisable();
#define BACKLIGHT_DISABLE()             backlightDisable()
void backlightEnable(uint8_t level = 0);
#define BACKLIGHT_ENABLE()            backlightEnable(currentBacklightBright)
bool isBacklightEnabled();

// Audio driver
void audioInit() ;
#define VOLUME_LEVEL_MAX  23
#define VOLUME_LEVEL_DEF  12
#if !defined(SOFTWARE_VOLUME)
void setScaledVolume(uint8_t volume);
void setVolume(uint8_t volume);
int32_t getVolume();
#endif
void setSampleRate(uint32_t frequency);
void audioConsumeCurrentBuffer();

#define audioDisableIrq()               taskDISABLE_INTERRUPTS()
#define audioEnableIrq()                taskENABLE_INTERRUPTS()

void hapticOff();
void hapticOn();

// Second serial port driver
#define DEBUG_BAUDRATE                  115200
#define LUA_DEFAULT_BAUDRATE            115200

// LED driver
void ledInit();
void ledOff();
void ledRed();
void ledGreen();
void ledBlue();
#if defined(FUNCTION_SWITCHES)
void fsLedOff(uint8_t);
void fsLedOn(uint8_t);
#endif

#define LCD_DEPTH                       16
void lcdRefresh();
bool touchPanelInit(void);

void lcdInit();
void lcdInitFinish();
void lcdOff();

#define lcdRefreshWait()

// Top LCD driver
void toplcdInit();
void toplcdOff();
void toplcdRefreshStart();
void toplcdRefreshEnd();
void setTopFirstTimer(int32_t value);
void setTopSecondTimer(uint32_t value);
void setTopRssi(uint32_t rssi);
void setTopBatteryState(int state, uint8_t blinking);
void setTopBatteryValue(uint32_t volts);

#if defined(__cplusplus)
#include "fifo.h"

#if defined(CROSSFIRE)
#define TELEMETRY_FIFO_SIZE             128
#else
#define TELEMETRY_FIFO_SIZE             64
#endif

extern Fifo<uint8_t, TELEMETRY_FIFO_SIZE> telemetryFifo;
#endif

#define BATT_SCALE 1251
#define BATTERY_DIVIDER 320384
#define VOLTAGE_DROP 0


// WiFi

#ifndef ESP_NOW_ETH_ALEN
#define ESP_NOW_ETH_ALEN 6
#endif

void initWiFi();
void startWiFi( char *ssid_zchar, char *passwd_zchar, char* ftppass_zchar);
void stopWiFi();
const char* getWiFiStatus();
bool isWiFiStarted(uint32_t expire=500);

void startWiFiESPNow();
void stopWiFiESPNow();
void init_espnow();
void disable_espnow();
void pause_espnow();
void resume_espnow();
void init_bind_espnow();
void stop_bind_espnow();
bool is_binding_espnow();

#endif // _BOARD_H_
