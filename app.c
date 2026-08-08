#include "app.h"
#include "app_config.h"
#include "logger.h"
#include "metrics.h"
#include "ambient_modules.h"
#include "nvs_flash.h"

static const char *TAG = "app";

void app_start(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    metrics_init();
    platform_wifi_start();
    platform_ble_start();
    platform_temperature_start();
    platform_led_start();
    platform_web_start();
    classifier_start();
    ESP_LOGI(TAG, "ambient awareness firmware started");
}
