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

#include "opentx.h"
#include "intmodule_serial_driver.h"
#include "driver/uart.h"

static void get_serial_config(const etx_serial_init* params, uart_config_t *uart_config) {
  uart_config->baud_rate = params->baudrate;

  switch (params->parity) {
  case ETX_Parity_None:
    uart_config->parity = UART_PARITY_DISABLE;
    break;
  case ETX_Parity_Even:
    uart_config->parity = UART_PARITY_EVEN;
    break;
  case ETX_Parity_Odd:
    uart_config->parity = UART_PARITY_ODD;
    break;
  default:
    break;
  }

  switch (params->stop_bits) {
  case ETX_StopBits_One:
    uart_config->stop_bits = UART_STOP_BITS_1;
    break;
  case ETX_StopBits_OneAndHalf:
    uart_config->stop_bits = UART_STOP_BITS_1_5;
    break;
  case ETX_StopBits_Two:
    uart_config->stop_bits = UART_STOP_BITS_2;
    break;
  default:
    break;
  }
}

void* intmoduleSerialStart(const etx_serial_init* params)
{
  uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
  };
  get_serial_config(params, &uart_config);

  // We won't use a buffer for sending data.
  uart_driver_install(INTMOD_UART_PORT, INTMODULE_FIFO_SIZE, 0, 0, NULL, ESP_INTR_FLAG_SHARED);
  uart_param_config(INTMOD_UART_PORT, &uart_config);
  uart_set_pin(INTMOD_UART_PORT, INTMOD_TX_PIN, INTMOD_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

  return (void *)INTMOD_UART_PORT;
}

void intmoduleSendByte(void* ctx, uint8_t byte)
{
  uint8_t data = byte;
  uart_write_bytes(INTMOD_UART_PORT, &data, 1);
}

void intmoduleSendBuffer(void* ctx, const uint8_t * data, uint8_t size)
{
  uart_write_bytes(INTMOD_UART_PORT, data, size);
}

void intmoduleWaitForTxCompleted(void* ctx)
{
  uart_wait_tx_done(INTMOD_UART_PORT, portMAX_DELAY);
}

static int intmoduleGetByte(void* ctx, uint8_t* data)
{
  return uart_read_bytes(INTMOD_UART_PORT, data, 1, 0);
}

static void intmoduleClearRxBuffer(void* ctx)
{
  uart_flush(INTMOD_UART_PORT);
}

static void intmoduleSerialStop(void* ctx)
{
  uart_driver_delete(INTMOD_UART_PORT);
}

void intmoduleStop()
{
  intmoduleSerialStop((void *)INTMOD_UART_PORT);
}

const etx_serial_driver_t IntmoduleSerialDriver = {
  .init = intmoduleSerialStart,
  .deinit = intmoduleSerialStop,
  .sendByte = intmoduleSendByte,
  .sendBuffer = intmoduleSendBuffer,
  .waitForTxCompleted = intmoduleWaitForTxCompleted,
  .getByte = intmoduleGetByte,
  .clearRxBuffer = intmoduleClearRxBuffer,
  .getBaudrate = nullptr,
  .setReceiveCb = nullptr,
  .setBaudrateCb = nullptr,
};
