#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include "ds18b20.h"
#include "temperature.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_component_slave.h"
#include "bluetooth_component.h"
#include "cJSON_Sensor_Manager.h"

#define I2C_SLAVE_ADDR 0x68
#define ONE_WIRE_PIN GPIO_NUM_16
#define SENSOR_READ_INTERVAL_MS 5000 // Read every 5 seconds
#define TEMP_TAG "Temperature"

esp_err_t temperature_init(void *arg);

#endif