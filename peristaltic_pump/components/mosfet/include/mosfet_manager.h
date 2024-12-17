#include "driver/i2c_slave.h"
static IRAM_ATTR bool i2c_slave_rx_done_callback(i2c_slave_dev_handle_t channel, const i2c_slave_rx_done_event_data_t *edata, void *user_data);
static void mosfet_publish_task(void *arg);
esp_err_t mosfet_init(void *arg);