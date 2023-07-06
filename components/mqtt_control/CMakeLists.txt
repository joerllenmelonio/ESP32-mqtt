idf_component_register(SRCS "mqtt_control.c"
                    REQUIRES mqtt esp-tls
                    INCLUDE_DIRS "include"
                    REQUIRES mqtt esp-tls nvs_flash ulp)