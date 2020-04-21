#include "config.h"
#include "config_defaults.h"
#include "app/fernotron.h"
#include "misc/int_types.h"
#include <ctype.h>
#include <stdlib.h>

const bool always_true = true;
const bool always_false;

config C = {
  .fer_centralUnitID = MY_FER_CENTRAL_UNIT_ID,
  .mcu_serialBaud = MY_MCU_UART_BAUD_RATE,
  0, // app_rtcAdjust
  recvTick, // recv
  transmTick,// transm
  rtcAvrTime,//rtc
  vrbNone,  //verboseOutput
  .fer_usedMembers = MY_FER_GM_USE,

  .app_configPassword = MY_APP_CONFIG_PASSWORD,
  .app_expertPassword = MY_APP_EXPERT_PASSWORD,
#ifdef ACCESS_GPIO
  .gpio =  { 0, },
#endif

#ifdef USE_NETWORK
  .network = MY_NETWORK_CONNECTION,
#endif


};

#ifdef POSIX_TIME
double tz2offset(const char *tz) {
  double offset = 0.0;

  for (const char *p=tz; *p; ++p) {
    if (isalpha(*p))
      continue;
    offset = strtod(p, 0) * -1;
    return offset;
  }
  return 0;
}

#endif


void read_config(u32 mask) {
#ifdef USE_CONFIG_KVS
  config_read_kvs(mask);
#else
  mcu_read_config(mask);
#endif
}

void save_config(u32 mask) {
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
  save_config(CM_ALL);
}

void read_config_all() {
  read_config(CM_ALL);
}
