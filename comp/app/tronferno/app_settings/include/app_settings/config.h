/**
 * \file    app_settings/config.h
 * \brief   Application settings
 */

#pragma once


#include "config_kvs/config.h"
#include "txtio/inout.h"
#include "fernotron/fer_pct.h"
#include "fernotron_trx/astro.h"
//#include "net/ethernet_setup.hh"
#include "gpio/pin.h"
#include <net/network_connection.hh>
#include "stdbool.h"


enum tfmcu_errorT {
  TFMCU_ERR_CC1101_RFIN_NOT_CONNECTED,
};
extern unsigned tfmcu_error_mask;
void tfmcu_put_error(tfmcu_errorT error_code, bool value);


extern bool app_safe_mode;
int app_safeMode_increment(bool reset = false);

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


#ifdef CONFIG_APP_USE_POSIX_TIME
void cfg_tz2timezone(void);
#endif



typedef struct {
  uint32_t mcu_serialBaud;
#ifdef CONFIG_APP_USE_MDR_TIME
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
//  uint32_t fer_usedMembers;
  char app_configPassword[16];
} config;

extern config C;
extern const bool always_true, always_false;

bool config_gpio_setPinMode(unsigned gpio_number, mcu_pin_mode ps, mcu_pin_level pl);

void config_setup_gpio();
struct cfg_gpio* config_read_gpio(struct cfg_gpio *c);
int8_t config_read_rfout_gpio();
int8_t config_read_rfin_gpio();
int8_t config_read_setbutton_gpio();
int8_t config_read_rf_trx();
int8_t config_read_rfsck_gpio();
int8_t config_read_rfmosi_gpio();
int8_t config_read_rfmiso_gpio();
int8_t config_read_rfss_gpio();
const char* config_read_rf_repeater(char *d, unsigned d_size);
int8_t config_read_rtc_auto_upd();

void config_setup_global();
uint32_t config_read_used_members();
uint32_t config_read_baud();

const char* config_read_tz(char *d, unsigned d_size);
float config_read_timezone();
enum dst config_read_dst();

enum nwConnection config_read_network_connection();

void config_setup_astro();
struct cfg_astro* config_read_astro(struct cfg_astro *c);
enum astroCorrection config_read_astro_correction();
float config_read_longitude();
float config_read_latitude();
void config_setup_cc1101();
void config_setup_repeater();
struct cc1101_settings* config_read_cc1101(struct cc1101_settings *c);
const char *config_read_cc1101_config(char *buf, size_t len);
void config_ext_setup_txtio();
void config_ext_setup_cliTcpServer();

void config_setup_mqttAppClient();
