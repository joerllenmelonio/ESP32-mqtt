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

#define WIFI_SSID "insert wifi ssid"
#define WIFI_PASS "insert wifi password"

static const char *TAG = "MQTT_BUTTON02";

static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;

// configuração do wifi e inicialização da interface
wifi_config_t wifi_config = {
    .sta = {
      .ssid = WIFI_SSID,
      .password = WIFI_PASS,
    },
};


// define status de conexão
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
      printf("Disconectado de %s!\n",WIFI_SSID);
      esp_wifi_connect();
      break;

    default:
      break;
  }

  return ESP_OK;
}

// função principal
void vTaskWifi(void *pvParameter)
{
  tcpip_adapter_ip_info_t ip_info;


  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());
  printf("Conectando a %s\n", WIFI_SSID);

  // status
  printf("Esperando por conexão de rede... \n");
  xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
  printf("Conectado!\n");

  // retorna o IP local e outras infos
  ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info));
  printf("IP Address:  %s\n", ip4addr_ntoa(&ip_info.ip));
  printf("Subnet mask: %s\n", ip4addr_ntoa(&ip_info.netmask));
  printf("Gateway:     %s\n", ip4addr_ntoa(&ip_info.gw));
  printf("Pronto para conectar-se a um webserver\n");

  while (true) {
    vTaskDelay(1000 / portTICK_RATE_MS);
  }
}

void init()
{

  printf("Versão do ESP-IDF: ");
  printf(IDF_VER"\n");
  // desabilita o log padrão do wifi
  esp_log_level_set("wifi", ESP_LOG_NONE);

  // cria evento de grupo pro event handler
  wifi_event_group = xEventGroupCreate();

  tcpip_adapter_init(); //stack tcp
  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));  // initializa event handler

  // initializa  o wifi stack em STAtion mode
  wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
  ESP_ERROR_CHECK(esp_wifi_start());

}