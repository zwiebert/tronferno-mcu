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
#include "user_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "config/config.h"

#define D(x) 

#define CFG_NAMESPACE "Tronferno"
#define CFG_KEY "global.C"

config C = {
  MY_FER_CENTRAL_UNIT_ID,
  MY_MCU_UART_BAUD_RATE,
  MY_GEO_LONGITUDE,
  MY_GEO_LATITUDE,
  MY_GEO_TIMEZONE,
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
  { 0, },
#endif
  MY_GEO_TZ,
#ifdef USE_MQTT
  MY_MQTT_URL,
  MY_MQTT_USER,
  MY_MQTT_PASSWORD,
  MY_MQTT_ENABLE,
#endif
};

#ifdef CONFIG_BLOB

void read_config(uint32_t mask) {
  esp_err_t err = 0;
  nvs_handle my_handle;
  size_t len;

  if ((err = nvs_open(CFG_NAMESPACE, NVS_READONLY, &my_handle)) == ESP_OK) {
    err = (len = sizeof (C)), nvs_get_blob(my_handle, CFG_KEY, &C, &len);
    nvs_close(my_handle);
  }

}

void save_config(uint32_t mask) {
  esp_err_t err = 0;
  nvs_handle my_handle;

  if ((err = nvs_open(CFG_NAMESPACE, NVS_READWRITE, &my_handle)) == ESP_OK) {
    err = nvs_set_blob(my_handle, CFG_KEY, &C, sizeof (C));
    nvs_commit(my_handle);
    nvs_close(my_handle);
  }
}

#else

void read_config(uint32_t mask) {
  esp_err_t err = 0;
  nvs_handle handle;
  size_t len;

  if ((err = nvs_open(CFG_NAMESPACE, NVS_READONLY, &handle)) == ESP_OK) {

    if (mask & CONFIG_RECV) {
      int8_t temp;
      if (ESP_OK == nvs_get_i8(handle, "C_RECEIVER", &temp))
        C.app_recv = temp;
    }

    if (mask & CONFIG_TRANSM) {
      int8_t temp;
      if (ESP_OK == nvs_get_i8(handle, "C_TRANSM", &temp))
        C.app_transm = temp;
    }

    if (mask & CONFIG_VERBOSE) {
      int8_t temp;
      if (ESP_OK == nvs_get_i8(handle, "C_VERBOSE", &temp))
        C.app_verboseOutput = temp;
    }

    if (mask & CONFIG_CUID) {
      nvs_get_u32(handle, "C_CUID", &C.fer_centralUnitID);
    }

    if (mask & CONFIG_BAUD) {
      nvs_get_u32(handle, "C_BAUD", &C.mcu_serialBaud);
    }

    if (mask & CONFIG_WIFI_SSID) {
      (len = sizeof C.wifi_SSID), err = nvs_get_str(handle, "C_WIFI_SSID", C.wifi_SSID, &len);
      D(ets_printf("read wifi-ssid err=%x\n", err));
    }

    if (mask & CONFIG_WIFI_PASSWD) {
      (len = sizeof C.wifi_password), nvs_get_str(handle, "C_WIFI_PASSWD", C.wifi_password, &len);
    }

    if (mask & CONFIG_CFG_PASSWD) {
      (len = sizeof C.app_configPassword), nvs_get_str(handle, "C_CFG_PASSWD", C.app_configPassword, &len);
    }

    if (mask & CONFIG_LONGITUDE) {
      (len = sizeof C.geo_longitude), nvs_get_blob(handle, "C_LONGITUDE", &C.geo_longitude, &len);
    }

    if (mask & CONFIG_LATITUDE) {
      (len = sizeof C.geo_longitude), nvs_get_blob(handle, "C_LATITUDE", &C.geo_latitude, &len);
    }

    if (mask & CONFIG_TIZO) {
      (len = sizeof C.geo_timezone), nvs_get_blob(handle, "C_TIZO", &C.geo_timezone, &len);
    }

    if (mask & CONFIG_TZ) {
      (len = sizeof C.geo_tz), nvs_get_str(handle, "C_TZ", C.geo_tz, &len);
    }

    if (mask & CONFIG_GPIO) {
      (len = sizeof C.gpio), nvs_get_blob(handle, "C_GPIO", &C.gpio, &len);
    }

    if (mask & CONFIG_MQTT_URL) {
      (len = sizeof C.mqtt_url), nvs_get_str(handle, "C_MQTT_URL", C.mqtt_url, &len);
    }
    if (mask & CONFIG_MQTT_USER) {
      (len = sizeof C.mqtt_user), nvs_get_str(handle, "C_MQTT_USER", C.mqtt_user, &len);
    }
    if (mask & CONFIG_MQTT_PASSWD) {
      (len = sizeof C.mqtt_password), nvs_get_str(handle, "C_MQTT_PASSWD", C.mqtt_password, &len);
    }
    if (mask & CONFIG_MQTT_ENABLE) {
      int8_t temp;
      if (ESP_OK == nvs_get_i8(handle, "C_MQTT_ENABLE", &temp))
        C.mqtt_enable = temp;
    }

    nvs_close(handle);
  }

}

void save_config(uint32_t mask) {
  esp_err_t err = 0;
  nvs_handle handle;
  size_t len;
  
  if ((err = nvs_open(CFG_NAMESPACE, NVS_READWRITE, &handle)) == ESP_OK) {

    if (mask & CONFIG_RECV) {
      nvs_set_i8(handle, "C_RECEIVER", C.app_recv);
    }

    if (mask & CONFIG_TRANSM) {
      nvs_set_i8(handle, "C_TRANSM", C.app_transm);
    }

    if (mask & CONFIG_VERBOSE) {
      nvs_set_i8(handle, "C_VERBOSE", C.app_verboseOutput);
    }

    if (mask & CONFIG_CUID) {
      nvs_set_u32(handle, "C_CUID", C.fer_centralUnitID);
    }

    if (mask & CONFIG_BAUD) {
      nvs_set_u32(handle, "C_BAUD", C.mcu_serialBaud);
    }

    if (mask & CONFIG_WIFI_SSID) {
      err = nvs_set_str(handle, "C_WIFI_SSID", C.wifi_SSID);
      D(ets_printf("config: wifi_ssid err=%x\n", err));
    }

    if (mask & CONFIG_WIFI_PASSWD) {
      nvs_set_str(handle, "C_WIFI_PASSWD", C.wifi_password);
    }

    if (mask & CONFIG_CFG_PASSWD) {
      nvs_set_str(handle, "C_CFG_PASSWD", C.app_configPassword);
    }

   if (mask & CONFIG_LONGITUDE) {
      nvs_set_blob(handle, "C_LONGITUDE", &C.geo_longitude, sizeof C.geo_longitude);
    }

    if (mask & CONFIG_LATITUDE) {
      nvs_set_blob(handle, "C_LATITUDE", &C.geo_latitude, sizeof C.geo_latitude);
    }

    if (mask & CONFIG_TIZO) {
      nvs_set_blob(handle, "C_TIZO", &C.geo_timezone, sizeof C.geo_timezone);
    }

    if (mask & CONFIG_TZ) {
      nvs_set_str(handle, "C_TZ", C.geo_tz);
    }

    if (mask & CONFIG_GPIO) {
       nvs_set_blob(handle, "C_GPIO", &C.gpio, sizeof C.gpio);
     }

    if (mask & CONFIG_MQTT_URL) {
      nvs_set_str(handle, "C_MQTT_URL", C.mqtt_url);
    }
    if (mask & CONFIG_MQTT_USER) {
      nvs_set_str(handle, "C_MQTT_USER", C.mqtt_user);
    }
    if (mask & CONFIG_MQTT_PASSWD) {
      nvs_set_str(handle, "C_MQTT_PASSWD", C.mqtt_password);
    }
    if (mask & CONFIG_MQTT_ENABLE) {
      nvs_set_i8(handle, "C_MQTT_ENABLE", C.mqtt_enable);
    }

    nvs_commit(handle);
    nvs_close(handle);
  } else {
    D(ets_printf("error: cannot open config in nvs (err=%x)\n", err));
  }
}

#endif