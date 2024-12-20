#ifndef BLUETOOTH_COMPONENT_H
#define BLUETOOTH_COMPONENT_H

#ifndef CONFIG_IDF_TARGET_LINUX

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt_defs.h"
#include "esp_gatt_common_api.h"
#include "sdkconfig.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"

#define GATTS_TAG "BLE_ADV_TEST"
#define DEVICE_NAME "ESP32_BLE_Test"
#define PROFILE_NUM 1
#define PROFILE_APP_ID 0

void setup_raw_adv_data();
void bluetooth_send_data(const char *data_str);
void bluetooth_init();
//private: static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
//private: static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

#endif /* CONFIG_IDF_TARGET_LINUX */

#endif /* BLUETOOTH_COMPONENT_H */