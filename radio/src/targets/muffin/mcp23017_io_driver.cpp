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
#include "mcp23xxx.h"

extern i2c_master_bus_handle_t gpioext_i2c_bus_handle;
static i2c_master_dev_handle_t mcp0 = NULL;
static i2c_master_dev_handle_t mcp1 = NULL;

#define MCP_REG_ADDR(baseAddr, port) ((baseAddr << 1) | port)

/* ============== end chip definitions ==================== */

#define INTMOD_PWREN_BIT (0x80U)

#define MCP1_SWITCHES_MASK 0xF8U
#define MCP1_KEYS_MASK 0x7U
#define MCP1_TRIM_MASK 0xFFU

static uint32_t gpio_trims = 0U;
  // ~0x02 thr trim down
  // ~0x01 thr trim up
  // ~0x08 ele trim down
static uint32_t switches = 0U;
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

#define PWR_BTN_BIT (1 << 25) // G1B1
#define PWR_OFF_BIT 0x40
#define PWR_BTN_DOWN_AT_START 1
#define PWR_BTN_RELEASED_AT_START 2
#define PWR_BTN_DOWN 3
#define PWR_BTN_RELEASED_AFTER_DOWN 4

#if 0
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
#endif
static bool pwr_switch_on = true;
static const uint32_t key_mapping[32] {
    [0] = (1 << KEY_TELE),  // G0A0
    [1] = (1 << KEY_SYS),    // G0A1
    [2] = (1 << KEY_EXIT),  // G0A2
    [3] = 0,
    [4] = 0,
    [5] = 0,
    [6] = 0,
    [7] = 0,

    [8] = 0,
    [9] = (1 << KEY_ENTER),  // G0B1
    [10] = 0,
    [11] = 0,
    [12] = 0,
    [13] = 0,
    [14] = 0,
    [15] = 0,

    [16] = 0,
    [17] = 0,
    [18] = 0,
    [19] = 0,
    [20] = 0,
    [21] = 0,
    [22] = 0,
    [23] = 0,

    [24] = (1 << KEY_MODEL),  // G1B0
};

static const uint32_t trim_mapping[32] {
    [0] = 0,  // G0A0
    [1] = 0,    // G0A1
    [2] = 0,  // G0A2
    [3] = 0,
    [4] = 0,
    [5] = 0,
    [6] = 0,
    [7] = 0,

    [8] = 0,
    [9] = 0,  // G0B1
    [10] = 0,
    [11] = 0,
    [12] = 0,
    [13] = 0,
    [14] = 0,
    [15] = 0,

    [16] = (1 << ((4 * 2) + 1)), // G1A0 - CH4+
    [17] = (1 << (4 * 2)),       // G1A1 - CH4-
    [18] = (1 << ((1 * 2) + 1)), // G1A2 - CH1+
    [19] = (1 << (1 * 2)),       // G1A3 - CH1-
    [20] = (1 << ((2 * 2) + 1)), // G1A4 - CH2+
    [21] = (1 << (2 * 2)),       // G1A5 - CH2-
    [22] = (1 << ((3 * 2) + 1)), // G1A6 - CH3+
    [23] = (1 << (3 * 2)),       // G1A7 - CH3-

    [24] = 0,  // G1B0
};

uint32_t readKeys()
{
    uint32_t result = 0;
    esp_err_t err = ESP_OK;

    uint32_t gpio_all = 0U;
    uint8_t *gpioAB = (uint8_t *)&gpio_all;

    err = i2c_register_read(mcp0, MCP_REG_ADDR(MCP23XXX_GPIO, 0), gpioAB, 2);
    if (ESP_OK == err) {
        gpioAB += 2;
        err = i2c_register_read(mcp1, MCP_REG_ADDR(MCP23XXX_GPIO, 0), gpioAB, 2);
    }

    if (ESP_OK == err) {
        for (int i = 0; i < 32; i++) {
            if (0 == (gpio_all & (1 << i))) {
                result |= key_mapping[i];
                gpio_trims |= trim_mapping[i];
            }
        }
    }

    pwr_switch_on = (0 != (gpio_all & PWR_BTN_BIT));
    return result;
}

uint32_t readTrims()
{
  uint32_t result = gpio_trims;
  return result;
}


#define MCP0_A_PULLUP_MASK 0xFF
#define MCP0_A_DIR_MASK 0xFF
#define MCP0_B_PULLUP_MASK 0xFF
#define MCP0_B_DIR_MASK 0xFF
#define MCP1_A_PULLUP_MASK 0xFF
#define MCP1_A_DIR_MASK 0xFF
#define MCP1_B_PULLUP_MASK 0xBF
#define MCP1_B_DIR_MASK 0x03
#define MCP1_B_INIT_DATA 0x5C

void keysInit()
{
    i2c_device_config_t i2c_dev0_conf = {
        .device_address = MCP23XXX_ADDR,
        .scl_speed_hz = 400000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(gpioext_i2c_bus_handle, &i2c_dev0_conf, &mcp0));

    i2c_device_config_t i2c_dev1_conf = {
        .device_address = MCP23XXX_ADDR + 1,
        .scl_speed_hz = 400000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(gpioext_i2c_bus_handle, &i2c_dev1_conf, &mcp1));

    esp_err_t ret  = 0;
    ret = i2c_register_write_byte(mcp0, MCP_REG_ADDR(MCP23XXX_GPPU, 0), MCP0_A_PULLUP_MASK);
    if (ESP_OK == ret) {
        ret = i2c_register_write_byte(mcp0, MCP_REG_ADDR(MCP23XXX_GPPU, 0), MCP0_A_PULLUP_MASK); // TODO: for some reason need to set pull up twice?
    }
    if (ESP_OK == ret) {
        ret = i2c_register_write_byte(mcp0, MCP_REG_ADDR(MCP23XXX_IODIR, 0), MCP0_A_DIR_MASK);
    }

    if (ESP_OK == ret) {
        ret = i2c_register_write_byte(mcp0, MCP_REG_ADDR(MCP23XXX_GPPU, 1), MCP0_B_PULLUP_MASK);
    }
    if (ESP_OK == ret) {
        ret = i2c_register_write_byte(mcp0, MCP_REG_ADDR(MCP23XXX_IODIR, 1), MCP0_B_DIR_MASK);
    }

    if (ESP_OK == ret) {
        ret = i2c_register_write_byte(mcp1, MCP_REG_ADDR(MCP23XXX_GPPU, 0), MCP1_A_PULLUP_MASK);
    }
    if (ESP_OK == ret) {
        ret = i2c_register_write_byte(mcp1, MCP_REG_ADDR(MCP23XXX_GPPU, 0), MCP1_A_PULLUP_MASK); // TODO: for some reason need to set pull up twice?
    }
    if (ESP_OK == ret) {
        ret = i2c_register_write_byte(mcp1, MCP_REG_ADDR(MCP23XXX_IODIR, 0), MCP1_A_DIR_MASK);
    }

    if (ESP_OK == ret) {
        ret = i2c_register_write_byte(mcp1, MCP_REG_ADDR(MCP23XXX_GPPU, 1), MCP1_B_PULLUP_MASK);
    }
    if (ESP_OK == ret) {
        ret = i2c_register_write_byte(mcp1, MCP_REG_ADDR(MCP23XXX_IODIR, 1), MCP1_B_DIR_MASK);
    }
    if (ESP_OK == ret) {
        ret = i2c_register_write_byte(mcp1, MCP_REG_ADDR(MCP23XXX_GPIO, 1), MCP1_B_INIT_DATA);
    }

    if (ESP_OK != ret) {
        TRACE_ERROR("Error during MCP23017 init");
    } else {
        TRACE("MCP23017 initialized");
    }
}

void INTERNAL_MODULE_ON(void) {
#if 0
    uint8_t gpioB[1] = {0};
    i2c_register_read(mcp0, MCP_REG_ADDR(MCP23XXX_GPIO, 1), gpioB, sizeof(gpioB));
    i2c_register_write_byte(mcp0, MCP_REG_ADDR(MCP23XXX_GPIO, 1), gpioB[0] | INTMOD_PWREN_BIT);
#endif
}
void INTERNAL_MODULE_OFF(void) {
#if 0
    uint8_t gpioB[1] = {0};
    i2c_register_read(mcp0, MCP_REG_ADDR(MCP23XXX_GPIO, 1), gpioB, sizeof(gpioB));
    i2c_register_write_byte(mcp0, MCP_REG_ADDR(MCP23XXX_GPIO, 1), gpioB[0] & ~INTMOD_PWREN_BIT);
#endif
}

bool IS_INTERNAL_MODULE_ON(void) {
    uint8_t gpioB[1] = {0};
    i2c_register_read(mcp0, MCP_REG_ADDR(MCP23XXX_GPIO, 1), gpioB, sizeof(gpioB));
    return (0 != (gpioB[0] & INTMOD_PWREN_BIT));
}

void pwrOff()
{
    TRACE("Power off");
    RTOS_WAIT_MS(200);
    //i2c_register_write_byte(mcp1, MCP_REG_ADDR(MCP23XXX_GPIO, 1), PWR_OFF_BIT); // keep pwr on but make int module off first
    //i2c_register_write_byte(mcp1, MCP_REG_ADDR(MCP23XXX_IODIR, 1), (uint8_t)((~(INTMOD_PWREN_BIT | PWR_OFF_BIT)) & 0xFF));
    i2c_register_write_byte(mcp1, MCP_REG_ADDR(MCP23XXX_GPIO, 1), 0); // Power off
    while (1) RTOS_WAIT_MS(20); // should never return
}

bool pwrPressed()
{
  return pwr_switch_on;//(pwr_btn_state != PWR_BTN_RELEASED_AFTER_DOWN);
}
