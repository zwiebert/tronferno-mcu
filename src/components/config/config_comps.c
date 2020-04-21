/*
 * config_comps.c
 *
 *  Created on: 21.04.2020
 *      Author: bertw
 */

#include "config/config.h"
#include "config_defaults.h"
#include "config_kvs.h"

#include "misc/int_macros.h"
#include "key_value_store/kvs_wrapper.h"
#include "misc/int_types.h"

double tz2offset(const char *tz);

#ifdef USE_LAN
#include "net/ethernet.h"
void config_setup_ethernet() {
  static struct cfg_lan *c;
  kvshT h;

  if (c || (c = malloc(sizeof *c))) {
    *c = (struct cfg_lan ) { .phy = MY_LAN_PHY, .pwr_gpio = MY_LAN_PWR_GPIO, };

    if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
      kvsR(i8, CB_LAN_PHY, c->phy);
      kvsR(i8, CB_LAN_PWR_GPIO, c->pwr_gpio);
      kvs_close(h);
    }
    ethernet_setup(c);
    //free(c);
  }
}
#endif

#ifdef USE_NTP
#include "net/ntp.h"
void config_setup_ntpClient() {
  static struct cfg_ntp *c;
  kvshT h;

  if (c || (c = malloc(sizeof *c))) {
    *c = (struct cfg_ntp ) { .server = MY_NTP_SERVER };

    if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
      kvsRs(CB_NTP_SERVER, c->server);
      kvs_close(h);
    }
    ntp_setup(c);
    //free(c);
  }
}
#endif

#ifdef USE_WLAN
#include "net/wifistation.h"
void config_setup_wifiStation() {
  static struct cfg_wlan *c;
  kvshT h;

  if (c || (c = malloc(sizeof *c))) {
    *c = (struct cfg_wlan ) { .SSID = MY_WIFI_SSID, .password = MY_WIFI_PASSWORD, };
    if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
      kvsRs(CB_WIFI_SSID, c->SSID);
      kvsRs(CB_WIFI_PASSWD, c->password);
      kvs_close(h);
    }
    wifistation_setup(c);
    //free(c);
  }
}
#endif

#if 1
#include "net/mqtt/app/mqtt.h"
void config_setup_astro() {
  static struct cfg_astro *c;
  kvshT h;

  if (c || (c = malloc(sizeof *c))) {
    *c = (struct cfg_astro ) { .geo_longitude = MY_GEO_LONGITUDE, .geo_latitude = MY_GEO_LATITUDE, .astroCorrection = acAverage,
#ifndef POSIX_TIME
      .geo_timezone = MY_GEO_TIMEZONE,
#endif
        };
    if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
      kvsRb(CB_LONGITUDE, c->geo_longitude);
      kvsRb(CB_LATITUDE, c->geo_latitude);
      kvsR(i8, CB_ASTRO_CORRECTION, c->astroCorrection);
#ifndef POSIX_TIME
      kvsRb(CB_TIZO, c->geo_timezone);
#else
      char tz[64] = MY_GEO_TZ;
      kvsRs(CB_TZ, tz);
      c->geo_timezone = tz2offset(tz);
#endif
      kvs_close(h);
    }
    astro_init_and_reinit(c);
    //free(c);
  }
}
#endif

#ifdef USE_MQTT
#include "net/mqtt/app/mqtt.h"
void config_setup_mqttClient() {
  static struct cfg_mqtt *c;
  kvshT h;

  if (c || (c = malloc(sizeof *c))) {
    *c =
        (struct cfg_mqtt ) { .url = MY_MQTT_URL, .user = MY_MQTT_USER, .password = MY_MQTT_PASSWORD, .client_id = MY_MQTT_CLIENT_ID, .enable = MY_MQTT_ENABLE };
    if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
      kvsRs(CB_MQTT_URL, c->url);
      kvsRs(CB_MQTT_USER, c->user);
      kvsRs(CB_MQTT_PASSWD, c->password);
      kvsRs(CB_MQTT_CLIENT_ID, c->client_id);
      kvsR(i8, CB_MQTT_ENABLE, c->enable);
      kvs_close(h);
    }
    io_mqttApp_setup(c);
    //free(c);
  }
}
#endif

#ifdef USE_HTTP
#include "net/http/server/http_server.h"
void config_setup_httpServer() {
  static struct cfg_http *c;
  kvshT h;

  if (c || (c = malloc(sizeof *c))) {
    *c = (struct cfg_http ) { .user = MY_HTTP_USER, .password = MY_HTTP_PASSWORD, .enable = MY_HTTP_ENABLE };
    if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
      kvsRs(CB_HTTP_USER, c->user);
      kvsRs(CB_HTTP_PASSWD, c->password);
      kvsR(i8, CB_HTTP_ENABLE, c->enable);

      kvs_close(h);
    }
    hts_setup(c);
    //free(c);
  }
}
#endif
