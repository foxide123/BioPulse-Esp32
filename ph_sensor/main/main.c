#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ph_manager.h"
#include "esp_log.h"

void app_main(void)
{

    adc_oneshot_unit_handle_t handle = adc_one_shot_init();
    esp_err_t ret;

    if(handle != NULL)
    {
        while(1){
            int adc_value = 0;
            esp_err_t ret = adc_sensor_one_shot_read(handle, &adc_value);

            if(ret == ESP_OK){

                float voltage = adc_to_voltage_conversion(&adc_value);

                ESP_LOGI(PH_TAG, "ADC Value: %d, Voltage: %.2f V", adc_value, voltage);
            }
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        adc_one_shot_delete(handle);
    }
}