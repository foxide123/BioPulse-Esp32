idf_component_register(SRCS "src/temperature.c"
                INCLUDE_DIRS "include"
                REQUIRES unity ds18b20 mqtt i2c_component bt bluetooth_component cJSON
                PRIV_REQUIRES esp_driver_gpio)
                