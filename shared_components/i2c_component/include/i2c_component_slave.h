#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include "driver/i2c_slave.h"
#include "driver/i2c_types.h"
#include "hal/i2c_types.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "i2c_component_slave.h"
#include "i2c_component_interface.h"
#include "cJSON_Sensor_Manager.h"
#include "cJSON_Sensor_Model.h"
#include <string.h>

#define DATA_LENGTH 32
#define I2C_SLAVE_SCL_IO GPIO_NUM_22
#define I2C_SLAVE_SDA_IO GPIO_NUM_21
#define I2C_PORT I2C_NUM_0
#define I2C_SLAVE_TAG "I2C Slave Component"

typedef struct {
        i2c_slave_dev_handle_t handle;
        char* json;
} i2c_task_params_t;

void i2c_slave_read_task(void *arg);
void i2c_slave_write_task(void *arg);
#endif