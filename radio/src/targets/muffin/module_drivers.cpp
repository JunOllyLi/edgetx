/*
 * Copyright (C) EdgeTX
 *
 * Based on code named
 *   opentx - https://github.com/opentx/opentx
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

#include "hal.h"
#include "hal/serial_driver.h"
#include "hal/module_port.h"
#include "dataconstants.h"
#include "debug.h"
#include "esp32_uart_driver.h"

#if defined(HARDWARE_INTERNAL_MODULE)
static const etx_esp32_uart_hw_def_t intmod_uart_hw_def = {
    .uart_port = INTMOD_UART_PORT,
    .rx_pin = INTMOD_ESP_UART_RX,
    .tx_pin = INTMOD_ESP_UART_TX,
    .fifoSize = 4096,
    .queueSize = 10
};

const etx_module_port_t _internal_ports[] = {
  {
    .port = ETX_MOD_PORT_UART,
    .type = ETX_MOD_TYPE_SERIAL,
    .dir_flags = ETX_MOD_DIR_TX_RX | ETX_MOD_FULL_DUPLEX,
    .drv = { .serial = &ESPUartSerialDriver },
    .hw_def = (void *)&intmod_uart_hw_def,
  },
};

static void _set_internal_module_power(uint8_t on) {
    if (on) {
        INTERNAL_MODULE_ON();
    } else {
        INTERNAL_MODULE_OFF();
    }
}

static const etx_module_t _internal_module = {
  .ports = _internal_ports,
  .set_pwr = _set_internal_module_power,
  .set_bootcmd = nullptr,
  .n_ports = DIM(_internal_ports),
};
#endif

#if defined(HARDWARE_EXTERNAL_MODULE)
#if 0
static const etx_esp32_uart_hw_def_t extmod_uart_hw_def = {
    .uart_port = EXTMOD_UART_PORT,
    .rx_pin = EXTMOD_UART_RX,
    .tx_pin = EXTMOD_UART_TX,
    .fifoSize = 4096,
    .queueSize = 10
};
#else
static const etx_rmt_uart_hw_def_t extmod_uart_hw_def = {
    .rx_pin = EXTMOD_UART_RX,
    .tx_pin = EXTMOD_UART_TX,
    .memsize = 256,
    .rx_task_stack_size = 1024 * 6,
    .resolution_hz = 80000000,
    .idle_threshold_in_ns = 180000,
    .min_pulse_in_ns = 1000,
};
#endif
/**
 * This driver is specific for Muffin, which connected the 4in1 MULTI internal module to
 * EXTMODULE port. This is to trick the multi.cpp thinks it is a serial port for the
 * MULTI external modules (which uses inverted TX and SPORT for RX?)
 */
static void* proxyExtUartSerialStart(void *hw_def, const etx_serial_init* params);
static void proxyExtUartSerialStop(void* ctx);
etx_serial_driver_t proxyExtUartSerialDriver = {
    .init = proxyExtUartSerialStart,
    .deinit = proxyExtUartSerialStop,
};

static void *real_pUart = NULL;
static int real_pUart_cnt = 0;
static void* proxyExtUartSerialStart(void *hw_def, const etx_serial_init* params) {
    if (NULL == real_pUart) {
        etx_serial_init cfg;
        memcpy(&cfg, params, sizeof(etx_serial_init));
        cfg.polarity = ETX_Pol_Normal;
        cfg.direction = ETX_Dir_TX_RX;
        real_pUart = rmtuartSerialDriver.init(hw_def, &cfg);
        memcpy(&proxyExtUartSerialDriver, &rmtuartSerialDriver, sizeof(etx_serial_driver_t));
        proxyExtUartSerialDriver.init = proxyExtUartSerialStart;
        proxyExtUartSerialDriver.deinit = proxyExtUartSerialStop;
    }
    real_pUart_cnt++;
    return real_pUart;
}

static void proxyExtUartSerialStop(void* ctx)
{
    real_pUart_cnt--;
    if (real_pUart_cnt == 0) {
        rmtuartSerialDriver.deinit(ctx);
        real_pUart = NULL;
    }
}

static void proxyExtUartSerialSetInverted(uint8_t enable) {
    // do nothing.
}

const etx_module_port_t _external_ports[] = {
  {
    .port = ETX_MOD_PORT_UART,
    .type = ETX_MOD_TYPE_SERIAL,
    .dir_flags = ETX_MOD_DIR_TX,
    .drv = { .serial = &proxyExtUartSerialDriver },
    .hw_def = (void *)&extmod_uart_hw_def,
    .set_inverted = proxyExtUartSerialSetInverted,
  },
  {
    .port = ETX_MOD_PORT_SPORT,
    .type = ETX_MOD_TYPE_SERIAL,
    .dir_flags = ETX_MOD_DIR_RX,
    .drv = { .serial = &proxyExtUartSerialDriver },
    .hw_def = (void *)&extmod_uart_hw_def,
  },
};

static void _set_external_module_power(uint8_t on) {
    if (on) {
        EXTERNAL_MODULE_ON();
    } else {
        EXTERNAL_MODULE_OFF();
    }
}

static const etx_module_t _external_module = {
  .ports = _external_ports,
  .set_pwr = _set_external_module_power,
  .set_bootcmd = nullptr,
  .n_ports = DIM(_external_ports),
};
#endif


BEGIN_MODULES()
#if defined(HARDWARE_INTERNAL_MODULE)
  &_internal_module,
#else
  nullptr,
#endif
#if defined(HARDWARE_EXTERNAL_MODULE)
  &_external_module,
#else
  nullptr,
#endif
END_MODULES()

