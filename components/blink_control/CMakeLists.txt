idf_component_register(SRCS "blink_control.c"
                    INCLUDE_DIRS "include"
                    REQUIRES mqtt esp-tls nvs_flash ulp)