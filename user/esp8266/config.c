/*
 * config_defaults.c
 *
 *  Created on: 16.09.2017
 *      Author: bertw
 */
#include "config.h"

#include "data_flash.h"

config C = {
	MY_FER_CENTRAL_UNIT_ID,
	MY_MCU_ESP8266_BAUD_RATE,
	MY_GEO_LONGITUDE,
	MY_GEO_LATITUDE,
	MY_GEO_TIMEZONE,
	MY_GEO_DST,
	recvTick, // recv
	transmTick,// transm
	rtcAvrTime,//rtc
	MY_WIFI_SSID,
	MY_WIFI_PASSWORD
};

void ICACHE_FLASH_ATTR read_config(void) {
	read_data();
}
void ICACHE_FLASH_ATTR save_config(void) {
	save_data();
}
