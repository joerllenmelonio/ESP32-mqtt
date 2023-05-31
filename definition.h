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