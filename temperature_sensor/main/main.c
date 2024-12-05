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


#define  I2C_SLAVE_SCL_IO GPIO_NUM_22
#define  I2C_SLAVE_SDA_IO GPIO_NUM_21

#define DATA_LENGTH 32
#define I2C_SLAVE_SCL_IO GPIO_NUM_22
#define I2C_SLAVE_SDA_IO GPIO_NUM_21
#define I2C_SLAVE_ADDR 0x65
#define TEST_I2C_PORT I2C_NUM_0
#define TAG "I2C Slave Component"

QueueHandle_t temp_queue;
volatile float temperature = 0.0f;

void app_main(void)
{
    temp_queue = xQueueCreate(10, sizeof(float));

    if(temp_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create temperature queue");
        return;
    }

    gpio_set_pull_mode(ONE_WIRE_PIN, GPIO_PULLUP_ONLY);

    i2c_task_params_t *task_params = malloc(sizeof(i2c_task_params_t));
    if(task_params == NULL) {
        ESP_LOGE(TAG, "Failed to allocated memory for task parameters");
        return;
    }

    task_params->i2c_config.addr_bit_len = I2C_ADDR_BIT_LEN_7;
    task_params->i2c_config.clk_source = I2C_CLK_SRC_DEFAULT;
    task_params->i2c_config.i2c_port = TEST_I2C_PORT;
    task_params->i2c_config.send_buf_depth = 256;
    task_params->i2c_config.scl_io_num = I2C_SLAVE_SCL_IO;
    task_params->i2c_config.sda_io_num = I2C_SLAVE_SDA_IO;
    task_params->i2c_config.slave_addr = I2C_SLAVE_ADDR;

    task_params->temp_queue = temp_queue;

    xTaskCreate(i2c_slave_write_task, "i2c_slave_write_task", 4096, (void *)task_params, 5, NULL);

    while (1)
    {
        if (one_wire_reset())
        {
            ESP_LOGI("1-Wire", "Device found");

            one_wire_write_byte(0xCC);
            one_wire_write_byte(0x44);

            vTaskDelay(pdMS_TO_TICKS(750));

            one_wire_reset();
            one_wire_write_byte(0xCC);
            one_wire_write_byte(0xBE);

            uint8_t tempLSB = one_wire_read_byte();
            uint8_t tempMSB = one_wire_read_byte();
            int16_t tempRaw = (tempMSB << 8) | tempLSB;
            float temperature = tempRaw / 16.0;

            ESP_LOGI("1-Wire", "Temperature: %.2f°C", temperature);

            if (xQueueSend(temp_queue, &temperature, pdMS_TO_TICKS(1000)) == pdPASS) {
                ESP_LOGI(TAG, "Successfully RECEIVED the value to the queue");
            }
        }
        else{
            ESP_LOGI("1-Wire", "No device found");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

#endif