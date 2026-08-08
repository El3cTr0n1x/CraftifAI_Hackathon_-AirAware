#include "metrics.h"
#include "app_config.h"
#include "logger.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

static const char *TAG = "classifier";
static ambient_state_t classify(const ambient_metrics_t *m)
{
    if (m->total_count <= APP_QUIET_MAX_DEVICES && m->signal_variance <= APP_QUIET_MAX_VARIANCE) return AMBIENT_QUIET;
    if (m->total_count >= APP_ACTIVE_MIN_DEVICES || m->signal_variance >= APP_ACTIVE_MIN_VARIANCE) return AMBIENT_ACTIVE;
    return AMBIENT_PRESENT;
}
static void classifier_task(void *arg)
{
    (void)arg; ambient_state_t pending = AMBIENT_QUIET; int64_t since = esp_timer_get_time();
    for (;;) {
        ambient_metrics_t m; metrics_get(&m); ambient_state_t next = classify(&m);
        if (next != pending) { pending = next; since = esp_timer_get_time(); }
        if (pending != m.state && esp_timer_get_time() - since >= (int64_t)APP_STATE_COOLDOWN_MS * 1000) {
            metrics_set_state(pending); ESP_LOGI(TAG, "ambient state transition -> %d", pending);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
void classifier_start(void) { xTaskCreate(classifier_task, "classifier", 4096, NULL, 3, NULL); }
