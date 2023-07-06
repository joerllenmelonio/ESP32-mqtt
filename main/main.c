#include <string.h>
#include <stdio.h>
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "../definition.h"
#include "../components/blink_control/include/blink_control.h"
#include "../components/mqtt_control/include/mqtt_control.h"
#include "../components/switch_control/include/switch_control.h"
#include "../components/wifi_control/include/wifi_control.h"
#include "../components/ulp_code/include/ulp_code.h"

QueueHandle_t xQueueSwitch;
QueueHandle_t xQueueCount;

void vTaskLed(void *pvParameters);
void vTaskSwitch(void *pvParameters);
void vTaskWifi(void *pvParameters);
void Publisher_Task(void *pvParameters);

void init_queue(void){
    ESP_ERROR_CHECK(nvs_flash_init());
    init();

    xQueueCount = xQueueCreate( 5, sizeof(info_count) );
    xQueueSwitch = xQueueCreate(5 , sizeof(info_led) );
}

void app_main(void)
{
    init_queue();

    xTaskCreatePinnedToCore( vTaskLed, "TaskLed", configMINIMAL_STACK_SIZE + 2048, NULL, 4, NULL, 0 );
    xTaskCreatePinnedToCore( vTaskSwitch, "TaskSwitch", configMINIMAL_STACK_SIZE + 2048, NULL, 4, NULL, 0 );
    xTaskCreatePinnedToCore( vTaskWifi, "TaskWifi", configMINIMAL_STACK_SIZE + 2048, NULL, 6, NULL, 1);
    xTaskCreatePinnedToCore( Publisher_Task, "Publisher_Task", configMINIMAL_STACK_SIZE + 1024 * 5, NULL, 5, NULL, 1 );
}