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

static void esp32_rmt_ctx_free(rmt_ctx_t *ctx);

#if 0
static void esp32_rmt_tx_task(void * pdata) {
    rmt_ctx_t *ctx = (rmt_ctx_t *)pdata;
    rmt_transmit_config_t txcfg = {0};
    rmt_enable(ctx->rmt);
    while(!ctx->exit) {
        rmt_transmit(ctx->rmt, ctx->encoder, ctx->data, count, &txcfg);
        rmt_tx_wait_all_done(ctx->rmt, -1);
    }
    esp32_rmt_ctx_free(ctx); // done with the ctx
    vTaskDelete(NULL);
}
#endif

static void esp32_rmt_rx_task(void * pdata) {
    rmt_ctx_t *ctx = (rmt_ctx_t *)pdata;
    rmt_symbol_word_t raw_symbols[512];
    rmt_rx_done_event_data_t rx_data;

    rmt_enable(ctx->rmt);
    while(!ctx->exit) {
        // ready to receive
        ESP_ERROR_CHECK(rmt_receive(ctx->rmt, raw_symbols, sizeof(raw_symbols), &ctx->rx_cfg));
        xQueueReceive(ctx->rxQueue, &rx_data, portMAX_DELAY);
        if (!ctx->exit) {
            ctx->decoder(ctx, &rx_data);
        }
     }
    esp32_rmt_ctx_free(ctx); // done with the ctx
    vTaskDelete(NULL);
}

rmt_ctx_t *esp32_rmt_tx_init(rmt_ctx_t *ctxmem, int pin, rmt_reserve_memsize_t memsize, float tick_in_ns, rmt_tx_encode_cb_t enc_fn, size_t pulse_in_frame) {
    rmt_ctx_t *rvalue = ctxmem;
#if 0
    if (NULL != rvalue) {
        rvalue->encoder = enc_fn;
        rvalue->exit = false;
        rvalue->rmt = rmtInit(pin, RMT_TX_MODE, memsize);
        rvalue->data = (rmt_data_t *)malloc(sizeof(rmt_data_t) * pulse_in_frame);
        if ((NULL != rvalue->rmt) && (NULL != rvalue->data)) {
            rvalue->tick_in_ns = rmtSetTick(rvalue->rmt, tick_in_ns);
            xTaskCreateStaticPinnedToCore(esp32_rmt_tx_task, "esp32_rmt_tx_task", sizeof(rvalue->rmt_task_stack)/sizeof(rvalue->rmt_task_stack[0]),
                    rvalue, 5, rvalue->rmt_task_stack, rvalue->task_struct, TRAINER_PPM_OUT_TASK_CORE); // TODO-feather priority
        } else {
            free(rvalue);
            rvalue = NULL;
        }
    }
#endif
    return rvalue;
}

IRAM_ATTR static bool rmt_rx_done_callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t receive_queue = (QueueHandle_t)user_data;
    // send the received RMT symbols to the parser task
    xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

rmt_ctx_t *esp32_rmt_rx_init(rmt_ctx_t *ctxmem, int pin, rmt_reserve_memsize_t memsize, float tick_in_ns, rmt_rx_decode_cb_t dec_fn, size_t pulse_in_frame, size_t idle_threshold_in_ns, size_t min_pulse_in_ns) {
    rmt_ctx_t *rvalue = ctxmem;

    if (NULL != rvalue) {
        rmt_rx_channel_config_t rx_channel_cfg = {
            .gpio_num = (gpio_num_t)pin,
            .clk_src = RMT_CLK_SRC_DEFAULT,
            .resolution_hz = (uint32_t)(1000000000 / tick_in_ns),
            .mem_block_symbols = memsize, // amount of RMT symbols that the channel can store at a time
        };
        ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_channel_cfg, &rvalue->rmt));

        rvalue->decoder = dec_fn;
        rvalue->rxQueue = xQueueCreate(1, sizeof(rmt_rx_done_event_data_t));
        rvalue->exit = false;
        if ((NULL != rvalue->rmt) && (NULL != rvalue->rxQueue)) {
            rvalue->rx_cfg.signal_range_min_ns = min_pulse_in_ns;
            rvalue->rx_cfg.signal_range_max_ns = idle_threshold_in_ns;
            xTaskCreateStaticPinnedToCore(esp32_rmt_rx_task, "esp32_rmt_rx_task", sizeof(rvalue->rmt_task_stack)/sizeof(rvalue->rmt_task_stack[0]),
                    rvalue, 5, rvalue->rmt_task_stack, rvalue->task_struct, TRAINER_PPM_OUT_TASK_CORE); // TODO-ESP32 priority
            rmt_rx_event_callbacks_t cbs = {
                .on_recv_done = rmt_rx_done_callback,
            };
            ESP_ERROR_CHECK(rmt_rx_register_event_callbacks(rvalue->rmt, &cbs, rvalue->rxQueue));
        } else {
            free(rvalue);
            rvalue = NULL;
        }
    }
    return rvalue;
}

void esp32_rmt_stop(rmt_ctx_t *ctx) {
    ctx->exit = true;
    if (NULL != ctx->rxQueue) {
        rmt_rx_done_event_data_t end = {0};
        xQueueSend(ctx->rxQueue, &end, 0); // doesn't matter what was sent, just send to end the loop
    }
    // TODO-v5: no thing special need to be done for TX. It will end after the last frame sends out
}

static void esp32_rmt_ctx_free(rmt_ctx_t *ctx) {
    if (NULL != ctx) {
        rmt_del_channel(ctx->rmt);
        free(ctx);
    }
}

int rmt_ppm_decode_cb(rmt_ctx_t *ctx, rmt_symbol_word_t *rxdata, size_t rxdata_len, int16_t *ppm_decode_buf)
{
    int channel = 0;
    uint32_t high = 0U;
    for (size_t i = 0; i < rxdata_len; i++) {
        if (i != 0) {
            uint32_t val = (high + rxdata[i].duration0) * (ctx->tick_in_ns / 1000); // result in us
            if (val > 800 && val < 2200) {
                ppm_decode_buf[channel++] =
                        (int16_t)(val - 1500) * (g_eeGeneral.PPM_Multiplier+10) / 10;  // +-500 != 512, but close enough.
            } else {
                channel = -1;
                break;
            }
        }
        high = rxdata[i].duration1;
    }
    return channel;
}

void rmt_ppm_encode_cb(rmt_ctx_t *ctx, uint16_t *ppm, size_t len)
{
#if 0
    // setupPulsesPPM() always setup data with 0.5us tick
    for (int i = 0; i < len; i++) {
        ctx->data[i].duration0 = ppm[i] - 600;
        ctx->data[i].level0 = 1;
        ctx->data[i].duration1 = 600;
        ctx->data[i].level1 = 0;
    }
    ctx->data[len].val = 0;
#endif
}
