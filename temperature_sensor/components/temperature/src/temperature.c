#include "ds18b20.h"
#include "temperature.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_component_slave.h"

#define SENSOR_READ_INTERVAL_MS 5000 // Read every 5 seconds

static float read_temperature(void)
{
    if(one_wire_reset(ONE_WIRE_PIN))
    {
        ESP_LOGI(TEMP_TAG, "Device found");

        one_wire_write_byte(ONE_WIRE_PIN, 0xCC);
        one_wire_write_byte(ONE_WIRE_PIN, 0x44);

        vTaskDelay(pdMS_TO_TICKS(750));

        one_wire_reset(ONE_WIRE_PIN);
        one_wire_write_byte(ONE_WIRE_PIN, 0xCC);
        one_wire_write_byte(ONE_WIRE_PIN, 0xBE);

        uint8_t tempLSB = one_wire_read_byte(ONE_WIRE_PIN);
        uint8_t tempMSB = one_wire_read_byte(ONE_WIRE_PIN);
        int16_t tempRaw = (tempMSB << 8) | tempLSB;
        float temperature = tempRaw / 16.0;

        ESP_LOGI("1-Wire", "Temperature: %.2f°C", temperature);
        return temperature;
    } 
    else{
        ESP_LOGI(TEMP_TAG, "No device found");
        return -1000.0f;
    }
}

static void temperature_publish_task(void *arg)
{
    esp_mqtt_client_handle_t mqtt_client = *(esp_mqtt_client_handle_t *)arg;
    float temperature = 0.0f;

    while(1)
    {
        temperature = read_temperature();

        if(temperature != -1000.0f)
        {
            char temp_str[10];
            snprintf(temp_str, sizeof(temp_str), "%.2f", temperature);

            int msg_id = esp_mqtt_client_publish(mqtt_client, "/sensors/temperature", temp_str, 0, 1, 1);
            if(msg_id != -1)
            {
                ESP_LOGI(TEMP_TAG, "Published temperature: %s°C, msg_id=%d", temp_str, msg_id);
            }else
            {
                ESP_LOGE(TEMP_TAG, "Failed to publish temperature");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(SENSOR_READ_INTERVAL_MS));
    }
}

esp_err_t temperature_init(esp_mqtt_client_handle_t client)
{
    QueueHandle_t temp_queue = xQueueCreate(10, sizeof(float));

    if(temp_queue == NULL) {
        ESP_LOGE(TEMP_TAG, "Failed to create temperature queue");
        return ESP_FAIL;
    }

    gpio_set_pull_mode(ONE_WIRE_PIN, GPIO_PULLUP_ONLY);

    i2c_task_params_t *task_params = malloc(sizeof(i2c_task_params_t));
    if(task_params == NULL) {
        ESP_LOGE(TEMP_TAG, "Failed to allocated memory for task parameters");
        return ESP_FAIL;
    }

    task_params->i2c_config.addr_bit_len = I2C_ADDR_BIT_LEN_7;
    task_params->i2c_config.clk_source = I2C_CLK_SRC_DEFAULT;
    task_params->i2c_config.i2c_port = TEST_I2C_PORT;
    task_params->i2c_config.send_buf_depth = 256;
    task_params->i2c_config.scl_io_num = I2C_SLAVE_SCL_IO;
    task_params->i2c_config.sda_io_num = I2C_SLAVE_SDA_IO;
    task_params->i2c_config.slave_addr = I2C_SLAVE_ADDR;

    task_params->temp_queue = temp_queue;

    BaseType_t task_created = xTaskCreate(temperature_publish_task, "i2c_slave_write_task", 4096, (void *)task_params, 5, NULL);
    if(task_created != pdPASS)
    {
        ESP_LOGE(TEMP_TAG, "Failed to create I2C slave write task");
        free(task_params);
        return ESP_FAIL;
    }

    ESP_LOGI(TEMP_TAG, "Temperature component initialized successfully");
    return ESP_OK;
}