#include <stdio.h>
#include "ulp_code.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../../definition.h"
#include <unistd.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "ulp_ulp_code.h"

#include "../mqtt_control/include/mqtt_control.h"
#include "../switch_control/include/switch_control.h"

extern const uint8_t ulp_code_bin_start[] asm("_binary_ulp_ulp_code_bin_start");
extern const uint8_t ulp_code_bin_end[] asm("_binary_ulp_ulp_code_bin_end");

info_count_t info_count_click;
char *getFlash(void);
void setFlash(char* information);

void deepSleepDelay(int delay){
    printf("\n[DEEPSLEEP] Rebooting in %d seconds...\n", delay);
    usleep(delay * 1000000);

    printf("\n[DEEPSLEEP] Going into deep sleep.\n");
    ESP_ERROR_CHECK( esp_sleep_enable_ulp_wakeup() );
    esp_deep_sleep(TIME_DEEP_SLEEP * 1000000);
}

void vTaskControlUlp(void *pvParameters) {
    while (true){
        int temp = getValueFromFlash();
        int new_value;
        esp_sleep_wakeup_cause_t wakeup_cause = esp_sleep_get_wakeup_cause();
        switch (wakeup_cause) {
            case ESP_SLEEP_WAKEUP_ULP:
                printf("\n[WAKEUP] ULP!");
                new_value = updatePulse();
                if (new_value != temp) {
                    xQueueSend( xQueueCount, &info_count_click, portMAX_DELAY);
                } 
                deepSleepDelay(5);
                break;
            case ESP_SLEEP_WAKEUP_TIMER:
                printf("\n[WAKEUP] TIMER SLEEP!");
                new_value = updatePulse();
                if (new_value != temp) {
                    xQueueSend( xQueueCount, &info_count_click, portMAX_DELAY);
                } 
                deepSleepDelay(5);
                break;
            default:
                printf("\n[WAKEUP] TURN ON!");
                initULP();
                deepSleepDelay(5);
                break;
        }
        vTaskDelay( 10000 / portTICK_PERIOD_MS );
    }
}

void initULP(void) {
    esp_err_t err = ulp_load_binary(0, ulp_code_bin_start,
            (ulp_code_bin_end - ulp_code_bin_start) / sizeof(uint32_t));
    ESP_ERROR_CHECK(err);

    /* GPIO que sera usado como entrada. */
    gpio_num_t gpio_num = GPIO_NUM_0;
    int rtcio_num = rtc_io_number_get(gpio_num);
    assert(rtc_gpio_is_valid_gpio(gpio_num) && "GPIO used for pulse counting must be an RTC IO");

    ulp_debounce_counter = 3;
    ulp_debounce_max_count = 3;
    ulp_next_edge = 0;
    ulp_io_number = rtcio_num; 
    ulp_edge_count_to_wake_up = 20; 

    /* GPIO como RTC IO, como entrada, desabilitando pullup e pulldown */
    rtc_gpio_init(gpio_num);
    rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_INPUT_ONLY);
    rtc_gpio_pulldown_dis(gpio_num);
    rtc_gpio_pullup_dis(gpio_num);
    rtc_gpio_hold_en(gpio_num);

    #if CONFIG_IDF_TARGET_ESP32
        rtc_gpio_isolate(GPIO_NUM_12);
        rtc_gpio_isolate(GPIO_NUM_15);
    #endif // CONFIG_IDF_TARGET_ESP32

    /* Inicializando ULP */
    err = ulp_run(&ulp_entry - RTC_SLOW_MEM);
    ESP_ERROR_CHECK(err);
}

int updatePulse(void) {
    /* programa ULP contando as bordas de sinal, e convertendo para o número de pulsos */
    uint32_t pulse_count_from_ulp = (ulp_edge_count & UINT16_MAX) / 2;
    /* In case of an odd number of edges, keep one until next time */
    ulp_edge_count = ulp_edge_count % 2;
    // printf("\nPulse count from ULP: %5d\n", pulse_count_from_ulp);

    int temp = getValueFromFlash();
    info_count_click.clicks = temp + pulse_count_from_ulp;
    return info_count_click.clicks;
    // printf("\n%d\n%d", info_count_click.clicks, temp);
}

int getValueFromFlash(void){
    char *value = getFlash();
    if (value == NULL) {
        setFlash("0");
    }
    int temp;
    sscanf(value, "%d", &temp);

    return temp;
}