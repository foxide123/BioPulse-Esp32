idf_component_register(
    SRCS "src/wifi_component.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_wifi nvs_flash esp_netif esp_http_server esp_wifi esp_eth esp-tls
)