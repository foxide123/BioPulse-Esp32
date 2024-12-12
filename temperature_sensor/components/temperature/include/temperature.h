#include "mqtt_client.h"

#define I2C_SLAVE_ADDR 0x65
#define ONE_WIRE_PIN GPIO_NUM_16
#define SENSOR_READ_INTERVAL_MS 5000 // Read every 5 seconds
#define TEMP_TAG "Temperature"

esp_err_t temperature_init(esp_mqtt_client_handle_t client);