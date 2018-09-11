#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "string.h"

#include "rom/uart.h"

#include "main/common.h"
#include "main/config.h"
#include "main/fer.h"
#include "main/inout.h"

#define printf io_printf_fun
#ifndef DISTRIBUTION
#define D(x) x
#else
#define D(x)
#endif



// WIFI Station ////////////////////////////////////////

void ICACHE_FLASH_ATTR
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

void ICACHE_FLASH_ATTR
setup_wifistation(void) {
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );

  user_set_station_config();
}
