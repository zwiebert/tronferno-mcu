/*
 * config.h
 *
 * Created: 24.08.2017 20:56:00
 *  Author: bertw
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "app_config/proj_app_cfg.h"
#include "txtio/inout.h"
#include "fernotron/astro.h"

#include "net/ethernet.h"

#include "app_config/proj_app_cfg.h"

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

// CONFIG_IGORE_MASK holds bits to always ignore when read/save config
// so these options will not be persistent during restart
//
#define CONFIG_IGORE_MASK ((1<<CB_RECV)|(1<<CB_TRANSM))

enum configItem {
  CB_RECV, CB_TRANSM, CB_CUID, CB_USED_MEMBERS, CB_BAUD, CB_GPIO, CB_VERBOSE,
#ifdef USE_WLAN
  CB_WIFI_SSID, CB_WIFI_PASSWD,
#endif
  CB_CFG_PASSWD, CB_LONGITUDE, CB_LATITUDE,
#ifndef POSIX_TIME
CB_TIZO,
#else
  CB_TZ,
#endif
#ifdef MDR_TIME
  CB_DST,
#endif
#ifdef USE_MQTT
  CB_MQTT_URL, CB_MQTT_USER, CB_MQTT_PASSWD, CB_MQTT_ENABLE,
#endif
#ifdef USE_HTTP
  CB_HTTP_USER, CB_HTTP_PASSWD, CB_HTTP_ENABLE,
#endif
#ifdef USE_NTP
  CB_NTP_SERVER,
#endif
  CB_ASTRO_CORRECTION,
#ifdef USE_NETWORK
  CB_NETWORK_CONNECTION,
#endif
#ifdef USE_LAN
  CB_LAN_PHY, CB_LAN_PWR_GPIO,
#endif


//-----------
  CB_size
};

//#define CM_ALL ((uint32_t)((1UL<<CB_size)-1))
#define CM_ALL ((uint32_t)~0UL)

// save C to persistent storage
void save_config_all();
void save_config_item(enum configItem item);
void save_config(uint32_t mask);

// restore C from persistent storage
void read_config_all();
void read_config_item(enum configItem item);
void read_config(uint32_t mask);

void config_setup(void);

#ifdef USE_CONFIG_KVS
void config_read_kvs(uint32_t mask); //IMPLEMENT_ME
void config_save_kvs(uint32_t mask); //IMPLEMENT_ME
#else
void mcu_read_config(uint32_t mask); //IMPLEMENT_ME
void mcu_save_config(uint32_t mask); //IMPLEMENT_ME
#endif

#endif /* CONFIG_H_ */

