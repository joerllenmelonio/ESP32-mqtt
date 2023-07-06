#ifndef __DEFINITION__H__
#define __DEFINITION__H__

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "nvs.h"
#define CORE_0 0
#define CORE_1 1
#define TIME_DEEP_SLEEP 120

#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc_periph.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/ulp.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/ulp.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/ulp.h"
#endif


extern QueueHandle_t xQueueSwitch;
extern QueueHandle_t xQueueCount;

//definição de structs
typedef struct{
    int led;
    bool status;
}info_led_t;

typedef struct {
    int clicks;
} info_count_t;

extern info_led_t info_led;
extern info_count_t info_count;

#endif