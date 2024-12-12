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

#include "bluetooth_component.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"

//Service UUID: ea36022b-9052-44b9-9e7c-d33e46260b2a
// Service UUID Split:
    // 0x2b, 0x02, 0x36, 0xea, <- little-endian (reversed)
    // 0x52, 0x90, <- little-endian (reversed)
    // 0xb9, 0x44, <- little-endian (reversed)
    // 0x9e, 0x7c, <- big-endian (original)
    // 0xd3, 0x3e, 0x46, 0x26, 0x0b, 0x2a <- big-endian (original)
// Temp Char. UUID: ea36022b-9052-44b9-9e7c-d33e46260b2b
// pH Char. UUID: ea36022b-9052-44b9-9e7c-d33e46260b2c
// Water Pump Char. UUID: ea36022b-9052-44b9-9e7c-d33e46260b2d


#define GATTS_TAG "BLE_ADV_TEST"
#define DEVICE_NAME "ESP32_BLE_Test"
#define PROFILE_NUM 1
#define PROFILE_APP_ID 0

static uint8_t last_written_data[512];
static uint16_t last_written_len  = 0;
static uint16_t cccd_handle       = 0;

static uint16_t char_handle          = 0;
static uint16_t conn_id              = 0;
static esp_gatt_if_t global_gatts_if = 0;
static bool notifications_enabled    = false;

uint8_t environmental_service_uuid128[16] = {
    0x2b, 0x02, 0x36, 0xea,
    0x52, 0x90,
    0xb9, 0x44,
    0x9e, 0x7c,
    0xd3, 0x3e, 0x46, 0x26, 0x0b, 0x2a,
};

static uint8_t raw_adv_data[30];
void setup_raw_adv_data() {
    raw_adv_data[0] = 0x02;
    raw_adv_data[1] = ESP_BLE_AD_TYPE_FLAG;
    raw_adv_data[2] = 0x06; // Discoverable Mode

    raw_adv_data[3] = 0x03;
    raw_adv_data[4] = ESP_BLE_AD_TYPE_TX_PWR; // TX Power
    raw_adv_data[5] = 0xEB; // -21 dBM

    raw_adv_data[6] = 0x11;
    raw_adv_data[7] = ESP_BLE_AD_TYPE_128SRV_CMPL; // Complete 128-bit UUID
    memcpy(&raw_adv_data[8], environmental_service_uuid128, sizeof(environmental_service_uuid128));
}

//Provides additional information upon request by a central device after
//  it has detected an advertising packet.
/*
uint8_t raw_scan_rsp_data[] = {
    0x0F, ESP_BLE_AD_TYPE_NAME_CMPL, 'E', 'S', 'P', '_', 'G', 'A', 'T', 'T', 'S', '_', 'D', 'E', 'M', 'O',
};
*/

static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp        = false,
    .include_name        = true,
    .include_txpower     = false,
    .min_interval        = 0x0006,
    .max_interval        = 0x0010,
    .appearance          = 0x00,
    .manufacturer_len    = 0,
    .p_manufacturer_data = NULL,
    .service_data_len    = 0,
    .p_service_data      = NULL,
    .service_uuid_len    = sizeof(environmental_service_uuid128),
    .p_service_uuid      = environmental_service_uuid128,
    .flag                = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

// scan response data
static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp        = true,
    .include_name        = true,
    .include_txpower     = true,
    .appearance          = 0x00,
    .manufacturer_len    = 0,
    .p_manufacturer_data = NULL,
    .service_data_len    = 0,
    .p_service_data      = NULL,
    .service_uuid_len    = sizeof(environmental_service_uuid128),
    .p_service_uuid      = environmental_service_uuid128,
    .flag                = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

//#endif /* CONFIG_SET_RAW_ADV_DATA */

static esp_ble_adv_params_t adv_params = {
    .adv_int_min       = 0x20,
    .adv_int_max       = 0x40,
    .adv_type          = ADV_TYPE_IND,
    .own_addr_type     = BLE_ADDR_TYPE_PUBLIC, // Uses a public Bluetooth Device Address
    .channel_map       = ADV_CHNL_ALL, // Advertises on all BLE advertising channels (37, 38, 39)
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY, // Allows any central device to scan and connect
};


struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

/*static struct gatts_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_APP_ID] = {
        .gatts_cb = gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE, 
        // ESP_GATT_IF_NONE - (gatts interface) Application Profile not linked to any client
    },
};
*/

void bluetooth_send_data(const char *data_str)
{
    if(notifications_enabled && global_gatts_if != 0 && conn_id !=0 && char_handle !=0) {
        esp_err_t err = esp_ble_gatts_send_indicate(
            global_gatts_if,
            conn_id,
            char_handle,
            strlen(data_str),
            (uint8_t*)data_str,
            false
        );

        if(err != ESP_OK) {
            ESP_LOGE(GATTS_TAG, "Failed to send notification: %s", esp_err_to_name(err));
        } else {
            ESP_LOGI(GATTS_TAG, "Notification sent: %s", data_str);
        }
    } else {
        ESP_LOGI(GATTS_TAG, "Notifications not enabled or not connected.");
    }
}

// Callback function for GAP events
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch(event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            ESP_LOGI(GATTS_TAG, "Advertising data set complete");
            // Start advertising
            esp_ble_gap_start_advertising(&adv_params);
            break;
        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
            ESP_LOGI(GATTS_TAG, "Scan response data set complete");
            break;
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(GATTS_TAG, "Advertising start failed, status = %d", param->adv_start_cmpl.status);
            }else {
                ESP_LOGI(GATTS_TAG, "Advertising started successfully");
            }
            break;
        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(GATTS_TAG, "Advertising stop failed, status = %d", param->adv_stop_cmpl.status);
            } else {
                ESP_LOGI(GATTS_TAG, "Advertising stopped successfully");
            }
            break;
        default:
            break;
        
    }
}

static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch(event) {
        case ESP_GATTS_REG_EVT:
            esp_gatt_srvc_id_t service_id = {
                .is_primary = true,
                .id.inst_id = 0x00,
                .id.uuid = {
                    .len = ESP_UUID_LEN_128,
                    .uuid = {.uuid128 = {
                        0x2b, 0x02, 0x36, 0xea,
                        0x52, 0x90,
                        0xb9, 0x44,
                        0x9e, 0x7c,
                        0xd3, 0x3e, 0x46, 0x26, 0x0b, 0x2a
                    }},
                },
            };
            // register the service with the ESP GATT server
            esp_err_t ret = esp_ble_gatts_create_service(gatts_if, &service_id, 4);
            if(ret != ESP_OK) {
                ESP_LOGE(GATTS_TAG, "Failed to create service: %s", esp_err_to_name(ret));
            }
            ESP_LOGI(GATTS_TAG, "GATT server registered, app_id %d", param->reg.app_id);
            break;

        case ESP_GATTS_CREATE_EVT:
            ESP_LOGI(GATTS_TAG, "Service created, handle %d", param->create.service_handle);
            uint16_t service_handle = param->create.service_handle;

            esp_bt_uuid_t char_uuid = {
                .len = ESP_UUID_LEN_128,
                .uuid = { .uuid128 = {
                    0x2b, 0x02, 0x36, 0xea, 0x52, 0x90, 0xb9, 0x44,
                    0x9e, 0x7c, 0xd3, 0x3e, 0x46, 0x26, 0x0b, 0x2b
                }},
            };
            ret = esp_ble_gatts_add_char(
                service_handle,
                &char_uuid,
                ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY, 
                NULL, 
                NULL
            );

            esp_bt_uuid_t cccd_uuid = {
                .len = ESP_UUID_LEN_16,
                .uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG,
            };

            ret = esp_ble_gatts_add_char_descr(
                service_handle,
                &cccd_uuid,
                ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                NULL,
                NULL
            );

            if (ret != ESP_OK) {
                ESP_LOGE(GATTS_TAG, "Failed to add characteristic: %s", esp_err_to_name(ret));
            }

            // Starting the service
            esp_ble_gatts_start_service(service_handle);
            break;
        case ESP_GATTS_ADD_CHAR_DESCR_EVT:
            ESP_LOGI(GATTS_TAG, "Descriptior added, attr_handle %d, status %d",
                        param->add_char_descr.attr_handle, param->add_char_descr.status);
            if (param->add_char_descr.status == ESP_GATT_OK) {
                cccd_handle = param->add_char_descr.attr_handle;
                ESP_LOGI(GATTS_TAG, "CCCD handle set to %d", cccd_handle);
            }
            break;
        case ESP_GATTS_READ_EVT:
            // For sending back read data to indicate success
            ESP_LOGI(GATTS_TAG, "Read event received");
            if(param->read.need_rsp){
                esp_gatt_rsp_t rsp = {0};
                rsp.attr_value.handle = param->read.handle;
                rsp.attr_value.len = last_written_len;
                memcpy(rsp.attr_value.value, last_written_data, last_written_len);

                esp_err_t err = esp_ble_gatts_send_response(
                    gatts_if,
                    param->read.conn_id,
                    param->read.trans_id,
                    ESP_GATT_OK,
                    &rsp
                );

                if(err != ESP_OK){
                    ESP_LOGE(GATTS_TAG, "Failed to send read response: %s", esp_err_to_name(err));
                }else{
                    ESP_LOGI(GATTS_TAG, "Sent read response with %d bytes", last_written_len);
                }
            }
            break;
        case ESP_GATTS_WRITE_EVT:
            ESP_LOGI(GATTS_TAG, "Write event received");
            esp_gatt_status_t status = ESP_GATT_OK;

            // Logging received write event data
            uint8_t *value = param->write.value;
            uint16_t length = param->write.len;
            ESP_LOGI(GATTS_TAG, "Data written: %.*s", length, value);

            last_written_len = length;
            memcpy(last_written_data, value, length);

            //Toggling notifications
            if(param->write.handle == cccd_handle) {
                //Check if notifications are enabled
                if(param->write.value[0] == 0x01) {
                    ESP_LOGI(GATTS_TAG, "Notifications enabled");
                } else {
                    ESP_LOGI(GATTS_TAG, "Notifications disabled");
                }
            }
            
            //Responding to client that wrote the data
            if(param->write.need_rsp){
                esp_gatt_rsp_t rsp = {0};
                rsp.attr_value.handle = param->write.handle;
                rsp.attr_value.len = 1;
                rsp.attr_value.value[0] = 0x00; //

                esp_err_t err = esp_ble_gatts_send_response(
                    gatts_if,
                    param->write.conn_id,
                    param->write.trans_id,
                    status,
                    &rsp
                );

                if(err != ESP_OK){
                    ESP_LOGE(GATTS_TAG, "Failed to send response: %s", esp_err_to_name(err));
                }
            }
            break;
        default:
            break;
    }
}

void bluetooth_init()
{
    esp_log_level_set("*", ESP_LOG_DEBUG);
    setup_raw_adv_data();

    esp_err_t ret;

    // NVS init. Used by e.g. by WiFi to save SSID and password
    ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Release memory for Classic BT because we use BLE
    ret = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    if(ret) {
        ESP_LOGE(GATTS_TAG, "Bluetooth controller memory release failed: %s", esp_err_to_name(ret));
        return;
    }

    // Bluetooth controller initialization
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if(ret) {
        ESP_LOGE(GATTS_TAG, "%s initialize controller failed", __func__);
        return;
    }
    
    // Enabling Bluetooth Controller
    // ESP_BT_MODE_BLE - BLE mode
    // ESP_BT_MODE_CLASSIC_BT - BT classic mode
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if(ret) {
        ESP_LOGE(GATTS_TAG, "%s enable controller failed", __func__);
        return;
    }

    // Bluedroid
    ret = esp_bluedroid_init();
    if(ret) {
        ESP_LOGE(GATTS_TAG, "%s init bluetooth failed", __func__);
        return;
    }

    // Bluedroid enable
    ret = esp_bluedroid_enable();
    if(ret) {
        ESP_LOGE(GATTS_TAG, "%s enable blueooth failed", __func__);
        return;
    }

    // GATT and GAP - reacting to events such as what happens when another device
    //  tries to read or write parameters and establish a connection.

    // GATT event handler  
    ret = esp_ble_gatts_register_callback(gatts_profile_event_handler);
    if(ret) {
        ESP_LOGE(GATTS_TAG, "gatts register error, error code = %x", ret);
        return;
    }

    // GAP callback event handler
    ret = esp_ble_gap_register_callback(gap_event_handler);
    if(ret) {
        ESP_LOGE(GATTS_TAG, "gap register error, error code = %x", ret);
        return;
    }
    

    // GAP device anme
    ret = esp_ble_gap_set_device_name(DEVICE_NAME);
    if(ret) {
        ESP_LOGE(GATTS_TAG, "Set device name failed, error code = %x", ret);
        return;
    }

    // Configure advertising data
    ret = esp_ble_gap_config_adv_data(&adv_data);
    if (ret){
        ESP_LOGE(GATTS_TAG, "Configure advertising data failed, error code = %x", ret);
        return;
    }

    ret = esp_ble_gatts_app_register(PROFILE_APP_ID);
    if(ret){
        ESP_LOGE(GATTS_TAG, "gatts app register error, error code = %x", ret);
        return;
    }


    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(512);
    if(local_mtu_ret){
        ESP_LOGE(GATTS_TAG, "set local MTU failed, error ode = %x", local_mtu_ret);
    }

    ESP_LOGI(GATTS_TAG, "BLE Advertising Setup Complete. Device Name: %s", DEVICE_NAME);
}