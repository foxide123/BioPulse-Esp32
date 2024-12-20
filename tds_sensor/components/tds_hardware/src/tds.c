// Original source code: https://wiki.keyestudio.com/KS0429_keyestudio_TDS_Meter_V1.0#Test_Code
// Project details: https://RandomNerdTutorials.com/arduino-tds-water-quality-sensor/
#include "tds.h"

static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);
static void adc_calibration_deinit(adc_cali_handle_t handle);

int analogBuffer[SCOUNT];     // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
int copyIndex         = 0;

static int adc_raw[2][10];
static int voltage[2][10];
float averageVoltage  = 0.0f;
float tdsValue        = 0.0f;
float temperature     = 16.0f;       // current temperature for compensation

const static char *TDS_TAG = "TDS_COMPONENT";

#if CONFIG_IDF_TARGET_ESP32S3
#define ADC_CHAN0 ADC_CHANNEL_3 
#define ADC_ATTEN ADC_ATTEN_DB_12
#else
#define ADC_CHAN0 ADC_CHANNEL_5
#define ADC_ATTEN ADC_ATTEN_DB_12
#endif

adc_oneshot_unit_handle_t adc_handle = NULL;
adc_cali_handle_t adc_cali_chan0_handle = NULL;
bool do_calibration_chan0 = false;

int readADC(int pin) {
  int raw = 0;
  ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHAN0, &raw));
  return raw;
}

bool adc_init_and_calibrate() {
  esp_err_t ret;

  // ADC Init
  adc_oneshot_unit_init_cfg_t init_config = {
    .unit_id = ADC_UNIT_1,
  };
  ret =adc_oneshot_new_unit(&init_config, &adc_handle);
  if(ret != ESP_OK) {
    ESP_LOGE(TDS_TAG, "Faile to initialize ADC");
    return false;
  }

  // ADC Config  
  adc_oneshot_chan_cfg_t config = {
    .atten    = ADC_ATTEN,
    .bitwidth = ADC_BITWIDTH_DEFAULT,
  };
  ret = adc_oneshot_config_channel(adc_handle, ADC_CHAN0, &config);
  if(ret != ESP_OK) {
    ESP_LOGE(TDS_TAG, "Failed to configure ADC channel");
    return false;
  }

  // ADC Calibration Init
  bool calibration_success = adc_calibration_init(ADC_UNIT_1, ADC_CHAN0, ADC_ATTEN, &adc_cali_chan0_handle);
  if (!calibration_success) {
    ESP_LOGE(TDS_TAG, "ADC calibration failed");
    return false;
  }
  do_calibration_chan0 = true;

  return true;
}

// ADC Calibration
static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
  adc_cali_handle_t handle = NULL;
  esp_err_t ret = ESP_FAIL;
  bool calibrated = false;

  if (!calibrated) {
    ESP_LOGI(TDS_TAG, "calibration scheme version is %s", "Curve Fitting");
    adc_cali_curve_fitting_config_t cali_config = {
      .unit_id  = unit,
      .chan     = channel,
      .atten    = atten,
      .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
    if(ret == ESP_OK) {
      calibrated = true;
    }
  }

  *out_handle = handle;
  if (ret == ESP_OK) {
    ESP_LOGI(TDS_TAG, "Calibration Success");
  } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
    ESP_LOGW(TDS_TAG, "eFuse not burnt, skip software calibration");
  } else {
    ESP_LOGE(TDS_TAG, "Invalid arg or no memory");
  }

  return calibrated;
}

static void adc_calibration_deinit(adc_cali_handle_t handle)
{
  ESP_LOGI(TDS_TAG, "deregister %s calibration scheme", "Curve Fitting");
  ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(handle));
}

uint32_t current_time_ms() {
    return (uint32_t)(esp_timer_get_time() / 1000ULL);
}


// median filtering algorithm
int getMedianNum(int bArray[], int iFilterLen){
  int bTab[iFilterLen];
  for (uint8_t i = 0; i<iFilterLen; i++) {
    bTab[i] = bArray[i];
  }

  for (int j = 0; j < iFilterLen - 1; j++) {
    for (int i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        int bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  int bTemp;
  if ((iFilterLen & 1) > 0){
    bTemp = bTab[(iFilterLen - 1) / 2];
  }
  else {
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}

void tds_task(void *param) {
    uint32_t printTimepoint = current_time_ms();

    while(1){
        uint32_t now = current_time_ms();

        // Every 1000 ms, read and print ADC value
        if (now - printTimepoint > 1000U) {
            printTimepoint = now;
            int raw = readADC(TDS_PIN);
            float voltage = 0.0f;

            if(do_calibration_chan0) {
                ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc_cali_chan0_handle, raw, &voltage));
            } else {
                voltage = (float)raw * VREF / 4095.0f; // Adjust based on ADC resolution
            }

            ESP_LOGI(TDS_TAG, "ADC1 Channel[%d] Raw Data: %d, Voltage: %.2f V", ADC_CHAN0, raw, voltage);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // Tear Down (Unreachable due to infinite loop, consider proper task deletion)
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle));
    if (do_calibration_chan0) {
        adc_calibration_deinit(adc_cali_chan0_handle);
    }
}

void adc_test_task(void *param) {
    uint32_t printTimepoint = current_time_ms();

    while(1){
        uint32_t now = current_time_ms();

        // Every 1000 ms, read and print ADC value
        if (now - printTimepoint > 1000U) {
            printTimepoint = now;
            int raw = readADC(TDS_PIN);
            float voltage = 0.0f;

            if(do_calibration_chan0) {
                ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc_cali_chan0_handle, raw, &voltage));
            } else {
                voltage = (float)raw * VREF / 4095.0f; // For 12-bit ADC
            }

            ESP_LOGI(TDS_TAG, "ADC1 Channel[%d] Raw Data: %d, Voltage: %.2f V", ADC_CHAN0, raw, voltage);
        }

        vTaskDelay(pdMS_TO_TICKS(500)); // Adjust delay as needed
    }

    // Tear Down (Unreachable due to infinite loop, consider proper task deletion)
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle));
    if (do_calibration_chan0) {
        adc_calibration_deinit(adc_cali_chan0_handle);
    }
}





/*
void setup(){
  Serial.begin(115200);
  pinMode(TdsSensorPin,INPUT);
}

void loop(){
  static unsigned long analogSampleTimepoint = millis();
  if(millis()-analogSampleTimepoint > 40U){     //every 40 milliseconds,read the analog value from the ADC
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);    //read the analog value and store into the buffer
    analogBufferIndex++;
    if(analogBufferIndex == SCOUNT){ 
      analogBufferIndex = 0;
    }
  }   
  
  static unsigned long printTimepoint = millis();
  if(millis()-printTimepoint > 800U){
    printTimepoint = millis();
    for(copyIndex=0; copyIndex<SCOUNT; copyIndex++){
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
      
      // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0;
      
      //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0)); 
      float compensationCoefficient = 1.0+0.02*(temperature-25.0);
      //temperature compensation
      float compensationVoltage=averageVoltage/compensationCoefficient;
      
      //convert voltage value to tds value
      tdsValue=(133.42*compensationVoltage*compensationVoltage*compensationVoltage - 255.86*compensationVoltage*compensationVoltage + 857.39*compensationVoltage)*0.5;
      
      //Serial.print("voltage:");
      //Serial.print(averageVoltage,2);
      //Serial.print("V   ");
      Serial.print("TDS Value:");
      Serial.print(tdsValue,0);
      Serial.println("ppm");
    }
  }
}
*/
