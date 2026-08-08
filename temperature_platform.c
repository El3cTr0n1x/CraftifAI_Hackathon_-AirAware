#include "ambient_modules.h"
#include "app_config.h"
#include "metrics.h"
#include "logger.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/temperature_sensor.h"

static const char *TAG = "temperature";
static void temp_task(void *arg)
{
    temperature_sensor_handle_t sensor = (temperature_sensor_handle_t)arg;
    for (;;) {
        float c = 0;
        esp_err_t e = temperature_sensor_get_celsius(sensor, &c);
        if (e == ESP_OK) { metrics_update_temperature(c, true); ESP_LOGI(TAG, "internal temperature %.1f C", c); }
        else { ESP_LOGE(TAG, "temperature read failed: %s", esp_err_to_name(e)); metrics_update_temperature(0, false); }
        vTaskDelay(pdMS_TO_TICKS(APP_TEMP_INTERVAL_MS));
    }
}
void platform_temperature_start(void)
{
    temperature_sensor_config_t cfg = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);
    temperature_sensor_handle_t sensor = NULL;
    esp_err_t e = temperature_sensor_install(&cfg, &sensor);
    if (e != ESP_OK) { ESP_LOGE(TAG, "temperature install failed: %s", esp_err_to_name(e)); return; }
    ESP_ERROR_CHECK(temperature_sensor_enable(sensor));
    xTaskCreate(temp_task, "temperature", 3072, sensor, 3, NULL);
}
