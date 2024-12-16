idf_component_register(SRCS "main.c"
            INCLUDE_DIRS "."
            REQUIRES cJSON esp_driver_gpio tds_hardware
            PRIV_REQUIRES
)
            