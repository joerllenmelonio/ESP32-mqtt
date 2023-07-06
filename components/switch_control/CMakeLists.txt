idf_component_register(SRCS "switch_control.c"
                    INCLUDE_DIRS "include"
                    REQUIRES mqtt esp-tls nvs_flash ulp)