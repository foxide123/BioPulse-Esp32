#ifdef CONFIG_IDF_TARGET_LINUX

void app_main(void)
{
    
}

#else

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ds18b20.h"
//#include "i2c_master.h"
//#include "i2c_slave.h"
#include "driver/gpio.h"

void app_main(void)
{
    gpio_set_pull_mode(ONE_WIRE_PIN, GPIO_PULLUP_ONLY);

    while (1)
    {
        if (one_wire_reset())
        {
            ESP_LOGI("1-Wire", "Device found");

            one_wire_write_byte(0xCC);
            one_wire_write_byte(0x44);

            vTaskDelay(pdMS_TO_TICKS(750));

            one_wire_reset();
            one_wire_write_byte(0xCC);
            one_wire_write_byte(0xBE);

            uint8_t tempLSB = one_wire_read_byte();
            uint8_t tempMSB = one_wire_read_byte();
            int16_t temp = (tempMSB << 8) | tempLSB;

            ESP_LOGI("1-Wire", "Temperature: %d.%04d C", temp/16, (temp%16)*625);
        }else{
            ESP_LOGI("1-Wire", "No device found");
        }
    }
}

#endif