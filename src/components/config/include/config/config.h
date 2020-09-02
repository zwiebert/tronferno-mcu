/*
 * config.h
 *
 * Created: 24.08.2017 20:56:00
 *  Author: bertw
 */

#pragma once

#include "app_config/proj_app_cfg.h"
#include "config_kvs/config.h"
#include "txtio/inout.h"
#include "fernotron/types.h"
#include "fernotron/astro.h"
#include "net/ethernet.h"
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


#ifdef POSIX_TIME
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
  uint32_t fer_usedMembers; // each of the nibbles 1-7 stands for a group. nibble 1 == group 1. nibble 0 = number of used Groups (stored for the front-end, not used here on the MCU)
  gm_bitmask_t fer_usedMemberMask;
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
#ifndef POSIX_TIME
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
enum astroCorrection config_read_astro_correction();

void config_setup_mqttAppClient();






