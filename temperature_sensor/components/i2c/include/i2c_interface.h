
#ifndef I2C_INTERFACE_H
#define I2C_INTERFACE_H

#ifdef CONFIG_IDF_TARGET_LINUX
#include "Mocki2c_slave.h"
#else
#include "driver/i2c_slave.h"
#include "esp_err.h"
#endif


esp_err_t i2c_interface_new_slave_device(const i2c_slave_config_t* slave_config, i2c_slave_dev_handle_t* ret_handle);
esp_err_t i2c_interface_slave_transmit(i2c_slave_dev_handle_t i2c_slave, const uint8_t* data, int size, int xfer_timeout_ms);


#endif // I2C_INTERFACE_H