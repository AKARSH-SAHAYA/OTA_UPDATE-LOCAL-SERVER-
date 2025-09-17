#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "nvs_flash.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"

static const char *TAG = "OTA_CLIENT";

// WiFi credentials
#define WIFI_SSID "Test"
#define WIFI_PASS "123456789"

// Flask server URL
#define URL "http://10.235.107.98:5000/firmware"

static bool ota_started = false;  // Prevent multiple OTA tasks

// OTA task
void ota_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Starting OTA update...");

    esp_http_client_config_t server_config = {
        .url = URL,
        .keep_alive_enable = true,
        .method = HTTP_METHOD_GET,
    };

    esp_http_client_handle_t client = esp_http_client_init(&server_config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to init HTTP client");
        vTaskDelete(NULL);
        return;
    }

    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        vTaskDelete(NULL);
        return;
    }

    int content_length = esp_http_client_fetch_headers(client);
    ESP_LOGI(TAG, "Content length: %d", content_length);

    const esp_partition_t *next_ota = esp_ota_get_next_update_partition(NULL);
    esp_ota_handle_t ota_operation;
    ESP_ERROR_CHECK(esp_ota_begin(next_ota, OTA_WITH_SEQUENTIAL_WRITES, &ota_operation));

    char buffer[1024];
    int data_read;
    int total_written = 0;

    while (true) {
        data_read = esp_http_client_read(client, buffer, sizeof(buffer));
        if (data_read < 0) {
            ESP_LOGE(TAG, "HTTP read error");
            esp_ota_abort(ota_operation);
            esp_http_client_cleanup(client);
            vTaskDelete(NULL);
            return;
        } else if (data_read > 0) {
            err = esp_ota_write(ota_operation, buffer, data_read);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "OTA write failed: %s", esp_err_to_name(err));
                esp_ota_abort(ota_operation);
                esp_http_client_cleanup(client);
                vTaskDelete(NULL);
                return;
            }
            total_written += data_read;
            ESP_LOGI(TAG, "Written %d bytes", total_written);
        } else if (data_read == 0) {
            if (esp_http_client_is_complete_data_received(client)) {
                ESP_LOGI(TAG, "All data received, total %d bytes", total_written);
                break;
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }

    ESP_ERROR_CHECK(esp_ota_end(ota_operation));
    ESP_ERROR_CHECK(esp_ota_set_boot_partition(next_ota));
    esp_http_client_cleanup(client);

    ESP_LOGI(TAG, "OTA complete! Rebooting...");
    esp_restart();
}

// WiFi event handler
static void wifi_handler(void* args, esp_event_base_t event_base,
                         int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Retry connecting to WiFi...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));

        if (!ota_started) {
            ota_started = true;
            xTaskCreate(&ota_task, "ota_task", 8192, NULL, 5, NULL);
        }
    }
}

// Initialize WiFi
void wifi_initialization()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_handler,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_station = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_station));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting OTA Client App");
    wifi_initialization();
}
