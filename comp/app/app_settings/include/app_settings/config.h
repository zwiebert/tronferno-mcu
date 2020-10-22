/**
 * \file    app_settings/config.h
 * \brief   Application settings
 */

#pragma once

#include "app_config/proj_app_cfg.h"
#include "config_kvs/config.h"
#include "txtio/inout.h"
#include "fernotron/types.h"
//#include "net/ethernet_setup.h"
#include "gpio/pin.h"
#include "stdbool.h"

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

enum nwConnection {
  nwNone, nwWlanSta, nwWlanAp, nwLan, nwLEN,
};


#ifdef USE_POSIX_TIME
void cfg_tz2timezone(void);
#endif



typedef struct {
  uint32_t fer_centralUnitID, mcu_serialBaud;
#ifdef MDR_TIME
  enum dst geo_dST;
#endif
  int32_t app_rtcAdjust;
  enum receiver app_recv;
  enum transmitter app_transm;
  enum rtclock app_rtc;
  /**
   * \brief each of the nibbles 1-7 stands for a group. nibble 1 == group 1.
   * \note nibble 0 = number of used Groups (stored for the front-end, not used here on the MCU)
   */
  uint32_t fer_usedMembers;
  char app_configPassword[16];
  char app_expertPassword[16];
} config;

extern config C;
extern const bool always_true, always_false;

#if 1//ndef TEST_HOST
#define cfg_getCuId() C.fer_centralUnitID
#endif

enum configAppItem {
  CBA_start = CB_size-1,
  CB_RECV, CB_TRANSM, CB_CUID, CB_USED_MEMBERS, CB_BAUD, CB_GPIO,
  CB_CFG_PASSWD, CB_LONGITUDE, CB_LATITUDE,
#ifndef USE_POSIX_TIME
CB_TIZO,
#else
  CB_TZ,
#endif
#ifdef MDR_TIME
  CB_DST,
#endif
  CB_ASTRO_CORRECTION,
#ifdef USE_NETWORK
  CB_NETWORK_CONNECTION,
#endif
  CB_RFOUT_GPIO, CB_RFIN_GPIO, CB_SETBUTTON_GPIO,

//-----------
  CBA_size
};

enum { CBT_none, CBT_i8, CBT_u8, CBT_i16, CBT_u16, CBT_i32,
  CBT_u32, CBT_i64, CBT_u64, CBT_str, CBT_f, CBT_blob, CBT_end};

constexpr uint8_t cb_types[CBA_size] = {
    CBT_i8 /* CB_VERBOSE */,
   #ifdef USE_WLAN
    CBT_str/* CB_WIFI_SSID */, CBT_str/* CB_WIFI_PASSWD */,
   #endif
   #ifdef USE_MQTT
    CBT_str/* CB_MQTT_URL */, CBT_str/* CB_MQTT_USER */, CBT_str/* CB_MQTT_PASSWD */, CBT_str/* CB_MQTT_CLIENT_ID */, CBT_i8/* CB_MQTT_ENABLE */,
   #endif
   #ifdef USE_HTTP
    CBT_str/* CB_HTTP_USER */, CBT_str/* CB_HTTP_PASSWD */, CBT_i8/* CB_HTTP_ENABLE */,
   #endif
   #ifdef USE_NTP
    CBT_str/* CB_NTP_SERVER */,
   #endif
   #ifdef USE_LAN
    CBT_i8/* CB_LAN_PHY */, CBT_i8/* CB_LAN_PWR_GPIO */,
   #endif
   //-----------
    CBT_i8/* CB_RECV */, CBT_i8/* CB_TRANSM */, CBT_u32/* CB_CUID */, CBT_u32/* CB_USED_MEMBERS */, CBT_u32/* CB_BAUD */, CBT_str/* CB_GPIO */,
     CBT_str/* CB_CFG_PASSWD */, CBT_f/* CB_LONGITUDE */, CBT_f/* CB_LATITUDE */,
     #ifndef USE_POSIX_TIME
     /* CB_TIZO */,
     #else
     CBT_str/* CB_TZ */,
     #endif
     #ifdef MDR_TIME
       /* CB_DST */,
     #endif
       CBT_i8/* CB_ASTRO_CORRECTION */,
     #ifdef USE_NETWORK
       CBT_i8/* CB_NETWORK_CONNECTION */,
     #endif
       CBT_i8/* CB_RFOUT_GPIO */, CBT_i8/* CB_RFIN_GPIO */, CBT_i8/* CB_SETBUTTON_GPIO */,
};


bool config_gpio_setPinMode(unsigned gpio_number, mcu_pin_mode ps, mcu_pin_level pl);

bool config_item_modified(enum configItem item);

const char* config_read_item_s(enum configItem item, char *d, unsigned d_size, const char *def);
uint32_t config_read_item_u32(enum configItem item, uint32_t def);
int8_t config_read_item_i8(enum configItem item, int8_t def);
float config_read_item_f(enum configItem item, float def);

void config_setup_gpio();
struct cfg_gpio* config_read_gpio(struct cfg_gpio *c);
int8_t config_read_rfout_gpio();
int8_t config_read_rfin_gpio();
int8_t config_read_setbutton_gpio();

void config_setup_global();
uint32_t config_read_used_members();
uint32_t config_read_baud();

const char* config_read_tz(char *d, unsigned d_size);
float config_read_timezone();
enum dst config_read_dst();

enum nwConnection config_read_network_connection();

void config_setup_astro();
struct cfg_astro* config_read_astro(struct cfg_astro *c);
float config_read_longitude();
float config_read_latitude();

void config_setup_mqttAppClient();






