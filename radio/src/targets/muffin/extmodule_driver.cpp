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

void extmoduleStop()
{
  EXTERNAL_MODULE_OFF();
}

static void config_ppm_output(uint16_t ppm_delay, bool polarity)
{
  // PPM generation principle:
  //
  // Hardware timer in PWM mode is used for PPM generation
  // Output is OFF if CNT<CCR1(delay) and ON if bigger
  // CCR1 register defines duration of pulse length and is constant
  // AAR register defines duration of each pulse, it is
  // updated after every pulse in Update interrupt handler.
}

void extmodulePpmStart(uint16_t ppm_delay, bool polarity)
{
  EXTERNAL_MODULE_ON();
  config_ppm_output(ppm_delay, polarity);
}

void extmoduleSendNextFramePpm(void* pulses, uint16_t length,
                               uint16_t ppm_delay, bool polarity)
{
#if 0
  if (!stm32_pulse_if_not_running_disable(&extmoduleTimer))
    return;

  // Set polarity
  stm32_pulse_set_polarity(&extmoduleTimer, !polarity);

  // Start DMA request and re-enable timer
  stm32_pulse_start_dma_req(&extmoduleTimer, pulses, length,
                            LL_TIM_OCMODE_PWM1, ppm_delay * 2);
#endif
}

#if defined(PXX1)
void extmodulePxx1PulsesStart()
{
  EXTERNAL_MODULE_ON();
#if 0
  stm32_pulse_config_output(&extmoduleTimer, false, LL_TIM_OCMODE_PWM1, 9 * 2);
  stm32_pulse_init(&extmoduleTimer);
#endif
}

void extmoduleSendNextFramePxx1(const void* pulses, uint16_t length)
{
#if 0
  if (!stm32_pulse_if_not_running_disable(&extmoduleTimer)) return;

  // Start DMA request and re-enable timer
  stm32_pulse_start_dma_req(&extmoduleTimer, pulses, length, LL_TIM_OCMODE_PWM1,
                            9 * 2);
#endif
}
#endif

// TODO: polarity?
void extmoduleSerialStart()
{
  EXTERNAL_MODULE_ON();
#if 0
  stm32_pulse_init(&extmoduleTimer);
  stm32_pulse_config_output(&extmoduleTimer, true, LL_TIM_OCMODE_TOGGLE, 0);
#endif
}

void extmoduleSendNextFrameSoftSerial(const void* pulses, uint16_t length, bool polarity)
{
#if 0
  if (!stm32_pulse_if_not_running_disable(&extmoduleTimer))
    return;

  // Set polarity
  stm32_pulse_set_polarity(&extmoduleTimer, polarity);
  
  // Start DMA request and re-enable timer
  stm32_pulse_start_dma_req(&extmoduleTimer, pulses, length, LL_TIM_OCMODE_TOGGLE, 0);
#endif
}

void extmoduleInitTxPin()
{
#if 0
  LL_GPIO_InitTypeDef pinInit;
  LL_GPIO_StructInit(&pinInit);
  pinInit.Pin = extmoduleTimer.GPIO_Pin;
  pinInit.Mode = LL_GPIO_MODE_OUTPUT;
  pinInit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  pinInit.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(extmoduleTimer.GPIOx, &pinInit);
#endif
}

// Delay based byte sending @ 57600 bps
void extmoduleSendInvertedByte(uint8_t byte)
{
#if 0
  uint16_t time;
  uint32_t i;

  __disable_irq();
  time = getTmr2MHz();
  LL_GPIO_SetOutputPin(EXTMODULE_TX_GPIO, EXTMODULE_TX_GPIO_PIN);
  while ((uint16_t) (getTmr2MHz() - time) < 34)	{
    // wait
  }
  time += 34;
  for (i = 0; i < 8; i++) {
    if (byte & 1) {
      LL_GPIO_ResetOutputPin(EXTMODULE_TX_GPIO, EXTMODULE_TX_GPIO_PIN);
    }
    else {
      LL_GPIO_SetOutputPin(EXTMODULE_TX_GPIO, EXTMODULE_TX_GPIO_PIN);
    }
    byte >>= 1 ;
    while ((uint16_t) (getTmr2MHz() - time) < 35) {
      // wait
    }
    time += 35 ;
  }
  LL_GPIO_ResetOutputPin(EXTMODULE_TX_GPIO, EXTMODULE_TX_GPIO_PIN);
  __enable_irq();	// No need to wait for the stop bit to complete
  while ((uint16_t) (getTmr2MHz() - time) < 34) {
    // wait
  }
#endif
}
