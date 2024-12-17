idf_component_register(SRCS "src/mosfet.c" "src/mosfet_manager.c"
                INCLUDE_DIRS "include"
                REQUIRES esp_driver_gpio esp_driver_i2c cJSON)
                