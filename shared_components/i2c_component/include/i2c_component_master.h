//#ifndef I2C_MASTER_H
//#define I2C_MASTER_H

#include "driver/i2c_master.h"
#include "hal/i2c_types.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h" 

#if CONFIG_IDF_TARGET_ESP32
    #define  I2C_MASTER_SCL_IO GPIO_NUM_22
    #define  I2C_MASTER_SDA_IO GPIO_NUM_21
#else
    #define  I2C_MASTER_SCL_IO GPIO_NUM_5
    #define  I2C_MASTER_SDA_IO GPIO_NUM_6
#endif

#define DATA_LENGTH 32
//#define I2C_MASTER_ADDR 0x65
#define I2C_PORT I2C_NUM_0
#define I2C_MASTER_TAG "I2C Slave Component"

typedef struct {
        i2c_master_dev_handle_t handle;
        char* json;
} i2c_master_task_params_t;


void i2c_master_read_task(void *arg);
void i2c_master_write_task(void *arg);
void i2c_master_transmit_receive_task(void *arg);
//#endif