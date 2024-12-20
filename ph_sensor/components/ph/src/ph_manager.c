#include "ph_manager.h"

#define ADC_CHANNEL ADC_CHANNEL_0 // GPIO4
#define ADC_UNIT ADC_UNIT_2
#define ADC_ATTEN ADC_ATTEN_DB_11    // 0-3.6V
#define ADC_WIDTH ADC_BITWIDTH_12   // 12-bit resolution

esp_err_t adc_sensor_one_shot_read(adc_oneshot_unit_handle_t handle, int *out_raw)
{
    if(out_raw == NULL){
        ESP_LOGE(PH_TAG, "Output pointer is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = adc_oneshot_read(handle, ADC_CHANNEL, out_raw);
    if(ret != ESP_OK){
        ESP_LOGE(PH_TAG, "ADC One-Shot read failed: %s", esp_err_to_name(ret));
    }
    return ret;
}

float adc_to_voltage_conversion(int adc_value)
{
    return ((float)adc_value * ADC_VMAX) / ADC_DMAX;
}

void adc_one_shot_delete(adc_oneshot_unit_handle_t handle){
    esp_err_t ret = adc_oneshot_del_unit(handle);
    if(ret == ESP_OK) {
        ESP_LOGI(PH_TAG, "Successfuly deleted adc oneshot unit");
    }else {
        ESP_LOGE(PH_TAG, "Error while deleting adc oneshot unit");
    }
}

adc_oneshot_unit_handle_t adc_one_shot_init()
{
     // 1. Create ADC Unit
    adc_oneshot_unit_handle_t handle;

    adc_oneshot_unit_init_cfg_t init_config2 = {
        .unit_id = ADC_UNIT,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    esp_err_t ret = adc_oneshot_new_unit(&init_config2, &handle);

    if(ret != ESP_OK) {
        return NULL;
    }else {
        return handle;
    }
}