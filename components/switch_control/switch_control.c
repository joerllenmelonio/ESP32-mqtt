#include <stdio.h>
#include "string.h"
#include "switch_control.h"
#include "../../definition.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

info_count_t info_count_click;
info_led_t status_led;

#define SWITCH 4  //botão
#define LED 2  //led do esp32

int countClick(int count);

void setFlash(char* information) {
    nvs_handle_t nvsHandle;
    esp_err_t ret = nvs_flash_init();
    ret = nvs_open("storage", NVS_READWRITE, &nvsHandle);
    if (ret != ESP_OK) {
        ESP_LOGI("PUT-ESP", "Erro ao abrir informação na flash.");
    }

    ret = nvs_set_str(nvsHandle, "contagem", information);
    if (ret != ESP_OK) {
        ESP_LOGI("PUT-ESP", "Erro ao gravar informação na flash.");
    }

    ret = nvs_commit(nvsHandle);
    if (ret != ESP_OK) {
        ESP_LOGI("PUT-ESP", "Erro ao salvar informação na flash.");
    }
}

char *getFlash(void) {
    nvs_handle_t nvsHandle;
    esp_err_t ret = nvs_flash_init();
    ret = nvs_open("storage", NVS_READONLY, &nvsHandle);
    if (ret != ESP_OK) {
        // Tratar o erro, se necessário
    }

    char* count_value = NULL; 
    count_value = (char *)malloc(sizeof(char *));

    size_t tamanhoString = sizeof(count_value);
    ret = nvs_get_str(nvsHandle, "contagem", count_value, &tamanhoString);
    if (ret != ESP_OK && ret != ESP_ERR_NVS_NOT_FOUND) {
        printf("[STATUS] FLASH RESETED: COUNT RESETED!");
        setFlash("0");
    }

    return count_value;
}

void gpio_switch(int button){
    gpio_reset_pin(button);
    gpio_set_direction(button,GPIO_MODE_INPUT);
}

void vTaskSwitch(void *pvParameters){
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

                printf("Led ligado.\n");
            }else{
                printf("Led desligado.\n");
            }

            info_count_click.clicks = count;
            xQueueSend( xQueueCount, &info_count_click, portMAX_DELAY);
            xQueueSend( xQueueSwitch, &status_led, portMAX_DELAY);
        }
        old_status_switch = status_switch;

        vTaskDelay( 200 / portTICK_PERIOD_MS );
    }
}

int countClick (int click){
    if (status_led.status == true) {
        click++;
        printf("\nO LED acendeu %d vez(es).\n", click);

        info_count_click.clicks = click;
        xQueueSend( xQueueCount, &info_count_click, portMAX_DELAY);
    }
    return click;
}