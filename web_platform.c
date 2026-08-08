#include "ambient_modules.h"
#include "metrics.h"
#include "logger.h"
#include "esp_http_server.h"
#include <stdio.h>

static const char *PAGE = "<!doctype html><html><head><meta name='viewport' content='width=device-width'><title>Ambient C3</title></head><body><h1>Ambient Awareness</h1><pre id='s'>Loading...</pre><script>setInterval(()=>fetch('/api/status').then(r=>r.json()).then(x=>s.textContent=JSON.stringify(x,null,2)),2000);</script></body></html>";
static esp_err_t root(httpd_req_t *r) { httpd_resp_set_type(r,"text/html"); return httpd_resp_send(r,PAGE,HTTPD_RESP_USE_STRLEN); }
static esp_err_t status(httpd_req_t *r) { ambient_metrics_t m; metrics_get(&m); char b[512]; snprintf(b,sizeof(b),"{\"state\":%d,\"wifi\":%u,\"ble\":%u,\"devices\":%u,\"variance\":%.2f,\"temperature_c\":%.2f,\"temperature_valid\":%s,\"wifi_failures\":%lu,\"ble_failures\":%lu}",m.state,m.wifi_count,m.ble_count,m.total_count,m.signal_variance,m.temperature_c,m.temperature_valid?"true":"false",(unsigned long)m.wifi_failures,(unsigned long)m.ble_failures); httpd_resp_set_type(r,"application/json"); httpd_resp_set_hdr(r,"Cache-Control","no-store"); return httpd_resp_send(r,b,HTTPD_RESP_USE_STRLEN); }
void platform_web_start(void) { httpd_config_t c=HTTPD_DEFAULT_CONFIG(); httpd_handle_t s=NULL; if (httpd_start(&s,&c)==ESP_OK) { httpd_uri_t a={.uri="/",.method=HTTP_GET,.handler=root}; httpd_uri_t b={.uri="/api/status",.method=HTTP_GET,.handler=status}; httpd_register_uri_handler(s,&a); httpd_register_uri_handler(s,&b); ESP_LOGI("web","dashboard at http://192.168.4.1/"); } }
