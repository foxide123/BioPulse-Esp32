#ifndef WIFI_COMPONENT_H
#define WIFI_COMPONENT_H

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <esp_http_server.h>
#include <time.h>
#include <sys/time.h>
#include <esp_system.h>
#include "esp_log.h"
#include "esp_tls_crypto.h"
#include "esp_sntp.h"
#include <ctype.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "wifi_component.h"

#define EXAMPLE_HTTP_QUERY_KEY_MAX_LEN 64
#define HTTPD_401 "401 UNAUTHORIZED"
#define WIFI_TAG "WIFI_COMPONENT"

#define WIFI_CONNECTED_BIT BIT0

void wifi_init(void);
esp_err_t init_nvs();
void start_config_portal();
void wifi_init(void);

#endif