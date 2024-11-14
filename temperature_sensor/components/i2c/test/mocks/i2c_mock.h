#ifdef UNIT_TESTING
#ifndef FAKE_I2C_H
#define FAKE_I2C_H

#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"
#include "fff.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define vTaskDelay vTaskDelay_mock

DECLARE_FAKE_VALUE_FUNC4(esp_err_t, i2c_master_write_to_device,
                         i2c_port_t, uint8_t, const uint8_t*, size_t);
DECLARE_FAKE_VALUE_FUNC4(esp_err_t, i2c_slave_transmit, i2c_slave_dev_handle_t, const uint8_t*, int, int);
DECLARE_FAKE_VALUE_FUNC2(esp_err_t, i2c_new_slave_device,const i2c_slave_config_t*, i2c_slave_dev_handle_t*);
DECLARE_FAKE_VOID_FUNC1(vTaskDelay_mock, TickType_t);

#endif // FAKE_I2C_H
#endif