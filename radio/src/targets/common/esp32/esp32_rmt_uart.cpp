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
#include "esp32_rmt_pulse.h"

typedef struct {
    rmt_ctx_t *rx;
    rmt_ctx_t *tx;
    etx_serial_init params;
    uint8_t bits_in_frame;
    uint8_t data_bits;
} rmt_uart_t;

static void* esp32_rmt_uart_init(const etx_serial_init* params, int pin) {
    rmt_uart_t *rvalue = (rmt_uart_t *)malloc(sizeof(rmt_uart_t));
    if (NULL != rvalue) {
	memcpy(&rvalue->params, params, sizeof(rvalue->params));
	rvalue->rx = esp32_rmt_rx_init
    }

    return rvalue;
}
