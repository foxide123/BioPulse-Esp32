#include "temperature.h"

static float read_temperature(void);
static void temperature_publish_task(void* arg);


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

        //ESP_LOGI("1-Wire", "Temperature: %.2f°C", temperature);
        return temperature;
    } 
    else{
        //ESP_LOGI(TEMP_TAG, "No device found");
        return -1000.0f;
    }
}


static void temperature_publish_task(void *arg)
{
    i2c_slave_dev_handle_t handle = *(i2c_slave_dev_handle_t *)arg;
    i2c_task_params_t task_params;
    task_params.handle = handle;

    uint8_t *json_buffer = (uint8_t *) malloc(256);

    float temperature = 0.0f;

    while(1)
    {
        temperature = read_temperature();

        if(temperature != -1000.0f)
        {
            //char temp_str[10];
            //snprintf(temp_str, sizeof(temp_str), "%.2f", temperature);
            jsonSensorResponse json;
            json.value = temperature;
            json.type = TEMPERATURE;
            json.id = "98620";
            json.sensor_name = "temp1";


            //task_params.json = createdJson;
            //i2c_slave_write_task(&task_params);

            char* createdJson = create_sensor_response_json(json);
            size_t json_length = strlen(createdJson)+1;
            // writing json length to the first byte in the buffer
            json_buffer[0] = (uint8_t)json_length;
            memcpy(json_buffer + 1, createdJson, json_length);

            uint8_t total_message_length = json_length + 1;
            esp_err_t transmit_ret = i2c_slave_transmit(handle, json_buffer, total_message_length, 1000 / portTICK_PERIOD_MS);
            if(transmit_ret == ESP_OK)
            {
                ESP_LOGI(TEMP_TAG, "Data transmitted successfully");
                memset(json_buffer, 0, json_length);
            }else
            {
                ESP_LOGE(TEMP_TAG, "Error transmitting data: %s", esp_err_to_name(transmit_ret));
            }

            bluetooth_send_data(createdJson);
    
           //int msg_id = esp_mqtt_client_publish(mqtt_client, "/sensors/temperature", createdJson, 0, 1, 1);
            //if(msg_id != -1)
            //{
             //   ESP_LOGI(TEMP_TAG, "Published temperature: %s°C, msg_id=%d", createdJson, msg_id);
           // }else
           // {
            //    ESP_LOGE(TEMP_TAG, "Failed to publish temperature");
            //}
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

esp_err_t temperature_init(void *arg)
{
    i2c_slave_dev_handle_t i2c_handle = *(i2c_slave_dev_handle_t *)arg;
    esp_err_t ret;


    xTaskCreate(temperature_publish_task, "temperature_publish_task", 1024 * 4, &i2c_handle, 10, NULL);

    return ESP_OK;
}