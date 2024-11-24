#ifdef UNIT_TESTING
#include "i2c_mock.h"

#define vTaskDelay vTaskDelay_mock

DEFINE_FAKE_VALUE_FUNC4(esp_err_t, i2c_master_write_to_device,
                         i2c_port_t, uint8_t, const uint8_t*, size_t);
DEFINE_FAKE_VALUE_FUNC4(esp_err_t, i2c_slave_transmit, i2c_slave_dev_handle_t, const uint8_t*, int, int);
DEFINE_FAKE_VALUE_FUNC2(esp_err_t, i2c_new_slave_device,const i2c_slave_config_t*, i2c_slave_dev_handle_t*);
DEFINE_FAKE_VOID_FUNC1(vTaskDelay_mock, TickType_t);
#endif