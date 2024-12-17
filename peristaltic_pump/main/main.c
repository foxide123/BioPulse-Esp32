#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/i2c_slave.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mosfet_manager.h"


#define I2C_SLAVE_ADDR 0x59
#define DATA_LENGTH 1
#define I2C_SLAVE_SCL_IO GPIO_NUM_22
#define I2C_SLAVE_SDA_IO GPIO_NUM_21
#define TEST_I2C_PORT I2C_NUM_0
#define MAIN_TAG "Main"


void app_main(void)
{

    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,   // 7-bit address
        .clk_source = I2C_CLK_SRC_DEFAULT,    // set the clock source
        .i2c_port = I2C_NUM_0,                        // set I2C port number
        .send_buf_depth = 256,                // set tx buffer length
        .scl_io_num = I2C_SLAVE_SCL_IO,                      // SCL gpio number
        .sda_io_num = I2C_SLAVE_SDA_IO,                      // SDA gpio number
        .slave_addr = 0x59,  
    };

    i2c_slave_dev_handle_t i2c_handle;
    ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &i2c_handle));

    esp_err_t mosfet_init_status = mosfet_init(&i2c_handle);
    if(mosfet_init_status != ESP_OK)
    {
        ESP_LOGE(MAIN_TAG, "Failed to initialize Mosfet");
        return;
    }


    /*
    mosfet_setup();

    while(1){
        mosfet_turn_on();
        vTaskDelay(pdMS_TO_TICKS(1000));

        mosfet_turn_off();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    */
}
