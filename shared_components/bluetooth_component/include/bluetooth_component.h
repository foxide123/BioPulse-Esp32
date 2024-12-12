#ifndef BLUETOOTH_COMPONENT_H
#define BLUETOOTH_COMPONENT_H

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"

void bluetooth_send_data(const char *data_str);
void bluetooth_init();
//private: static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
//private: static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

#endif /* BLUETOOTH_COMPONENT_H */