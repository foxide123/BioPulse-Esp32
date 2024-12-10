#include "mqtt_client.h"

#define MQTT_TAG "MQTT"
#define CONFIG_BROKER_URI "mqtt://10.154.220.118:1883"
#define CONFIG_BROKER_BIN_SIZE_TO_SEND 1024


void publish_custom_message(esp_mqtt_client_handle_t client, const char *topic, const char *message, int qos, int retain);
// send_binary and mqtt_event_handler are private methods
//void send_binary(esp_mqtt_client_handle_t client);
//void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
esp_mqtt_client_handle_t mqtt_app_start(const esp_mqtt_client_config_t *mqtt_cfg);
esp_mqtt_client_handle_t mqtt_init(const esp_mqtt_client_config_t *mqtt_cfg);