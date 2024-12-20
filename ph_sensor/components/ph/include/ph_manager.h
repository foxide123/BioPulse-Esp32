#ifndef PH_MANAGER_H
#define PH_MANAGER_H

#include "esp_log.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"

#define DEFAULT_VREF 1100 // Default Vref for ESP32, in mV 
#define ADC_VMAX 3.3
#define ADC_DMAX 4095
#define PH_TAG "PH_COMPONENT" // Logging Tag

esp_err_t adc_sensor_one_shot_read(adc_oneshot_unit_handle_t handle, int *out_raw);
float adc_to_voltage_conversion(int adc_value);
void adc_one_shot_delete(adc_oneshot_unit_handle_t handle);
adc_oneshot_unit_handle_t adc_one_shot_init();

#endif