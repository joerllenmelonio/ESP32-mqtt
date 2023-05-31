#include <stdio.h>
#include "blink_control.h"
#include "../../definition.h"

#define LED 2

info_led_t status_led;

void gpio_led(int led){
    gpio_reset_pin(led);
    gpio_set_direction(led,GPIO_MODE_OUTPUT);
}

void vTaskLed(void *pvParameters)
{
    while(true){
        xQueueReceive( xQueueSwitch, &status_led, portMAX_DELAY );
        
        int led = status_led.led;
        bool status = status_led.status;

        gpio_led(LED);

        gpio_set_level(led, status);
    }
}