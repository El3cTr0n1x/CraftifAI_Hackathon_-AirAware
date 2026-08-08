#include "ambient_modules.h"
#include "app_config.h"
#include "metrics.h"
#include "logger.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include <string.h>
#include <math.h>

static const char *TAG = "wifi";
static void scan_task(void *arg)
{
    (void)arg;
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(APP_WIFI_SCAN_INTERVAL_MS));
        wifi_scan_config_t cfg = { .show_hidden = true, .scan_type = WIFI_SCAN_TYPE_ACTIVE };
        esp_err_t err = esp_wifi_scan_start(&cfg, true);
        if (err != ESP_OK) { ESP_LOGE(TAG, "scan failed: %s", esp_err_to_name(err)); metrics_update_wifi(0, 0, false); continue; }
        uint16_t n = APP_WIFI_SCAN_MAX_APS;
        wifi_ap_record_t records[APP_WIFI_SCAN_MAX_APS];
        err = esp_wifi_scan_get_ap_records(&n, records);
        if (err != ESP_OK) { ESP_LOGE(TAG, "get records failed: %s", esp_err_to_name(err)); metrics_update_wifi(0, 0, false); continue; }
        float sum = 0, sum2 = 0; uint16_t count = 0;
        for (uint16_t i = 0; i < n; ++i) { if (records[i].ssid[0] == 0) continue; float r = records[i].rssi; sum += r; sum2 += r*r; count++; }
        float variance = count ? (sum2 / count) - (sum / count) * (sum / count) : 0;
        ESP_LOGI(TAG, "scan complete: %u APs, variance %.1f", count, variance);
        metrics_update_wifi(count, variance, true);
    }
}

void platform_wifi_start(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    esp_err_t e = esp_event_loop_create_default();
    if (e != ESP_OK && e != ESP_ERR_INVALID_STATE) ESP_ERROR_CHECK(e);
    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init));
    wifi_config_t ap = {0};
    memcpy(ap.ap.ssid, APP_AP_SSID, sizeof(APP_AP_SSID));
    memcpy(ap.ap.password, APP_AP_PASSWORD, sizeof(APP_AP_PASSWORD));
    ap.ap.ssid_len = strlen(APP_AP_SSID); ap.ap.channel = APP_AP_CHANNEL;
    ap.ap.max_connection = APP_AP_MAX_CLIENTS; ap.ap.authmode = WIFI_AUTH_WPA2_PSK;
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "SoftAP started: %s", APP_AP_SSID);
    xTaskCreate(scan_task, "wifi_scan", 4096, NULL, 4, NULL);
}
