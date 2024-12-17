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
#include "bluetooth_component.h"
#include "cJSON_Sensor_Manager.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "wifi_component.h"

#define DATA_LENGTH 32
#define I2C_SLAVE_SCL_IO GPIO_NUM_22
#define I2C_SLAVE_SDA_IO GPIO_NUM_21
#define I2C_SLAVE_ADDR 0x61
#define TEST_I2C_PORT I2C_NUM_0
#define MAIN_TAG "Main"

QueueHandle_t temp_queue;
volatile float temperature = 0.0f;

void app_main(void)
{
    bluetooth_init();

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

    // Registering i2c device
        i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,   // 7-bit address
        .clk_source = I2C_CLK_SRC_DEFAULT,    // set the clock source
        .i2c_port = I2C_NUM_0,                        // set I2C port number
        .send_buf_depth = 256,                // set tx buffer length
        .scl_io_num = I2C_SLAVE_SCL_IO,                      // SCL gpio number
        .sda_io_num = I2C_SLAVE_SDA_IO,                      // SDA gpio number
        .slave_addr = I2C_SLAVE_ADDR,  
    };

    i2c_slave_dev_handle_t i2c_handle;
    ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &i2c_handle));

    // Initialize Temperature component
    esp_err_t temp_init_status = temperature_init(&i2c_handle);
    if(temp_init_status != ESP_OK)
    {
        ESP_LOGE(MAIN_TAG, "Failed to initialize Temperature component");
        return;
    }
    
    uint8_t mac[6];
    esp_err_t ret= esp_read_mac(mac, ESP_MAC_WIFI_STA);
    if(ret != ESP_OK)
    {
        ESP_LOGE(MAIN_TAG, "Failed to read mac address");
    }
    else {
        char mac_str[18];
        snprintf(mac_str, sizeof(mac_str),
         "%02X:%02X:%02X:%02X:%02X:%02X",
         mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    }
    ESP_LOGI(MAIN_TAG, "Application initialized successfully");
}

#endif