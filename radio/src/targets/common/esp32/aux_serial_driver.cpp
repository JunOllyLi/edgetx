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

#if defined(AUX_SERIAL)

void auxSerialSetup(unsigned int baudrate, bool dma, uint16_t length, uint16_t parity, uint16_t stop)
{
}

static void* aux_serial_init(const etx_serial_init* params)
{
  return NULL;// TODO-v5 (void*)&aux_serial;
}

static void* auxSerialInit(const etx_serial_init* params)
{
  return aux_serial_init(params);
}

static void aux_serial_putc(void* ctx, uint8_t c)
{
}

static void aux_serial_send_buffer(void* ctx, const uint8_t* data, uint8_t size)
{
}

static void aux_wait_tx_completed(void* ctx)
{
  // TODO
  (void)ctx;
}

static void aux1SetRxCb(void*ctx, void (*cb)(uint8_t*, uint32_t))
{
}

static int aux_get_byte(void* ctx, uint8_t* data)
{
  return 0;
}

void auxSerialSbusInit()
{
}

void auxSerialStop()
{
}

void aux_serial_deinit(void* ctx)
{
}

const etx_serial_driver_t AuxSerialDriver = {
  .init = auxSerialInit,
  .deinit = aux_serial_deinit,
  .sendByte = aux_serial_putc,
  .sendBuffer = aux_serial_send_buffer,
  .waitForTxCompleted = aux_wait_tx_completed,
  .getByte = aux_get_byte,
  .getBaudrate = nullptr,
  .setReceiveCb = aux1SetRxCb,
  .setBaudrateCb = nullptr,
};

#endif // AUX_SERIAL

#if defined(AUX2_SERIAL)
uint8_t aux2SerialMode = UART_MODE_COUNT;  // Prevent debug output before port is setup

void aux2SerialSetup(unsigned int baudrate, bool dma, uint16_t length, uint16_t parity, uint16_t stop)
{
}

void aux2SerialInit(unsigned int mode, unsigned int protocol)
{
}

void aux2SerialPutc(char c)
{
}

void aux2SerialSbusInit()
{
}

void aux2SerialStop()
{
}

uint8_t aux2SerialTracesEnabled()
{
#if defined(DEBUG)
  return (aux2SerialMode == UART_MODE_DEBUG);
#else
  return false;
#endif
}

#endif // AUX2_SERIAL

#if defined(AUX_SERIAL)
const etx_serial_port_t auxSerialPort = {
  "AUX1",
  &AuxSerialDriver,
  nullptr
};
#define AUX_SERIAL_PORT &auxSerialPort
#else
#define AUX_SERIAL_PORT nullptr
#endif

#define AUX2_SERIAL_PORT nullptr

static const etx_serial_port_t* serialPorts[MAX_AUX_SERIAL] = {
  AUX_SERIAL_PORT,
  AUX2_SERIAL_PORT,
};

const etx_serial_port_t* auxSerialGetPort(int port_nr)
{
  if (port_nr >= MAX_AUX_SERIAL) return nullptr;
  return serialPorts[port_nr];
}
