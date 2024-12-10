#ifdef CONFIG_IDF_TARGET_LINUX

void app_main(void)
{
    
}

#else

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ds18b20.h"
#include "driver/gpio.h"
#include "i2c_component_slave.h"
#include "driver/i2c_slave.h"
#include "mqtt_component.h"
#include "mqtt_client.h"
#include "temperature.h"

#define DATA_LENGTH 32
#define I2C_SLAVE_SCL_IO GPIO_NUM_22
#define I2C_SLAVE_SDA_IO GPIO_NUM_21
#define I2C_SLAVE_ADDR 0x65
#define TEST_I2C_PORT I2C_NUM_0
#define MAIN_TAG "Main"

QueueHandle_t temp_queue;
volatile float temperature = 0.0f;

void app_main(void)
{

    // MQTT client config
    esp_mqtt_client_config_t mqtt_cfg = {
        .credentials = {
            .username = "bob",
            .authentication = {
                .password = "bob",
                //.certificate = (const char *)mqtt_eclipseprojects_io_pem_start,
            }
        },
        .broker = {
            .address = {
                .uri = "mqtt://10.154.220.118:1883",
            }
        }
    };

    // MQTT client initialization
    esp_mqtt_client_handle_t client = mqtt_init(&mqtt_cfg);
    if(client == NULL)
    {
        ESP_LOGE(MAIN_TAG, "Failed to initialize MQTT client");
        return;
    }

    // Initialize Temperature component
    esp_err_t temp_init_status = temperature_init(client);
    if(temp_init_status != ESP_OK)
    {
        ESP_LOGE(MAIN_TAG, "Failed to initialize Temperature component");
        return;
    }

    ESP_LOGI(MAIN_TAG, "Application initialized successfully");
}

#endif