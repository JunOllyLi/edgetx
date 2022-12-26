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
#include "pulses/pulses.h"

void intmoduleStop()
{
}

// #define HEARBEAT_OFFSET unsigned(6000 + g_model.flightModeData[0].gvars[0] * 100)
constexpr unsigned HEARBEAT_OFFSET = 6000;

void intmoduleSendNextFrame()
{
}

void intmodulePxx1PulsesStart()
{
}

#if defined(INTERNAL_MODULE_PPM)
void intmodulePpmStart()
{
}
#endif // defined(INTERNAL_MODULE_PPM)

extern "C" void INTMODULE_DMA_STREAM_IRQHandler()
{
}

extern "C" void INTMODULE_TIMER_CC_IRQHandler()
{
}
