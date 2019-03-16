#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "string.h"

#include "rom/uart.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "../../userio/inout.h"
#include "main/common.h"
#include "config/config.h"
#include "fernotron/fer.h"
#include "main/wifi.h"

#define printf io_printf_fun
#ifndef DISTRIBUTION
#define D(x) x
#else
#define D(x)
#endif



// WIFI Station ////////////////////////////////////////

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about one event
 * - are we connected to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;


void
user_set_station_config(void) {
  esp_err_t esp_err;

  wifi_config_t sta_config = { };

  strncpy((char*) sta_config.sta.ssid, C.wifi_SSID, sizeof sta_config.sta.ssid - 1);
  strncpy((char*) sta_config.sta.password, C.wifi_password, sizeof sta_config.sta.password - 1);
  sta_config.sta.bssid_set = false;

  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
  ESP_ERROR_CHECK(esp_wifi_start());
  esp_err = esp_wifi_connect();
  if (esp_err != ESP_OK) {
    printf("error: esp_wifi_connect: %s", esp_err_to_name(esp_err));
  }
}

#if 0
void wst_reconnect(void) {
  uint8_t status = wifi_station_get_connect_status();
  io_printf_fun("wifi state: %d\n", (int) 0xff & status);

  //wifi_station_connect();
}

#endif
static int s_retry_num = 0;
const char *TAG = "wifistation";

//#define RETRY_RECONNECT (s_retry_num < 255)
#define RETRY_RECONNECT (1)

static ip4_addr_t ip4_address;

static esp_err_t event_handler(void *ctx, system_event_t *event) {

  switch (event->event_id) {

  case SYSTEM_EVENT_STA_START:
    esp_wifi_connect();
    break;

  case SYSTEM_EVENT_STA_GOT_IP:
    ip4_address = event->event_info.got_ip.ip_info.ip;
    ESP_LOGI(TAG, "got ip:%s", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
    s_retry_num = 0;
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    wifi_connected();
    break;


  case SYSTEM_EVENT_STA_DISCONNECTED:
  wifi_disconnected();
  {
    if (RETRY_RECONNECT) {
      ip4_address.addr = 0;
      esp_wifi_connect();
      xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
      s_retry_num++;
     // ESP_LOGI(TAG, "retry to connect to the AP");
    }
    break;
  }

  default:
    break;
  }
  return ESP_OK;
}

void
wifi_ip_address_as_string(char *buf, unsigned buf_len) {
  ip4addr_ntoa_r(&ip4_address, buf, buf_len);
}

void
setup_wifistation(void) {
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

  s_wifi_event_group = xEventGroupCreate();

  tcpip_adapter_init();
  ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );

  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );

  user_set_station_config();
}
