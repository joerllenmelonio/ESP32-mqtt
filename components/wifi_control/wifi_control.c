#include <string.h>
#include "wifi_control.h"
#include "../mqtt_control/mqtt_control.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sys.h"

// WiFi parameters
#define WIFI_SSID "insert wifi ssid"
#define WIFI_PASS "insert wifi password"

static const char *TAG = "MQTT_BUTTON02";

// Event group
static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;

// configure the wifi connection and start the interface
wifi_config_t wifi_config = {
    .sta = {
      .ssid = WIFI_SSID,
      .password = WIFI_PASS,
    },
};


// Wifi event handler
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
  switch (event->event_id) {

    case SYSTEM_EVENT_STA_START:
      esp_wifi_connect();
      ESP_LOGI(TAG, "Tentando conectar ao Wi-Fi\n");
      break;

    case SYSTEM_EVENT_STA_GOT_IP:
      xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
      mqtt_app_start();
      break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
      xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
      printf("DISCONNECTED FROM %s!\n",WIFI_SSID);
      esp_wifi_connect();
      break;

    default:
      break;
  }

  return ESP_OK;
}

// Main task
void vTaskWifi(void *pvParameter)
{
  tcpip_adapter_ip_info_t ip_info;


  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());
  printf("Connecting to %s\n", WIFI_SSID);

  // wait for connection
  printf("Waiting for connection to the wifi network... \n");
  xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
  printf("Connected!\n");

  // print the local IP address
  ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info));
  printf("IP Address:  %s\n", ip4addr_ntoa(&ip_info.ip));
  printf("Subnet mask: %s\n", ip4addr_ntoa(&ip_info.netmask));
  printf("Gateway:     %s\n", ip4addr_ntoa(&ip_info.gw));
  printf("You can connect now to any webserver online! :-)\n");

  while (true) {
    vTaskDelay(1000 / portTICK_RATE_MS);
  }
}

// Main application
void init()
{

  printf("ESP-IDF version used: ");
  printf(IDF_VER"\n");
  // disable the default wifi logging
  esp_log_level_set("wifi", ESP_LOG_NONE);

  // create the event group to handle wifi eventsP
  wifi_event_group = xEventGroupCreate();

  // initialize the tcp stack
  tcpip_adapter_init();

  // initialize the wifi event handler
  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

  // initialize the wifi stack in STAtion mode with config in RAM
  wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
  ESP_ERROR_CHECK(esp_wifi_start());

}