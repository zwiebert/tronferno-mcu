/*
 * config.h
 *
 * Created: 24.08.2017 20:56:00
 *  Author: bertw
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "app/proj_app_cfg.h"
#include "txtio/inout.h"
#include "fernotron/astro.h"

#if !defined MCU_ESP8266
#if __has_include("sensitive/defaults.h")
#define HAVE_USER_DEFAULTS
#endif
#endif

#if defined HAVE_USER_DEFAULTS && ! defined DISTRIBUTION
#include "sensitive/defaults.h"
#endif

#ifndef MY_FER_CENTRAL_UNIT_ID
#ifdef CONFIG_MY_FER_CENTRAL_UNIT_ID
#define MY_FER_CENTRAL_UNIT_ID CONFIG_MY_FER_CENTRAL_UNIT_ID
#else
#define MY_FER_CENTRAL_UNIT_ID 0x801234
#endif
#endif

#ifndef MY_MCU_UART_BAUD_RATE
#ifdef CONFIG_MY_MCU_UART_BAUD_RATE
#define MY_MCU_UART_BAUD_RATE CONFIG_MY_MCU_UART_BAUD_RATE
#else
#define MY_MCU_UART_BAUD_RATE 115200
#endif
#endif

// Berlin/Germany
#ifndef MY_GEO_LONGITUDE
#ifdef CONFIG_MY_GEO_LONGITUDE
#define MY_GEO_LONGITUDE CONFIG_MY_GEO_LONGITUDE
#else
#define MY_GEO_LONGITUDE +13.38
#endif
#endif

#ifndef MY_GEO_LATITUDE
#ifdef CONFIG_MY_GEO_LATITUDE
#define MY_GEO_LATITUDE CONFIG_MY_GEO_LATITUDE
#else
#define MY_GEO_LATITUDE +52.53
#endif
#endif

#ifndef MY_GEO_TZ
#ifdef CONFIG_MY_GEO_TZ
#define MY_GEO_TZ CONFIG_MY_GEO_TZ
#else
#define MY_GEO_TZ "CET-1CEST-2,M3.5.0,M10.5.0" // POSIX TZ format
#endif
#endif

#ifndef MY_GEO_TIMEZONE
#ifdef CONFIG_MY_GEO_TIMEZONE
#define MY_GEO_TIMEZONE CONFIG_MY_GEO_TIMEZONE
#else
#define MY_GEO_TIMEZONE +1.0
#endif
#endif

#ifndef MY_GEO_DST
#ifdef CONFIG_MY_GEO_DST
#define MY_GEO_DST CONFIG_MY_GEO_DST
#else
#define MY_GEO_DST dstEU
#endif
#endif

#ifndef MY_WIFI_SSID
#ifdef CONFIG_MY_WIFI_SSID
#define MY_WIFI_SSID CONFIG_MY_WIFI_SSID
#else
#define MY_WIFI_SSID ""
#endif
#endif

#ifndef MY_WIFI_PASSWORD
#ifdef CONFIG_MY_WIFI_PASSWORD
#define MY_WIFI_PASSWORD CONFIG_MY_WIFI_PASSWORD
#else
#define MY_WIFI_PASSWORD ""
#endif
#endif

#ifndef MY_NTP_SERVER
#ifdef CONFIG_MY_NTP_SERVER
#define MY_NTP_SERVER CONFIG_MY_NTP_SERVER
#else
#define MY_NTP_SERVER "pool.ntp.org"
#endif
#endif

#ifndef MY_APP_CONFIG_PASSWORD
#ifdef CONFIG_MY_APP_CONFIG_PASSWORD
#define MY_APP_CONFIG_PASSWORD CONFIG_MY_APP_CONFIG_PASSWORD
#else
#define MY_APP_CONFIG_PASSWORD ""
#endif
#endif

#ifndef MY_APP_EXPERT_PASSWORD
#ifdef CONFIG_MY_APP_EXPERT_PASSWORD
#define MY_APP_EXPERT_PASSWORD CONFIG_MY_APP_EXPERT_PASSWORD
#else
#define MY_APP_EXPERT_PASSWORD ""
#endif
#endif

#ifndef MY_MQTT_ENABLE
#ifdef CONFIG_MY_MQTT_ENABLE
#define MY_MQTT_ENABLE CONFIG_MY_MQTT_ENABLE
#else
#define MY_MQTT_ENABLE 0
#endif
#endif

#ifndef MY_MQTT_URL
#ifdef CONFIG_MY_MQTT_URL
#define MY_MQTT_URL CONFIG_MY_MQTT_URL
#else
#define MY_MQTT_URL ""
#endif
#endif

#ifndef MY_MQTT_USER
#ifdef CONFIG_MY_MQTT_USER
#define MY_MQTT_USER CONFIG_MY_MQTT_USER
#else
#define MY_MQTT_USER ""
#endif
#endif

#ifndef MY_MQTT_PASSWORD
#ifdef CONFIG_MY_MQTT_PASSWORD
#define MY_MQTT_PASSWORD CONFIG_MY_MQTT_PASSWORD
#else
#define MY_MQTT_PASSWORD ""
#endif
#endif

#ifndef MY_HTTP_ENABLE
#ifdef CONFIG_MY_HTTP_ENABLE
#define MY_HTTP_ENABLE CONFIG_MY_HTTP_ENABLE
#else
#define MY_HTTP_ENABLE 0
#endif
#endif

#ifndef MY_HTTP_USER
#ifdef CONFIG_MY_HTTP_USER
#define MY_HTTP_USER CONFIG_MY_HTTP_USER
#else
#define MY_HTTP_USER ""
#endif
#endif

#ifndef MY_HTTP_PASSWORD
#ifdef CONFIG_MY_HTTP_PASSWORD
#define MY_HTTP_PASSWORD CONFIG_MY_HTTP_PASSWORD
#else
#define MY_HTTP_PASSWORD ""
#endif
#endif

#ifndef MY_NETWORK_CONNECTION
#define MY_NETWORK_CONNECTION nwWlanAp
#define MY_NETWORK_CONNECTION_OLD_USERS nwWlanSta
#endif

#ifndef MY_LAN_PHY
#define MY_LAN_PHY lanPhyLAN8270
#endif

#ifndef MY_LAN_PWR_GPIO
#define MY_LAN_PWR_GPIO 5
#endif

/*
 esp-idf: kconfig does not support float numbers...
config MY_GEO_LONGITUDE
  float "geographical longitude"
  default +13.38

config MY_GEO_LATITUDE
  float "geographical latitude"
  default +52.53

 */

#ifndef MY_FER_GM_USE
#define MY_FER_GM_USE 0x77777777
#endif

#include "app/proj_app_cfg.h"

enum dst {
	dstNone, dstEU, dstUS, dstAlways,
};

enum receiver {
	recvNone, recvTick
};

enum transmitter {
	transmNone, transmTick
};

enum rtclock {
	rtcNone, rtcAvrTime
};

enum verbosity {
	vrbNone, vrb1, vrb2, vrb3, vrb4, vrbDebug
};

#ifdef USE_LAN
enum lanPhy {
  lanPhyNone, lanPhyLAN8270, lanPhyRTL8201, lanPhyIP101, lanPhyLEN,
};
#endif

#ifdef USE_NETWORK
enum nwConnection {
  nwNone, nwWlanSta, nwWlanAp, nwLan, nwLEN,
};
#endif

#ifdef POSIX_TIME
void cfg_tz2timezone(void);
#endif

typedef struct {
	uint32_t fer_centralUnitID, mcu_serialBaud;
	float geo_longitude, geo_latitude;

	float geo_timezone;
#ifdef MDR_TIME
  enum dst geo_dST;
#endif
  int32_t app_rtcAdjust;
	enum receiver app_recv;
	enum transmitter app_transm;
	enum rtclock app_rtc;
	enum verbosity app_verboseOutput;
	uint32_t fer_usedMembers; // each of the nibbles 1-7 stands for a group. nibble 1 == group 1. nibble 0 = number of used Groups (stored for the front-end, not used here on the MCU)

#ifdef USE_WLAN
	char wifi_SSID[32];
	char wifi_password[64];
#endif
	char app_configPassword[16];
	char app_expertPassword[16];
#ifdef ACCESS_GPIO
	 enum mcu_pin_state gpio[CONFIG_GPIO_SIZE];
#endif
#ifdef POSIX_TIME
  char geo_tz[32];
#endif
#ifdef USE_MQTT
  char mqtt_url[64];
  char mqtt_user[16];
  char mqtt_password[31];
  int8_t mqtt_enable;
#endif
#ifdef USE_HTTP
  char http_user[16];
  char http_password[31];
  int8_t http_enable;
#endif
#ifdef USE_NTP
  char ntp_server[64];
#endif
#ifdef USE_NETWORK
  enum nwConnection network;
#endif
#ifdef USE_LAN
  enum lanPhy lan_phy;
  int8_t lan_pwr_gpio;
#endif
  enum astroCorrection astroCorrection;
} config;

extern config C;




enum configItem {
CB_RECV,
CB_TRANSM,
CB_CUID,
CB_USED_MEMBERS,
CB_BAUD,
CB_GPIO,
CB_VERBOSE,
#ifdef USE_WLAN
CB_WIFI_SSID,
CB_WIFI_PASSWD,
#endif
CB_CFG_PASSWD,
CB_LONGITUDE,
CB_LATITUDE,
#ifndef POSIX_TIME
CB_TIZO,
#else
CB_TZ,
#endif
#ifdef MDR_TIME
CB_DST,
#endif
#ifdef USE_MQTT
CB_MQTT_URL,
CB_MQTT_USER,
CB_MQTT_PASSWD,
CB_MQTT_ENABLE,
#endif
#ifdef USE_HTTP
CB_HTTP_USER,
CB_HTTP_PASSWD,
CB_HTTP_ENABLE,
#endif
#ifdef USE_NTP
CB_NTP_SERVER,
#endif
#ifdef USE_NETWORK
CB_NETWORK_CONNECTION,
CB_LAN_PHY,
CB_LAN_PWR_GPIO,
#endif
CB_ASTRO_CORRECTION,
CB_size
};

// save C to persistent storage
void save_config_all();
void save_config_item(enum configItem item);
void save_config(uint32_t mask);

// restore C from persistent storage
void read_config_all();
void read_config_item(enum configItem item);
void read_config(uint32_t mask);






#endif /* CONFIG_H_ */

