
#ifndef ESP32_COMMON_H_
#define ESP32_COMMON_H_

#include <rtos.h>
#include <inttypes.h>
#include "definitions.h"
#include "opentx_constants.h"
#include "hal.h"
#include "hal/serial_port.h"
#include "hal/watchdog_driver.h"

#define SYSTEM_TICKS_1MS pdMS_TO_TICKS(1)

static inline uint32_t ticksNow() {
#if defined(ESP_PLATFORM)
    return xTaskGetTickCount();
#else
    // for yaml_data
    return 0;
#endif
}

void init2MhzTimer();

#endif // ESP32_COMMON_H
