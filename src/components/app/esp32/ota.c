#include "app_config/proj_app_cfg.h"
#ifdef USE_OTA
#include "app/ota.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"

#include "nvs.h"
#include "nvs_flash.h"
#include "misc/int_types.h"

static const char *TAG = "simple_ota_example";
extern const u8 server_cert_pem_start[] asm("_binary_ca_cert_pem_start");

static ota_state_T state;

ota_state_T ota_getState(void) { return state; }

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;

static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

static char *Firmware_url = 0;

void simple_ota_example_task(void * pvParameter) {

  // ESP_LOGI(TAG, "Starting OTA example");

  esp_http_client_config_t config = { .url = Firmware_url, .cert_pem = (char *) server_cert_pem_start, .event_handler = _http_event_handler, };
  //vTaskDelete(NULL);
  state = ota_RUN;
  esp_err_t ret = esp_https_ota(&config);
  if (ret == ESP_OK) {
    state = ota_DONE;
  } else {
    state = ota_FAIL;
    // ESP_LOGE(TAG, "Firmware upgrade failed");
  }
  free(Firmware_url);
  vTaskDelete(NULL);
}

bool ota_doUpdate(const char *firmware_url) {
  Firmware_url = malloc(strlen(firmware_url)+1);
  strcpy (Firmware_url, firmware_url);
  xTaskCreate(&simple_ota_example_task, "ota_example_task", 16384, NULL, 5, NULL);
  return false;
}

#if 0
void ota_loop() { // TODO: under construction
  switch (state) {
  case ota_NONE:
      return;


  }
  state = ota_NONE;
}
#endif
void ota_setup()
{

}

#endif
