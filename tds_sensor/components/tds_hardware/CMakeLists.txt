idf_component_register(SRCS "src/tds.c"
                        INCLUDE_DIRS "include"
                        REQUIRES esp_driver_gpio esp_timer esp_adc)