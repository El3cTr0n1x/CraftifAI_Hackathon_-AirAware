#include "ambient_modules.h"
#include "app_config.h"
#include "logger.h"
#include "led_strip.h"
#include "led_strip_rmt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "metrics.h"

static led_strip_handle_t strip;
static const char *TAG = "led";
static void led_task(void *arg)
{
    (void)arg; ambient_state_t last = (ambient_state_t)-1;
    for (;;) { ambient_metrics_t m; metrics_get(&m); if (m.state != last) { last = m.state; uint8_t r=0,g=0,b=0; if (last == AMBIENT_QUIET) b=40; else if (last == AMBIENT_PRESENT) r=50,g=20; else r=60; led_strip_set_pixel(strip, 0, r,g,b); led_strip_refresh(strip); ESP_LOGI(TAG, "LED state %d", last); } vTaskDelay(pdMS_TO_TICKS(500)); }
}
void platform_led_start(void)
{
    led_strip_config_t cfg = { .strip_gpio_num = APP_LED_GPIO, .max_leds = APP_LED_COUNT, .led_model = LED_MODEL_WS2812, .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB };
    led_strip_rmt_config_t rmt = { .resolution_hz = 10 * 1000 * 1000 };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&cfg, &rmt, &strip));
    xTaskCreate(led_task, "led", 3072, NULL, 2, NULL);
}
