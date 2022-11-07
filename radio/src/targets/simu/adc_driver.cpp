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

#include "hal/adc_driver.h"
#include "definitions.h"

#include "hw_inputs.inc"

void enableVBatBridge(){}
void disableVBatBridge(){}
bool isVBatBridgeEnabled(){ return false; }

const char* adcGetStickName(uint8_t idx)
{
  if (idx >= DIM(_stick_inputs)) return "";
  return _stick_inputs[idx];
}

const char* adcGetPotName(uint8_t idx)
{
  if (idx >= DIM(_pot_inputs)) return "";
  return _pot_inputs[idx];
}

uint8_t adcGetMaxSticks()
{
  return DIM(_stick_inputs);
}

uint8_t adcGetMaxPots()
{
  return DIM(_pot_inputs);
}
