#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tds.h"
#include "esp_log.h"

void app_main(void)
{
    if(!adc_init_and_calibrate()) {
        ESP_LOGE("MAIN", "ADC initialization and calibration failed");
        vTaskDelete(NULL);
    }

    xTaskCreate(tds_task, "tds_task", 4096, NULL, 5, NULL);
}