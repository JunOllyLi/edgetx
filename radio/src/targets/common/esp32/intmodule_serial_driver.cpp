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
#include "esp_log.h"
#define TAG "INTMOD_UART"

static void get_serial_config(const etx_serial_init* params, uart_config_t *uart_config) {
  uart_config->baud_rate = params->baudrate;

  switch (params->encoding) {
  case ETX_Encoding_8N1:
    uart_config->parity = UART_PARITY_DISABLE;
    uart_config->stop_bits = UART_STOP_BITS_1;
    break;
  case ETX_Encoding_8E2:
    uart_config->parity = UART_PARITY_EVEN;
    uart_config->stop_bits = UART_STOP_BITS_2;
    break;
  default:
    break;
  }
}

static void (*intmod_on_idle_cb)(void *) = NULL;
static void *intmod_on_idle_cb_param = NULL;

static void intmod_setIdleCb(void* ctx, void (*on_idle)(void*), void* param) {
  intmod_on_idle_cb = on_idle;
  intmod_on_idle_cb_param = param;
}

static QueueHandle_t intmod_uart_queue;
static volatile bool intmod_uart_started = false;
static void intmod_uart_event_task(void *pvParameters)
{
    uart_event_t event;
    size_t buffered_size;
    while (intmod_uart_started) {
        //Waiting for UART event.
        if (xQueueReceive(intmod_uart_queue, (void *)&event, (TickType_t)portMAX_DELAY)) {
            switch (event.type) {
            //Event of UART receving data
            /*We'd better handler data event fast, there would be much more data events than
            other types of events. If we take too much time on data event, the queue might
            be full.*/
            case UART_DATA:
                //ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                if (NULL != intmod_on_idle_cb) {
                  intmod_on_idle_cb(intmod_on_idle_cb_param);
                }
                break;
            //Event of HW FIFO overflow detected
            case UART_FIFO_OVF:
                ESP_LOGI(TAG, "hw fifo overflow");
                // If fifo overflow happened, you should consider adding flow control for your application.
                // The ISR has already reset the rx FIFO,
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(INTMOD_UART_PORT);
                xQueueReset(intmod_uart_queue);
                break;
            //Event of UART ring buffer full
            case UART_BUFFER_FULL:
                ESP_LOGI(TAG, "ring buffer full");
                // If buffer full happened, you should consider increasing your buffer size
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(INTMOD_UART_PORT);
                xQueueReset(intmod_uart_queue);
                break;
            //Event of UART RX break detected
            case UART_BREAK:
                ESP_LOGI(TAG, "uart rx break");
                if (NULL != intmod_on_idle_cb) {
                  intmod_on_idle_cb(intmod_on_idle_cb_param);
                }
                break;
            //Event of UART parity check error
            case UART_PARITY_ERR:
                ESP_LOGI(TAG, "uart parity error");
                break;
            //Event of UART frame error
            case UART_FRAME_ERR:
                ESP_LOGI(TAG, "uart frame error");
                break;
            //UART_PATTERN_DET
            case UART_PATTERN_DET:
              {
                uart_get_buffered_data_len(INTMOD_UART_PORT, &buffered_size);
                int pos = uart_pattern_pop_pos(INTMOD_UART_PORT);
                ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
                break;
              }
            //Others
            default:
                ESP_LOGI(TAG, "uart event type: %d", event.type);
                break;
            }
        }
    }
    TRACE("========== INTMOD uart RX task exiting");
    vTaskDelete(NULL);
}

void* intmoduleSerialStart(void *hw_def, const etx_serial_init* params)
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
  uart_driver_install(INTMOD_UART_PORT, INTMODULE_FIFO_SIZE*8, 0, 10, &intmod_uart_queue, ESP_INTR_FLAG_IRAM);
  uart_param_config(INTMOD_UART_PORT, &uart_config);
  uart_set_pin(INTMOD_UART_PORT, INTMOD_ESP_UART_TX, INTMOD_ESP_UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

  intmod_uart_started = true;
  xTaskCreate(intmod_uart_event_task, "intmod_uart_event_task", 4096, NULL, 12, NULL);
  return (void *)INTMOD_UART_PORT;
}

void intmoduleSendByte(void* ctx, uint8_t byte)
{
  uint8_t data = byte;
  uart_write_bytes(INTMOD_UART_PORT, &data, 1);
}

void intmoduleSendBuffer(void* ctx, const uint8_t * data, uint32_t size)
{
  uart_write_bytes(INTMOD_UART_PORT, data, size);
}

void intmoduleWaitForTxCompleted(void* ctx)
{
  uart_wait_tx_done(INTMOD_UART_PORT, portMAX_DELAY);
}

static int intmoduleGetByte(void* ctx, uint8_t* data)
{
  int r = uart_read_bytes(INTMOD_UART_PORT, data, 1, 0);
  return r;
}

static void intmoduleClearRxBuffer(void* ctx)
{
  uart_flush(INTMOD_UART_PORT);
}

static void intmoduleSerialStop(void* ctx)
{
  intmod_uart_started = false;
  uart_driver_delete(INTMOD_UART_PORT);
}

int intmoduleGetBufferedBytes(void* ctx) {
  size_t size = 0U;
  if (ESP_OK != uart_get_buffered_data_len(INTMOD_UART_PORT, &size)) {
    size = 0U;
  }
  return 0;
}

int intmoduleCopyRxBuffer(void* ctx, uint8_t* buf, uint32_t len) {
  int r = uart_read_bytes(INTMOD_UART_PORT, buf, len, 0);
  return r;
}

const etx_serial_driver_t IntmoduleSerialDriver = {
  .init = intmoduleSerialStart,
  .deinit = intmoduleSerialStop,
  .sendByte = intmoduleSendByte,
  .sendBuffer = intmoduleSendBuffer,
  .waitForTxCompleted = intmoduleWaitForTxCompleted,
  .getByte = intmoduleGetByte,
  .getBufferedBytes = intmoduleGetBufferedBytes,
  .copyRxBuffer = intmoduleCopyRxBuffer,
  .clearRxBuffer = intmoduleClearRxBuffer,
  .getBaudrate = nullptr,
  .setReceiveCb = nullptr,
  .setIdleCb = intmod_setIdleCb,
  .setBaudrateCb = nullptr,
};
