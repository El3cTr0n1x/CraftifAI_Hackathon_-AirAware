#include "metrics.h"
#include "app_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include <string.h>

static ambient_metrics_t metrics;
static SemaphoreHandle_t lock;

static uint32_t now_ms(void) { return (uint32_t)(esp_timer_get_time() / 1000ULL); }

void metrics_init(void)
{
    memset(&metrics, 0, sizeof(metrics));
    metrics.state = AMBIENT_QUIET;
    lock = xSemaphoreCreateMutex();
}

void metrics_update_wifi(uint16_t count, float variance, bool success)
{
    if (!lock || xSemaphoreTake(lock, portMAX_DELAY) != pdTRUE) return;
    if (success) { metrics.wifi_count = count; metrics.last_wifi_ms = now_ms(); }
    else metrics.wifi_failures++;
    metrics.signal_variance = variance;
    metrics.total_count = metrics.wifi_count + metrics.ble_count;
    xSemaphoreGive(lock);
}

void metrics_update_ble(uint16_t count, float variance, bool success)
{
    if (!lock || xSemaphoreTake(lock, portMAX_DELAY) != pdTRUE) return;
    if (success) { metrics.ble_count = count; metrics.last_ble_ms = now_ms(); }
    else metrics.ble_failures++;
    metrics.signal_variance = variance;
    metrics.total_count = metrics.wifi_count + metrics.ble_count;
    xSemaphoreGive(lock);
}

void metrics_update_temperature(float celsius, bool valid)
{
    if (!lock || xSemaphoreTake(lock, portMAX_DELAY) != pdTRUE) return;
    metrics.temperature_c = celsius;
    metrics.temperature_valid = valid;
    if (valid) metrics.last_temp_ms = now_ms();
    xSemaphoreGive(lock);
}

void metrics_set_state(ambient_state_t state)
{
    if (!lock || xSemaphoreTake(lock, portMAX_DELAY) != pdTRUE) return;
    metrics.state = state;
    xSemaphoreGive(lock);
}

void metrics_get(ambient_metrics_t *out)
{
    if (!out || !lock || xSemaphoreTake(lock, portMAX_DELAY) != pdTRUE) return;
    *out = metrics;
    xSemaphoreGive(lock);
}
