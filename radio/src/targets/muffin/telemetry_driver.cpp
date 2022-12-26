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

Fifo<uint8_t, TELEMETRY_FIFO_SIZE> telemetryFifo;
uint32_t telemetryErrors = 0;

void telemetryPortInit(uint32_t baudrate, uint8_t mode)
{
}

void telemetryPortInvertedInit(uint32_t baudrate)
{
}

inline void telemetryPortInvertedRxBit()
{
}

void telemetryPortSetDirectionOutput()
{
}

void sportWaitTransmissionComplete()
{
}

void telemetryPortSetDirectionInput()
{
}

void sportSendByte(uint8_t byte)
{
}

void sportStopSendByteLoop()
{
}

void sportSendByteLoop(uint8_t byte)
{
}

void sportSendBuffer(const uint8_t * buffer, uint32_t count)
{
}

void check_telemetry_exti()
{
}

// TODO we should have telemetry in an higher layer, functions above should move to a sport_driver.cpp
bool telemetryGetByte(uint8_t * byte)
{
  return false;
}

void telemetryClearFifo()
{
}

bool sportGetByte(uint8_t * byte)
{
  return telemetryFifo.pop(*byte);
}

