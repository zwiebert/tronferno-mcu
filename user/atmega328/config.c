/*
 * config.c
 *
 * Created: 24.08.2017 20:59:42
 *  Author: bertw
 */

#include "main/config.h"
#include <avr/eeprom.h>

config C;

struct ee_config {
	uint32_t zero;
	config c;
	uint8_t reserve[64];
};

struct ee_config ee_cfg EEMEM = {
	0,
	{	MY_FER_CENTRAL_UNIT_ID,
		MY_MCU_ATMEGA328_BAUD_RATE,
		MY_GEO_LONGITUDE,
		MY_GEO_LATITUDE,
		MY_GEO_TIMEZONE,
		MY_GEO_DST,
		0,
		recvTick,
		transmTick,
		rtcAvrTime,
		vrb1,
#ifdef USE_WLAN
		MY_WIFI_SSID,
		MY_WIFI_PASSWORD,
#endif
		MY_APP_CONFIG_PASSWORD,
		MY_APP_EXPERT_PASSWORD,
	}
};

struct ee_config tmp;

void read_config() {
	// memcpy(&C, &ee_cfg, sizeof (config));
	eeprom_read_block(&C, &ee_cfg.c, sizeof(config));
}

void save_config() {
	eeprom_update_block(&C, &ee_cfg.c, sizeof(config));
}

#if TEST_MODULE_CONFIG

bool testModule_config()
{

	return true;
}
#endif
