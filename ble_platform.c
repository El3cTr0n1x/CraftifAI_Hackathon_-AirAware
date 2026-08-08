#include "ambient_modules.h"
#include "app_config.h"
#include "metrics.h"
#include "logger.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "nvs_flash.h"
#include <string.h>

static const char *TAG="ble"; static volatile uint16_t found;
static int gap(struct ble_gap_event *e, void *arg) { (void)arg; if(e->type==BLE_GAP_EVENT_DISC) found++; else if(e->type==BLE_GAP_EVENT_DISC_COMPLETE) { metrics_update_ble(found,0,true); ESP_LOGI(TAG,"scan complete: %u devices",found); } return 0; }
static void scan_task(void *arg) { (void)arg; struct ble_gap_disc_params p={.itvl=0x50,.window=0x30,.passive=1,.filter_duplicates=1}; for(;;) { found=0; int rc=ble_gap_disc(BLE_OWN_ADDR_PUBLIC,APP_BLE_SCAN_DURATION_MS,&p,gap,NULL); if(rc) { ESP_LOGE(TAG,"scan failed rc=%d",rc); metrics_update_ble(0,0,false); } vTaskDelay(pdMS_TO_TICKS(APP_BLE_SCAN_INTERVAL_MS)); } }
static void sync_cb(void) { uint8_t t; if(ble_hs_id_infer_auto(0,&t)==0) xTaskCreate(scan_task,"ble_scan",4096,NULL,4,NULL); }
static void host_task(void *arg) { (void)arg; nimble_port_run(); nimble_port_freertos_deinit(); }
void platform_ble_start(void) { if(nimble_port_init()!=ESP_OK) { ESP_LOGE(TAG,"NimBLE init failed"); return; } ble_hs_cfg.sync_cb=sync_cb; nimble_port_freertos_init(host_task); }
