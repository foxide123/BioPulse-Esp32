idf_component_register(SRCS "main.c"
            INCLUDE_DIRS "."
            REQUIRES ph esp_adc driver
            PRIV_REQUIRES
)
            