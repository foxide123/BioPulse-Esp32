
//#include "driver/i2c_slave.h"
//#include "driver/i2c.h"

#ifdef CONFIG_IDF_TARGET_LINUX
//#include "Mocki2c_slave.h"
#endif

#include "driver/i2c_slave.h"
#include "driver/i2c_types.h"
#include "hal/i2c_types.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "i2c_component_slave.h"
#include "i2c_component_interface.h"
#include "driver/i2c.h"
#include "cJSON_Manager.h"
#include "cJSON_Model.h"
#include <string.h>

esp_err_t ret;
uint8_t data_rd[DATA_LENGTH]; // Buffer for reading
uint8_t data_wr[DATA_LENGTH] = {0x00, 0x01, 0x02}; // Buffer for writing
QueueHandle_t s_receive_queue;


// Callback function called when slave receives data
static IRAM_ATTR bool i2c_slave_rx_done_callback(i2c_slave_dev_handle_t channel, const i2c_slave_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t receive_queue = (QueueHandle_t)user_data;
    xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

//  Read request from master. Slave sends data back
void i2c_slave_read_task(void *arg)
{
    uint8_t *data_rd = (uint8_t *)malloc(DATA_LENGTH);

    //Casting the argument to i2c_slave_config_t pointer
    i2c_slave_config_t *i2c_slv_config = (i2c_slave_config_t *)arg;
    

    i2c_slave_dev_handle_t slave_handle;
    ESP_ERROR_CHECK(i2c_new_slave_device(i2c_slv_config, &slave_handle));

    // Create a queue to receive notifications when data has been received
    s_receive_queue = xQueueCreate(1, sizeof(i2c_slave_rx_done_event_data_t));

    // Set up the callback for receiving events
    i2c_slave_event_callbacks_t cbs = {
        .on_recv_done = i2c_slave_rx_done_callback,
    };
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(slave_handle, &cbs, s_receive_queue));

    i2c_slave_rx_done_event_data_t rx_data;
    while (1) {
        ret = i2c_slave_receive(slave_handle, data_rd, DATA_LENGTH);
        if(ret == ESP_OK)
        {
            if (xQueueReceive(s_receive_queue, &rx_data, pdMS_TO_TICKS(10000))) {
            ESP_LOGI(TAG, "Received data from master");
        } else {
            ESP_LOGW(TAG, "No data received in 10 seconds");
        }
        }else{
            ESP_LOGE(TAG, "Error while receaving data: %s", esp_err_to_name(ret));
            free(data_rd);
        }
    }
}

//void * allows to pass any datas
void i2c_slave_write_task(void *arg)
{

    i2c_task_params_t *params = (i2c_task_params_t *)arg;
    i2c_slave_dev_handle_t handle = params->handle;
    char* json = params->json;

    size_t json_length = strlen(json) + 1;
    uint8_t json_buffer[256];
    if(json_length > sizeof(json_buffer)){
        ESP_LOGE(TAG, "JSON string too long for I2C");
    }
    else {
        memcpy(json_buffer, json, json_length);
    }
   
    while (1)
    {

        esp_err_t ret = i2c_slave_transmit(handle, json_buffer, json_length, 1000);
        if(ret == ESP_OK)
        {
            ESP_LOGI(TAG, "Data transmitted successfully");
        }
        else 
        {
            ESP_LOGE(TAG, "Error transmitting data: %s", esp_err_to_name(ret));
        }
    }
    // Clean up (not reached in this infinite loop)
    vTaskDelete(NULL);
}

