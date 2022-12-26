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
#include "FreeRTOS_entry.h"
#include "driver/gptimer.h"

static gptimer_handle_t MyTim2Mhz = NULL;
static gptimer_handle_t MyTim5ms = NULL;
static SemaphoreHandle_t sem5ms;

static bool IRAM_ATTR alarm_5ms_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;
    xSemaphoreGiveFromISR(sem5ms, &high_task_awoken);
    // return whether we need to yield at the end of ISR
    return (high_task_awoken == pdTRUE);
}

static void task5ms(void * pdata) {
  static uint8_t pre_scale;       // Used to get 10 Hz counter

  while (true) {
    if (xSemaphoreTake(sem5ms, portMAX_DELAY)) {
#if defined(COLORLCD)
      lv_tick_inc(5);
#endif

      ++pre_scale;

      if (pre_scale == 2) {
        pre_scale = 0;
        per10ms();
      }
    }
  }
}

// Start TIMER at 2000000Hz
void init2MhzTimer()
{
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 2000000, // 2MHz, 1 tick=0.5us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &MyTim2Mhz));

    ESP_ERROR_CHECK(gptimer_enable(MyTim2Mhz));
    ESP_ERROR_CHECK(gptimer_start(MyTim2Mhz));
}

uint16_t getTmr2MHz() {
    uint64_t count = 0;
    gptimer_get_raw_count(MyTim2Mhz, &count);
    return (uint16_t)(count & 0xFFFF);
}

tmr10ms_t get_tmr10ms() {
    uint64_t count = 0;
    gptimer_get_raw_count(MyTim2Mhz, &count);
    return (uint16_t)((count / 20000) & 0xFFFF); // 2MHz => 100Hz
}

#define TIM5MS_STACK_SIZE (1024 * 3)
RTOS_TASK_HANDLE taskId5ms;
RTOS_DEFINE_STACK(taskId5ms, task5ms_stack, TIM5MS_STACK_SIZE);
// Use the same 2MHz timer but with 5ms alarm
void init5msTimer()
{
    sem5ms = xSemaphoreCreateBinary();

    RTOS_CREATE_TASK_EX(taskId5ms,task5ms,"5ms timer",task5ms_stack,TIM5MS_STACK_SIZE,5,TMR_5MS_CORE);  // TODO-feather priority

    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick=1us
        .flags = {
            .intr_shared = true,
        }
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &MyTim5ms));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = alarm_5ms_cb,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(MyTim5ms, &cbs, NULL));

    ESP_ERROR_CHECK(gptimer_enable(MyTim5ms));

    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 5000, // period = 5ms
        .reload_count = 0,
        .flags = {
            .auto_reload_on_alarm = true,
        }
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(MyTim5ms, &alarm_config));
    ESP_ERROR_CHECK(gptimer_start(MyTim5ms));
}

void stop5msTimer()
{
    ESP_ERROR_CHECK(gptimer_set_alarm_action(MyTim5ms, NULL));
    ESP_ERROR_CHECK(gptimer_stop(MyTim5ms));
    ESP_ERROR_CHECK(gptimer_disable(MyTim5ms));
    ESP_ERROR_CHECK(gptimer_del_timer(MyTim5ms));
}

uint32_t ticksNow() {
   return xTaskGetTickCount();
}