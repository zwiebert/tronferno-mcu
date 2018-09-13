/*
 * config.c
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */


/*
 * config_defaults.c
 *
 *  Created on: 16.09.2017
 *      Author: bertw
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "user_config.h"
#include "main/config.h"

#define CFG_NAMESPACE "Tronferno"
#define CFG_KEY "global.C"

config C = {
  MY_FER_CENTRAL_UNIT_ID,
  MY_MCU_ESP8266_BAUD_RATE,
  MY_GEO_LONGITUDE,
  MY_GEO_LATITUDE,
  MY_GEO_TZ,
  0, // app_rtcAdjust
  recvTick, // recv
  transmTick,// transm
  rtcAvrTime,//rtc
  vrbNone,  //verboseOutput
  MY_FER_GM_USE,
#ifdef USE_WLAN
  MY_WIFI_SSID,
  MY_WIFI_PASSWORD,
#endif
  MY_APP_CONFIG_PASSWORD,
  MY_APP_EXPERT_PASSWORD,
#ifdef ACCESS_GPIO
  { 0, }
#endif
};

void ICACHE_FLASH_ATTR read_config(void) {
  esp_err_t err = 0;
  nvs_handle my_handle;
  size_t len;

  if ((err = nvs_open(CFG_NAMESPACE, NVS_READONLY, &my_handle)) == ESP_OK) {
    err = (len = sizeof (C)), nvs_get_blob(my_handle, CFG_KEY, &C, &len);
  }

}

void ICACHE_FLASH_ATTR save_config(void) {
  esp_err_t err = 0;
  nvs_handle my_handle;

  if ((err = nvs_open(CFG_NAMESPACE, NVS_READWRITE, &my_handle)) == ESP_OK) {
    err = nvs_set_blob(my_handle, CFG_KEY, &C, sizeof (C));
  }
}

