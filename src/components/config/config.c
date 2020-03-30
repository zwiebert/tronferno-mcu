#include "config.h"
#include "config_defaults.h"
#include "app/fernotron.h"
#include "misc/int_types.h"
#include <ctype.h>

config C = {
  .fer_centralUnitID = MY_FER_CENTRAL_UNIT_ID,
  .mcu_serialBaud = MY_MCU_UART_BAUD_RATE,
  .geo_longitude = MY_GEO_LONGITUDE,
  .geo_latitude = MY_GEO_LATITUDE,
  .geo_timezone = MY_GEO_TIMEZONE,
  0, // app_rtcAdjust
  recvTick, // recv
  transmTick,// transm
  rtcAvrTime,//rtc
  vrbNone,  //verboseOutput
  .fer_usedMembers = MY_FER_GM_USE,
#ifdef USE_WLAN
  .wifi_SSID = MY_WIFI_SSID,
  .wifi_password = MY_WIFI_PASSWORD,
#endif
  .app_configPassword = MY_APP_CONFIG_PASSWORD,
  .app_expertPassword = MY_APP_EXPERT_PASSWORD,
#ifdef ACCESS_GPIO
  .gpio =  { 0, },
#endif
#ifdef POSIX_TIME
  .geo_tz = MY_GEO_TZ,
#endif
#ifdef USE_MQTT
  .mqtt_url = MY_MQTT_URL,
  .mqtt_user = MY_MQTT_USER,
  .mqtt_password = MY_MQTT_PASSWORD,
  MY_MQTT_ENABLE,
#endif
#ifdef USE_HTTP
  .http_user = MY_HTTP_USER,
  .http_password = MY_HTTP_PASSWORD,
  .http_enable = MY_HTTP_ENABLE,
#endif
#ifdef USE_NTP
  .ntp_server = MY_NTP_SERVER,
#endif
#ifdef USE_NETWORK
  .network = MY_NETWORK_CONNECTION,
#endif
#ifdef USE_LAN
  .lan_phy = MY_LAN_PHY,
  .lan_pwr_gpio = MY_LAN_PWR_GPIO,
#endif
  .astroCorrection = acAverage,
};

#ifdef POSIX_TIME
static double tz2offset(const char *tz) {
  double offset = 0.0;

  for (const char *p=tz; *p; ++p) {
    if (isalpha(*p))
      continue;
    offset = strtod(p, 0) * -1;
    return offset;
  }
  return 0;
}

void cfg_tz2timezone(void) {
  C.geo_timezone = tz2offset(C.geo_tz);
}
#endif


void read_config(u32 mask) {
#ifdef USE_CONFIG_KVS
  config_read_kvs(mask);
#else
  mcu_read_config(mask);
#endif
  fer_copyConfig();
}

void save_config(u32 mask) {
  fer_copyConfig();
#ifdef USE_CONFIG_KVS
  config_save_kvs(mask);
#else
  mcu_save_config(mask);
#endif

}


void save_config_item(enum configItem item) {
  save_config(1UL << item);
}

void read_config_item(enum configItem item) {
  read_config(1UL << item);
}

void save_config_all() {
  save_config(~0UL);
}

void read_config_all() {
  read_config(~0UL);
}
