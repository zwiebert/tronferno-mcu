/*
 * config_defaults.c
 *
 *  Created on: 16.09.2017
 *      Author: bertw
 */
#include "user_config.h"
#include "main/config.h"


#include "data_flash.h"


config C = {
	MY_FER_CENTRAL_UNIT_ID,
	MY_MCU_ESP8266_BAUD_RATE,
	MY_GEO_LONGITUDE,
	MY_GEO_LATITUDE,
	MY_GEO_TIMEZONE,
	MY_GEO_DST,
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

#if ENABLE_SPIFFS
#include "../main/config_spiffs.h"
void ICACHE_FLASH_ATTR read_config(void) {
  if (!config_read()) {
#if LEGACY_STORAGE_READ
    read_data();  // FIXME: support old storage in parallel for some time
  }
#endif
}
void ICACHE_FLASH_ATTR save_config(void) {
  config_save();
#if LEGACY_STORAGE_WRITE
  save_data(); // FIXME: support old storage in parallel for some time
#endif
}
#else
void ICACHE_FLASH_ATTR read_config(void) {
	read_data();
}
void ICACHE_FLASH_ATTR save_config(void) {
	save_data();
}
#endif
