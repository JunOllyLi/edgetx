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
#include "i2c_driver.h"
#include "FreeRTOS_entry.h"
// keys uses MCP23X17
/* ================= chip definitions ======================= */
#define MCP23XXX_IODIR 0x00   //!< I/O direction register
#define MCP23XXX_IPOL 0x01    //!< Input polarity register
#define MCP23XXX_GPINTEN 0x02 //!< Interrupt-on-change control register
#define MCP23XXX_DEFVAL                                                        \
  0x03 //!< Default compare register for interrupt-on-change
#define MCP23XXX_INTCON 0x04 //!< Interrupt control register
#define MCP23XXX_IOCON 0x05  //!< Configuration register
#define MCP23XXX_GPPU 0x06   //!< Pull-up resistor configuration register
#define MCP23XXX_INTF 0x07   //!< Interrupt flag register
#define MCP23XXX_INTCAP 0x08 //!< Interrupt capture register
#define MCP23XXX_GPIO 0x09   //!< Port register
#define MCP23XXX_OLAT 0x0A   //!< Output latch register

#define MCP23XXX_ADDR 0x20 //!< Default I2C Address

#define MCP_PORT(pin) ((pin < 8) ? 0 : 1) //!< Determine port from pin number

#define MCP23XXX_INT_ERR 255 //!< Interrupt error

#define MCP_REG_ADDR(baseAddr, port) ((baseAddr << 1) | port)

/* ============== end chip definitions ==================== */

#define MCP0_ADDR (MCP23XXX_ADDR)
#define MCP1_ADDR (MCP23XXX_ADDR + 1)

#define INTMOD_PWREN_BIT (0x80U)

#define MCP1_SWITCHES_MASK 0xF8U
#define MCP1_KEYS_MASK 0x7U
#define MCP1_TRIM_MASK 0xFFU

static uint8_t mcp1_trim = 0U;
  // ~0x02 thr trim down
  // ~0x01 thr trim up
  // ~0x08 ele trim down
static uint8_t switches = 0U;
  // ~0x08 swc up
  // ~0x80 swa down
  // ~0x40 swb down
  // ~0x10 swc down
  // ~0x20 swd down

#define MCP1_SW_PIN_A 0x80
#define MCP1_SW_PIN_B 0x40
#define MCP1_SW_PIN_D 0x20
#define MCP1_SW_PIN_C_L 0x08
#define MCP1_SW_PIN_C_H 0x10

#define ADD_INV_2POS_CASE(x) \
  case SW_S ## x ## 0: \
    xxx = switches  & MCP1_SW_PIN_ ## x ; \
    break; \
  case SW_S ## x ## 2: \
    xxx = ~switches  & MCP1_SW_PIN_ ## x ; \
    break
#define ADD_2POS_CASE(x) \
  case SW_S ## x ## 2: \
    xxx = switches  & MCP1_SW_PIN_ ## x ; \
    break; \
  case SW_S ## x ## 0: \
    xxx = ~switches  & MCP1_SW_PIN_ ## x ; \
    break
#define ADD_INV_3POS_CASE(x, i) \
  case SW_S ## x ## 0: \
    xxx = (switches & MCP1_SW_PIN_ ## x ## _H); \
    if (IS_3POS(i)) { \
      xxx = xxx && (~switches & MCP1_SW_PIN_ ## x ## _L); \
    } \
    break; \
  case SW_S ## x ## 1: \
    xxx = (~switches & MCP1_SW_PIN_ ## x ## _H) && (~switches & MCP1_SW_PIN_ ## x ## _L); \
    break; \
  case SW_S ## x ## 2: \
    xxx = (~switches & MCP1_SW_PIN_ ## x ## _H); \
    if (IS_3POS(i)) { \
      xxx = xxx && (switches & MCP1_SW_PIN_ ## x ## _L); \
    } \
    break
#define ADD_3POS_CASE(x, i) \
  case SW_S ## x ## 2: \
    xxx = (switches & MCP1_SW_PIN_ ## x ## _H); \
    if (IS_3POS(i)) { \
      xxx = xxx && (~switches & MCP1_SW_PIN_ ## x ## _L); \
    } \
    break; \
  case SW_S ## x ## 1: \
    xxx = (switches & MCP1_SW_PIN_ ## x ## _H) && (switches & MCP1_SW_PIN_ ## x ## _L); \
    break; \
  case SW_S ## x ## 0: \
    xxx = (~switches & MCP1_SW_PIN_ ## x ## _H); \
    if (IS_3POS(i)) { \
      xxx = xxx && (switches & MCP1_SW_PIN_ ## x ## _L); \
    } \
    break

static bool mcp0_exist = false;
static bool mcp1_exist = false;
static RTOS_MUTEX_HANDLE keyMutex;

#define PWR_BTN_BIT 0x80
#define PWR_OFF_BIT 0x40
#define PWR_BTN_DOWN_AT_START 1
#define PWR_BTN_RELEASED_AT_START 2
#define PWR_BTN_DOWN 3
#define PWR_BTN_RELEASED_AFTER_DOWN 4
static int pwr_btn_state = 0;

static void process_pwr_btn_state(bool btn_down) {
    switch (pwr_btn_state) {
      case 0:
        if (btn_down) {
          pwr_btn_state = PWR_BTN_DOWN_AT_START;
        } else {
          pwr_btn_state = PWR_BTN_RELEASED_AT_START;
        }
        break;
      case PWR_BTN_DOWN_AT_START:
        if (!btn_down) {
          pwr_btn_state = PWR_BTN_RELEASED_AT_START;
        }
        break;
      case PWR_BTN_RELEASED_AT_START:
        if (btn_down) {
          pwr_btn_state = PWR_BTN_DOWN;
        }
        break;
      case PWR_BTN_DOWN:
        if (!btn_down) {
          pwr_btn_state = PWR_BTN_RELEASED_AFTER_DOWN;
        }
        break;
    }
}

uint32_t readKeys()
{
  uint32_t result = 0;

  RTOS_LOCK_MUTEX(keyMutex);
  uint8_t mask = (1 << BUTTONS_ON_GPIOA) - 1;
  uint8_t gpioAB[2] = {0};
  if (mcp0_exist) {
    i2c_register_read(MCP0_ADDR, MCP_REG_ADDR(MCP23XXX_GPIO, 0), gpioAB, sizeof(gpioAB));
    result |= (gpioAB[0] ^ mask) & mask;
    process_pwr_btn_state(0 != (gpioAB[0] & PWR_BTN_BIT));
  }
  if (mcp1_exist) {
    i2c_register_read(MCP1_ADDR, MCP_REG_ADDR(MCP23XXX_GPIO, 0), gpioAB, sizeof(gpioAB));
    mcp1_trim = (gpioAB[0] & MCP1_TRIM_MASK) ^ MCP1_TRIM_MASK;
    switches = (gpioAB[1] & MCP1_SWITCHES_MASK) ^ MCP1_SWITCHES_MASK;

    result |= (((gpioAB[1] ^ MCP1_KEYS_MASK) & MCP1_KEYS_MASK) << (BUTTONS_ON_GPIOA + 1));
  }
  RTOS_UNLOCK_MUTEX(keyMutex);

  return result;
}

uint32_t readTrims()
{
  uint32_t result = mcp1_trim;
  return result;
}

bool trimDown(uint8_t idx)
{
  return readTrims() & (1 << idx);
}

bool keyDown()
{
  return readKeys() || readTrims();
}

/* TODO common to ARM */
void readKeysAndTrims()
{
  uint8_t index = 0;
  uint32_t keys_input = readKeys();
  for (unsigned i = 1; i != unsigned(1 << TRM_BASE); i <<= 1) {
    keys[index++].input(keys_input & i);
  }

  uint32_t trims_input = readTrims();
  for (uint8_t i = 1; i != uint8_t(1 << 8); i <<= 1) {
    keys[index++].input(trims_input & i);
  }

#if defined(PWR_BUTTON_PRESS)
  if ((keys_input || trims_input || pwrPressed()) && (g_eeGeneral.backlightMode & e_backlight_mode_keys)) {
#else
  if ((keys_input || trims_input) && (g_eeGeneral.backlightMode & e_backlight_mode_keys)) {
#endif
    // on keypress turn the light on
    resetBacklightTimeout();
  }
}

#if !defined(BOOT)
uint32_t switchState(uint8_t index)
{
  uint32_t xxx = 0;

  switch (index) {
    ADD_INV_2POS_CASE(A);
    ADD_INV_2POS_CASE(B);
    ADD_INV_3POS_CASE(C, 2);
    ADD_INV_2POS_CASE(D);
  }

  return xxx;
}
#endif

void keysInit()
{
  RTOS_CREATE_MUTEX(keyMutex);
//#ifndef DISABLE_I2C_DEVS
  // pull up
  esp_err_t ret  = 0;
  ret = i2c_register_write_byte(MCP0_ADDR, MCP_REG_ADDR(MCP23XXX_GPPU, 0), (1 << BUTTONS_ON_GPIOA) - 1);
  if (0 == ret) {
    mcp0_exist = true;
    ret = i2c_register_write_byte(MCP0_ADDR, MCP_REG_ADDR(MCP23XXX_GPPU, 0), (1 << BUTTONS_ON_GPIOA) - 1); // TODO: for some reason need to set pull up twice?
    // pin direction
    i2c_register_write_byte(MCP0_ADDR, MCP_REG_ADDR(MCP23XXX_IODIR, 0), 0xFF); // all input by default

    i2c_register_write_byte(MCP0_ADDR, MCP_REG_ADDR(MCP23XXX_IODIR, 1), (uint8_t)((~INTMOD_PWREN_BIT) & 0xFF));
    i2c_register_write_byte(MCP0_ADDR, MCP_REG_ADDR(MCP23XXX_GPIO, 1), 0x00U);
  }

  // pull up
  ret = i2c_register_write_byte(MCP1_ADDR, MCP_REG_ADDR(MCP23XXX_GPPU, 0), MCP1_TRIM_MASK);
  if (0 == ret) {
    mcp1_exist = true;
    ret = i2c_register_write_byte(MCP1_ADDR, MCP_REG_ADDR(MCP23XXX_GPPU, 0), MCP1_TRIM_MASK); // TODO: for some reason need to set pull up twice?
    i2c_register_write_byte(MCP1_ADDR, MCP_REG_ADDR(MCP23XXX_GPPU, 1), MCP1_SWITCHES_MASK | MCP1_KEYS_MASK);

    // pin direction
    i2c_register_write_byte(MCP1_ADDR, MCP_REG_ADDR(MCP23XXX_IODIR, 0), 0xFF); // all input by default
    i2c_register_write_byte(MCP1_ADDR, MCP_REG_ADDR(MCP23XXX_IODIR, 1), 0xFF); // all input by default
  }
}

void INTERNAL_MODULE_ON(void) {
    uint8_t gpioB[1] = {0};
    i2c_register_read(MCP0_ADDR, MCP_REG_ADDR(MCP23XXX_GPIO, 1), gpioB, sizeof(gpioB));
    i2c_register_write_byte(MCP0_ADDR, MCP_REG_ADDR(MCP23XXX_GPIO, 1), gpioB[0] | INTMOD_PWREN_BIT);
}
void INTERNAL_MODULE_OFF(void) {
    uint8_t gpioB[1] = {0};
    i2c_register_read(MCP0_ADDR, MCP_REG_ADDR(MCP23XXX_GPIO, 1), gpioB, sizeof(gpioB));
    i2c_register_write_byte(MCP0_ADDR, MCP_REG_ADDR(MCP23XXX_GPIO, 1), gpioB[0] & ~INTMOD_PWREN_BIT);
}

bool IS_INTERNAL_MODULE_ON(void) {
    uint8_t gpioB[1] = {0};
    i2c_register_read(MCP0_ADDR, MCP_REG_ADDR(MCP23XXX_GPIO, 1), gpioB, sizeof(gpioB));
    return (0 != (gpioB[0] & INTMOD_PWREN_BIT));
}

void pwrOff()
{
    TRACE("Power off");
    RTOS_WAIT_MS(200);
    i2c_register_write_byte(MCP0_ADDR, MCP_REG_ADDR(MCP23XXX_GPIO, 1), PWR_OFF_BIT); // keep pwr on but make int module off first
    i2c_register_write_byte(MCP0_ADDR, MCP_REG_ADDR(MCP23XXX_IODIR, 1), (uint8_t)((~(INTMOD_PWREN_BIT | PWR_OFF_BIT)) & 0xFF));
    i2c_register_write_byte(MCP0_ADDR, MCP_REG_ADDR(MCP23XXX_GPIO, 1), 0); // Power off
    while (1) RTOS_WAIT_MS(20); // should never return
}

bool pwrPressed()
{
  return (pwr_btn_state != PWR_BTN_RELEASED_AFTER_DOWN);
}
