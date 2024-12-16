/*
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
#define WIFI_SSID "foxide"
#define WIFI_PASS "gdi567op"

#define WIFI_CONNECTED_BIT BIT0

typedef struct {
    char ssid[32];
    char identity[64];
    char password[64];
} wifi_config_t_custom;

typedef struct {
    char *username;
    char *password;
} basic_auth_info_t;

static EventGroupHandle_t s_wifi_event_group;

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

static void wifi_init_sta(void);
// Initialize SNTP
static void initialize_sntp(void);

// Function to get formatted current time
static void get_formatted_time(char* buffer, size_t buffer_size);

void wifi_init(void);

static char *http_auth_basic(const char *username, const char *password);
static esp_err_t basic_auth_get_handler(httpd_req_t *req);
static void httpd_register_basic_auth(httpd_handle_t server);
static esp_err_t hello_get_handler(httpd_req_t *req);
static esp_err_t echo_post_handler(httpd_req_t *req);
static httpd_handle_t start_webserver(void);
static esp_err_t stop_webserver(httpd_handle_t server);
static void disconnect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
static void connect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);


static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_LOGI(WIFI_TAG, "Disconnected. Reconnecting...");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    // Initialize the TCP/IP stack
    ESP_ERROR_CHECK(esp_netif_init());

    // Create the default event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Create default Wi-Fi station
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Register event handler for Wi-Fi and IP events
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));

    // Configure Wi-Fi
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
             // Setting a password implies station will connect to all security modes
             // below WPA2, it does not harm as ESP32 will figure out the security mode
             // based on the AP's WPA2/WPA3 mode.
             //
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,

            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(WIFI_TAG, "wifi_init_sta finished.");

    // Wait for Wi-Fi connection
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(WIFI_TAG, "Connected to AP");
    } else {
        ESP_LOGI(WIFI_TAG, "Failed to connect to AP");
    }
}

// Initialize SNTP
static void initialize_sntp(void)
{
    ESP_LOGI(WIFI_TAG, "Initializing SNTP");

    // Set timezone offset (seconds). Example: UTC+2
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1); // Central European Time
    tzset();

    // Initialize SNTP
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org"); // Primary NTP server
    esp_sntp_setservername(1, "time.nist.gov"); // Secondary NTP server
    esp_sntp_init();

    // Wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;

    while (timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
        ESP_LOGI(WIFI_TAG, "Waiting for system time to be set...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    if (timeinfo.tm_year < (2016 - 1900)) {
        ESP_LOGE(WIFI_TAG, "Failed to obtain time");
    } else {
        ESP_LOGI(WIFI_TAG, "Time synchronized");
    }
}

// Function to get formatted current time
static void get_formatted_time(char* buffer, size_t buffer_size)
{
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", &timeinfo);
}


void wifi_init(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize Wi-Fi
    wifi_init_sta();

    // Initialize SNTP
    initialize_sntp();

    // Main loop: Get and print current time every second
    while (1) {
        char time_str[64];
        get_formatted_time(time_str, sizeof(time_str));
        ESP_LOGI(WIFI_TAG, "Current time: %s", time_str);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
















// AUTH HTTP BASIC
static char *http_auth_basic(const char *username, const char *password)
{
    size_t out;
    char *user_info = NULL;
    char *digest = NULL;
    size_t n = 0;
    int rc = asprintf(&user_info, "%s:%s", username, password);
    if (rc < 0) {
        ESP_LOGE(WIFI_TAG, "asprintf() returned: %d", rc);
        return NULL;
    }

    if (!user_info) {
        ESP_LOGE(WIFI_TAG, "No enough memory for user information");
        return NULL;
    }
    esp_crypto_base64_encode(NULL, 0, &n, (const unsigned char *)user_info, strlen(user_info));

    // 6: The length of the "Basic " string
    // n: Number of bytes for a base64 encode format
    // 1: Number of bytes for a reserved which be used to fill zero
    //
    digest = calloc(1, 6 + n + 1);
    if (digest) {
        strcpy(digest, "Basic ");
        esp_crypto_base64_encode((unsigned char *)digest + 6, n, &out, (const unsigned char *)user_info, strlen(user_info));
    }
    free(user_info);
    return digest;
}

// An AUTH HTTP GET handler
static esp_err_t basic_auth_get_handler(httpd_req_t *req)
{
    char *buf = NULL;
    size_t buf_len = 0;
    basic_auth_info_t *basic_auth_info = req->user_ctx;

    buf_len = httpd_req_get_hdr_value_len(req, "Authorization") + 1;
    if (buf_len > 1) {
        buf = calloc(1, buf_len);
        if (!buf) {
            ESP_LOGE(WIFI_TAG, "No enough memory for basic authorization");
            return ESP_ERR_NO_MEM;
        }

        if (httpd_req_get_hdr_value_str(req, "Authorization", buf, buf_len) == ESP_OK) {
            ESP_LOGI(WIFI_TAG, "Found header => Authorization: %s", buf);
        } else {
            ESP_LOGE(WIFI_TAG, "No auth value received");
        }

        char *auth_credentials = http_auth_basic(basic_auth_info->username, basic_auth_info->password);
        if (!auth_credentials) {
            ESP_LOGE(WIFI_TAG, "No enough memory for basic authorization credentials");
            free(buf);
            return ESP_ERR_NO_MEM;
        }

        if (strncmp(auth_credentials, buf, buf_len)) {
            ESP_LOGE(WIFI_TAG, "Not authenticated");
            httpd_resp_set_status(req, HTTPD_401);
            httpd_resp_set_type(req, "application/json");
            httpd_resp_set_hdr(req, "Connection", "keep-alive");
            httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Hello\"");
            httpd_resp_send(req, NULL, 0);
        } else {
            ESP_LOGI(WIFI_TAG, "Authenticated!");
            char *basic_auth_resp = NULL;
            httpd_resp_set_status(req, HTTPD_200);
            httpd_resp_set_type(req, "application/json");
            httpd_resp_set_hdr(req, "Connection", "keep-alive");
            int rc = asprintf(&basic_auth_resp, "{\"authenticated\": true,\"user\": \"%s\"}", basic_auth_info->username);
            if (rc < 0) {
                ESP_LOGE(WIFI_TAG, "asprintf() returned: %d", rc);
                free(auth_credentials);
                return ESP_FAIL;
            }
            if (!basic_auth_resp) {
                ESP_LOGE(WIFI_TAG, "No enough memory for basic authorization response");
                free(auth_credentials);
                free(buf);
                return ESP_ERR_NO_MEM;
            }
            httpd_resp_send(req, basic_auth_resp, strlen(basic_auth_resp));
            free(basic_auth_resp);
        }
        free(auth_credentials);
        free(buf);
    } else {
        ESP_LOGE(WIFI_TAG, "No auth header received");
        httpd_resp_set_status(req, HTTPD_401);
        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_hdr(req, "Connection", "keep-alive");
        httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Hello\"");
        httpd_resp_send(req, NULL, 0);
    }

    return ESP_OK;
}

static httpd_uri_t basic_auth = {
    .uri     = "/basic_auth",
    .method  = HTTP_GET,
    .handler = basic_auth_get_handler,
};

static void httpd_register_basic_auth(httpd_handle_t server)
{
    basic_auth_info_t *basic_auth_info = calloc(1, sizeof(basic_auth_info_t));
    if(basic_auth_info){
        basic_auth_info->username = CONFIG_EXAMPLE_BASIC_AUTH_USERNAME;
        basic_auth_info->password = CONFIG_EXAMPLE_BASIC_AUTH_PASSWORD;

        basic_auth.user_ctx = basic_auth_info;
        httpd_register_uri_handler(server, &basic_auth);
    }
}

// HTTP GET handler
static esp_err_t hello_get_handler(httpd_req_t *req)
{
    char* buf;
    size_t buf_len;

    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if(buf_len > 1){
        buf = malloc(buf_len);
        ESP_LOGE(WIFI_TAG, "buffer alloc failed!");
        if(httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(WIFI_TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-2") + 1;
    if(buf_len > 1){
        buf = malloc(buf_len);
        ESP_LOGE(WIFI_TAG, "buffer alloc failed");
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(WIFI_TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-1") + 1;
    if(buf_len > 1){
        buf = malloc(buf_len);
        ESP_LOGE(WIFI_TAG, "buffer alloc failed");
        if(httpd_req_get_hdr_value_str(req, "Test-Header-1", buf, buf_len) == ESP_OK){
            ESP_LOGI(WIFI_TAG, "Found header => Test-Header: %s", buf);
        }
        free(buf);
    }

    // Read URL query length and allocate memory
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if(buf_len > 1){
        buf = malloc(buf_len);
        ESP_LOGE(WIFI_TAG, "buffer alloc failed");
        if(httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            ESP_LOGI(WIFI_TAG, "Found URL query -> %s", buf);
            char param[EXAMPLE_HTTP_QUERY_KEY_MAX_LEN], dec_param[EXAMPLE_HTTP_QUERY_KEY_MAX_LEN] = {0};
            // Get value of expected key from query string
            if(httpd_query_key_value(buf, "query1", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(WIFI_TAG, "Found URL query parameter -> query1=%s", param);
               
                int decoded_len = custom_uri_encode(param, dec_param, sizeof(dec_param));
                if (decoded_len >= 0) {
                    ESP_LOGI(WIFI_TAG, "Decoded query parameter => %s", dec_param);
                } else {
                    ESP_LOGE(WIFI_TAG, "Failed to decode query parameter");
                }
            }
            if (httpd_query_key_value(buf, "query3", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(WIFI_TAG, "Found URL query parameter => query3=%s", param);
                
                int decoded_len = custom_uri_encode(param, dec_param, sizeof(dec_param));
                if (decoded_len >= 0) {
                    ESP_LOGI(WIFI_TAG, "Decoded query parameter => %s", dec_param);
                } else {
                    ESP_LOGE(WIFI_TAG, "Failed to decode query parameter");
                }
            }
            if (httpd_query_key_value(buf, "query2", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(WIFI_TAG, "Found URL query parameter => query2=%s", param);
                
                int decoded_len = custom_uri_encode(param, dec_param, sizeof(dec_param));
                if (decoded_len >= 0) {
                    ESP_LOGI(WIFI_TAG, "Decoded query parameter => %s", dec_param);
                } else {
                    ESP_LOGE(WIFI_TAG, "Failed to decode query parameter");
                }
            }
        }
        free(buf);
    }

    // custom response headers
    httpd_resp_set_hdr(req, "Custom-Header-1", "Custom-Value-1");
    httpd_resp_set_hdr(req, "Custom-Header-2", "Custom-Value-2");

    // send response
    const char* resp_str = (const char*) req->user_ctx;
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);

    if(httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(WIFI_TAG, "Request headers lost");
    }
    return ESP_OK;
}

static const httpd_uri_t hello = {
    .uri      = "/hello",
    .method   = HTTP_GET,
    .handler  = hello_get_handler,
    .user_ctx = "Hello World!"
};

// HTTP POST handler
static esp_err_t echo_post_handler(httpd_req_t *req)
{
    char buf[100];
    int ret, remaining = req->content_len;

    while(remaining>0){
        // Reading data for the request
        if((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <=0){
            if(ret == HTTPD_SOCK_ERR_TIMEOUT) {
                continue;
            }
            return ESP_FAIL;
        }
        // Sending back the same exact data
        httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;

         // Log data received 
        ESP_LOGI(WIFI_TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(WIFI_TAG, "%.*s", ret, buf);
        ESP_LOGI(WIFI_TAG, "====================================");
    }

    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t echo = {
    .uri      = "/echo",
    .method   = HTTP_POST,
    .handler  = echo_post_handler,
    .user_ctx = NULL
};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server   = NULL;
    httpd_config_t config   = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    //Start the httpd server
    ESP_LOGI(WIFI_TAG, "Starting server on port: %d", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(WIFI_TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &hello);
        httpd_register_uri_handler(server, &echo);
        httpd_register_basic_auth(server);

        return server;
    }

    ESP_LOGI(WIFI_TAG, "Error starting server!");
    return NULL;
}

static esp_err_t stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    return httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*)arg;
    if(*server){
        ESP_LOGI(WIFI_TAG, "Stopping webserver");
        if(stop_webserver(*server) == ESP_OK) {
            *server = NULL;
        } else {
            ESP_LOGE(WIFI_TAG, "Failed to stop http server");
        }
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(WIFI_TAG, "Starting webserver");
        *server = start_webserver();
    }
}


esp_err_t init_nvs()
{
    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    return ret;
}

// start AP mode and configuration portal
void start_config_portal()
{
    esp_netif_create_default_wifi_ap();

    wifi_config_t ap_config = {
        .ap = {
            .ssid = "ESP32",
            .ssid_len = strlen("ESP32"),
            .channel = 1,
            .password = "config123",
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA2_PSK
        },
    };

    if(strlen("config123") == 0) {
        ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(WIFI_TAG, "Configuration Portal started. Connect to ESP AP");
}

void wifi_init(void){
    init_nvs();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    if(!credentials_stored()){
        start_config_portal();
    } else {
        //Retrieving credentials from NVS
        wifi_config_t_custom config;

        //Configuring WPA-Enterprise with retrieved credentials
        esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)config.identity, strlen(config.identity));
        esp_wifi_sta_wpa2_ent_set_password((uint8_t *)config.password, strlen(config.password));
        esp_wifi_sta_wpa2_ent_set_eap_method("PEAP");
        esp_wifi_sta_wpa2_ent_set_phase2_method("MSCHAPV2");

        esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();
        esp_wifi_sta_wpa2_ent_enable(&config);

        wifi_config_t wifi_config = {
            .sta = {
                .ssid = "",
                .password = "",
                .threshold = {
                    .authmode = WIFI_AUTH_WPA2_ENTERPRISE
                }
            }
        };

        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());

        ESP_LOGI(WIFI_TAG, "Conencting to WPA-Enterprise....");
    }
}
*/