// Original source code: https://wiki.keyestudio.com/KS0429_keyestudio_TDS_Meter_V1.0#Test_Code
// Project details: https://RandomNerdTutorials.com/arduino-tds-water-quality-sensor/

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "hal/adc_types.h"
#include "esp_adc/adc_oneshot.h"

#define TDS_PIN GPIO_NUM_4
#define VREF 5.0              // analog reference voltage(Volt) of the ADC
#define SCOUNT  30            // sum of sample point

int readADC(int pin);
uint32_t current_time_ms();
int getMedianNum(int bArray[], int iFilterLen);
bool adc_init_and_calibrate();
void tds_task(void *param);
void adc_test_task(void *param);
// void setup();
// void loop();
