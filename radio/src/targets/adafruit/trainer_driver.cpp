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
#include "Arduino.h"
#if defined(ARDUINO_ADAFRUIT_FEATHER_ESP32_V2)
#include "esp32_rmt_pulse.h"

static rmt_ctx_t* rmt_send = NULL;
static rmt_ctx_t* rmt_recv = NULL;
#else
#endif

#if !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32_V2)
static void trainer_gpio_isr(void) {
  uint16_t capture = getTmr2MHz();
  captureTrainerPulses(capture);
}
#else
static void ppm_trainer_decode_cb(rmt_ctx_t *ctx, uint32_t *rxdata, size_t rxdata_len)
{
    int16_t ppm[MAX_TRAINER_CHANNELS];
    int channel = rmt_ppm_decode_cb(ctx, rxdata, rxdata_len, ppm);
    if (channel > 0) {
        ppmInputValidityTimer = PPM_IN_VALID_TIMEOUT;
        memcpy(ppmInput, ppm, sizeof(ppm));
    }
}
#endif

void init_trainer_capture()
{
#if defined(ARDUINO_ADAFRUIT_FEATHER_ESP32_V2)
    rmt_recv = esp32_rmt_rx_init(TRAINER_IN_GPIO, RMT_MEM_64,
            RMT_PPM_IN_TICK_NS,
            ppm_trainer_decode_cb,
            MAX_TRAINER_CHANNELS,
            RMT_PPM_IDLE_THRESHOLD_NS);
#else
    attachInterrupt(TRAINER_IN_GPIO, trainer_gpio_isr, RISING);
#endif
}

void stop_trainer_capture()
{
#if defined(ARDUINO_ADAFRUIT_FEATHER_ESP32_V2)
    esp32_rmt_stop(rmt_recv);
#else
    detachInterrupt(TRAINER_IN_GPIO);
#endif
}

#if defined(ARDUINO_ADAFRUIT_FEATHER_ESP32_V2)
static size_t esp32_rmt_ppm_encode_cb(rmt_ctx_t *ctx) {
    setupPulsesPPMTrainer();
    size_t count = 0;
    while (0 != trainerPulsesData.ppm.pulses[count]) {
        count++;
    }
    rmt_ppm_encode_cb(ctx, (uint16_t *)trainerPulsesData.ppm.pulses, count);
    return count;
}
#endif

void init_trainer_ppm()
{
#if defined(ARDUINO_ADAFRUIT_FEATHER_ESP32_V2)
    rmt_send = esp32_rmt_tx_init(RMT_TX_PIN, RMT_MEM_64,
            RMT_PPM_OUT_TICK_NS,
            esp32_rmt_ppm_encode_cb,
            MAX_TRAINER_CHANNELS + 2); // extra two for idle pulse and termination
#endif
}

void stop_trainer_ppm()
{
#if defined(ARDUINO_ADAFRUIT_FEATHER_ESP32_V2)
    esp32_rmt_stop(rmt_send);
#endif
}

#if defined(TRAINER_MODULE_CPPM)
void init_trainer_module_cppm()
{
}
#endif

#if defined(TRAINER_MODULE_SBUS)
void init_trainer_module_sbus()
{
}

void stop_trainer_module_sbus()
{
}
#endif

#if defined(SBUS_TRAINER)
int sbusGetByte(uint8_t * byte)
{
    return 0;
}
#endif
