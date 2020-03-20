/*
 * config_defaults.c
 *
 *  Created on: 16.09.2017
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "config/config.h"


#include "esp8266/data_flash.h"

#if 0
config C = {
	MY_FER_CENTRAL_UNIT_ID,
	MY_MCU_UART_BAUD_RATE,
	MY_GEO_LONGITUDE,
	MY_GEO_LATITUDE,
#ifdef POSIX_TIME
	MY_GEO_TZ,
#else
	MY_GEO_TIMEZONE,
	MY_GEO_DST,
#endif
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
#ifdef USE_NTP
  .ntp_server = MY_NTP_SERVER,
#endif
};
#endif

#if ENABLE_SPIFFS
#include "config/config_spiffs.h"
void  mcu_read_config(uint32_t mask) {
  if (!config_read()) {
#if LEGACY_STORAGE_READ
    read_data();  // FIXME: support old storage in parallel for some time
  }
#endif
}
void  mcu_save_config(uint32_t mask) {
  config_save();
#if LEGACY_STORAGE_WRITE
  save_data(); // FIXME: support old storage in parallel for some time
#endif
}
#else
void  mcu_read_config(u32 mask) {
	read_data();
}
void  mcu_save_config(u32 mask) {
	save_data();
}
#endif
