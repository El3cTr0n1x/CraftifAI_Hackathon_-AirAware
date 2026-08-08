#ifndef AMBIENT_METRICS_H
#define AMBIENT_METRICS_H

#include <stdbool.h>
#include <stdint.h>

typedef enum { AMBIENT_QUIET = 0, AMBIENT_PRESENT, AMBIENT_ACTIVE } ambient_state_t;

typedef struct {
    uint16_t wifi_count;
    uint16_t ble_count;
    uint16_t total_count;
    float signal_variance;
    float temperature_c;
    bool temperature_valid;
    uint32_t wifi_failures;
    uint32_t ble_failures;
    uint32_t last_wifi_ms;
    uint32_t last_ble_ms;
    uint32_t last_temp_ms;
    ambient_state_t state;
} ambient_metrics_t;

void metrics_init(void);
void metrics_update_wifi(uint16_t count, float variance, bool success);
void metrics_update_ble(uint16_t count, float variance, bool success);
void metrics_update_temperature(float celsius, bool valid);
void metrics_get(ambient_metrics_t *out);
void metrics_set_state(ambient_state_t state);

#endif
