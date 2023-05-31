#include <stdio.h>
#include "switch_control.h"
#include "../../definition.h"

info_count_t info_count_click;
info_led_t status_led;

#define SWITCH 4
#define LED 2

void gpio_switch(int button){
    gpio_reset_pin(button);
    gpio_set_direction(button,GPIO_MODE_INPUT);
}

void vTaskSwitch(void *pvParameters)
{
    gpio_switch(SWITCH);

    int count = 0;

    bool stts_led = false;
    bool old_status_switch = 1;

    while(true){
        bool status_switch = gpio_get_level(SWITCH);
    

        if(status_switch != 1 && old_status_switch!=status_switch){
            
            count++;
            printf("O botão foi pressionado %d vez(es)\n",count);

            stts_led = !stts_led;
            status_led.led = LED;
            status_led.status = stts_led;

            if(stts_led==1){

                printf("O led esta ligado.\n");
            }else{
                printf("O led esta desligado.\n");
            }

            info_count_click.clicks = count;
            xQueueSend( xQueueCount, &info_count_click, portMAX_DELAY);
            xQueueSend( xQueueSwitch, &status_led, portMAX_DELAY);
        }
        old_status_switch = status_switch;

        vTaskDelay( 200 / portTICK_PERIOD_MS );
    }
}