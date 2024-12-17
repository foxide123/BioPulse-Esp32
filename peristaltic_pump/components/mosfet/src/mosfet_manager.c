#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/i2c_types.h"
#include "driver/i2c_slave.h"
#include "cJSON_Actuator_Manager.h"
#include "cJSON_Actuator_Model.h"

#include "mosfet_manager.h"
#include "mosfet.h"

#define DATA_LENGTH 1
#define MOSFET_TAG "MOSFET_COMPONENT"

uint8_t *data_rd = NULL;
QueueHandle_t s_receive_queue = NULL;
uint32_t size_rd = 0;

static IRAM_ATTR bool i2c_slave_rx_done_callback(i2c_slave_dev_handle_t channel, const i2c_slave_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t receive_queue = (QueueHandle_t)user_data;
    if(xQueueSendFromISR(receive_queue, edata, &high_task_wakeup) != pdPASS) {
       // ESP_LOGE(MOSFET_TAG, "Failed to send data to queue from ISR");
    }
    
    return high_task_wakeup == pdTRUE;
}

static void mosfet_publish_task(void *arg)
{

    ESP_LOGI(MOSFET_TAG, "MOSFET publish task started.");

    // Allocate memory for data reception
    data_rd = (uint8_t *) malloc(DATA_LENGTH);
    if(data_rd == NULL) {
        ESP_LOGE(MOSFET_TAG, "Failed to allocate memory for data_rd");
        vTaskDelete(NULL);
    }
    ESP_LOGI(MOSFET_TAG, "Allocated memory for data_rd");


    esp_err_t ret;
    i2c_slave_dev_handle_t handle = *(i2c_slave_dev_handle_t *)arg;

    s_receive_queue = xQueueCreate(10, sizeof(i2c_slave_rx_done_event_data_t));
    if(s_receive_queue == NULL){
        ESP_LOGE(MOSFET_TAG, "Failed to create receive queue");
        free(data_rd);
        vTaskDelete(NULL);
    }
    i2c_slave_event_callbacks_t cbs = {
        .on_recv_done = i2c_slave_rx_done_callback,
    };
    ret = i2c_slave_register_event_callbacks(handle, &cbs, s_receive_queue);
    if(ret != ESP_OK) {
        ESP_LOGE(MOSFET_TAG, "Failed to register I2C slave callbacks");
        free(data_rd);
        vQueueDelete(s_receive_queue);
        vTaskDelete(NULL);
    }
    ESP_LOGI(MOSFET_TAG, "I2C slave callbacks registered successfuly");

    while (1) {
        i2c_slave_rx_done_event_data_t rx_data;

            esp_err_t ret = i2c_slave_receive(handle, data_rd, DATA_LENGTH);
            if(ret == ESP_OK) {
                ESP_LOGI(MOSFET_TAG, "Received data: 0x%02X", data_rd[0]);

                // Received bytes
                if(data_rd[0] == 0x01) {
                    ESP_LOGI(MOSFET_TAG, "Command 0x01 received: Turning MOSFET ON");
                    mosfet_turn_on();
                } else if(data_rd[0] == 0x00) {
                    ESP_LOGI(MOSFET_TAG, "Command 0x00 received: Turning MOSFET OFF");
                    mosfet_turn_off();
                } else {
                    ESP_LOGW(MOSFET_TAG, "Unknown command: 0x%02X", data_rd[0]);
                }   
            }
            else {
                ESP_LOGW(MOSFET_TAG, "Failed to receive data");
            }
        xQueueReceive(s_receive_queue, &rx_data, pdMS_TO_TICKS(10000));
    }
    free(data_rd);
    vQueueDelete(s_receive_queue);
    vTaskDelete(NULL);
}

esp_err_t mosfet_init(void *arg)
{
    i2c_slave_dev_handle_t i2c_handle = *(i2c_slave_dev_handle_t *)arg;
    esp_err_t ret;

    mosfet_setup();

    xTaskCreate(mosfet_publish_task, "mosfet_publish_task", 1024 * 4, &i2c_handle, 10, NULL);
    
    return ESP_OK;
}