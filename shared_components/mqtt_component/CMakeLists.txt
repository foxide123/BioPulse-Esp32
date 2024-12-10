idf_component_register(
    SRCS "src/mqtt_component.c"
    INCLUDE_DIRS "include"
    REQUIRES  mqtt esp_partition nvs_flash esp_netif app_update
)