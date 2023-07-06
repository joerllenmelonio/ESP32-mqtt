idf_component_register(SRCS "wifi_control.c"
                    INCLUDE_DIRS "include"
                    REQUIRES mqtt esp-tls nvs_flash ulp esp_http_client)