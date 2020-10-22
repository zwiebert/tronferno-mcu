/*
 * config.c
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#include "fernotron/fer_main.h"
#include "app_settings/config.h"
#include "config_kvs.h"
#include <app_settings/config.h>
#include "app_settings/config_defaults.h"

#include "utils_misc/int_macros.h"
#include "key_value_store/kvs_wrapper.h"
#include "utils_misc/int_types.h"
#include "utils_misc/stof.h"
#include <stdbool.h>

#define D(x) 

#define CFG_KEY "global.C"
#ifdef CONFIG_DICT
extern const char * const config_keys[];
const char * const config_keys[] = {
  "C_RECEIVER", "C_TRANSM", "C_CUID", "C_GMU", "C_BAUD", "C_GPIO",
  "C_CFG_PASSWD", "C_LONGITUDE", "C_LATITUDE",
#ifndef USE_POSIX_TIME
"C_TIZO",
#else
  "C_TZ",
#endif
#ifdef MDR_TIME
  "C_DST",
#endif
  "C_AST_COR",
#ifdef USE_NETWORK
  "C_NW_CONN",
#endif
  "C_RFOUTP", "C_RFINP", "C_SETBTNP",
};

bool config_item_modified(enum configItem item) {
  kvshT h;
  bool ferCfg_isModified = false;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    switch ((int)item) {
    case CB_CUID:
      kvsR(u32, item, C.fer_centralUnitID);
      ferCfg_isModified = true;
      break;
    case CB_BAUD:
      kvsR(i8, item, C.mcu_serialBaud);
      break;
#ifdef MDR_TIME
    case CB_DST:
      kvsR(u32, item, C.geo_dST);
      break;
#endif
    case CB_USED_MEMBERS:
      ferCfg_isModified = true;
      kvsR(u32, item, C.fer_usedMembers);
      break;
    case CB_VERBOSE:
      config_setup_txtio();
      break;

    default:
      break;
    }
    kvs_close(h);
  }
  if (ferCfg_isModified)
     fer_main_setup({C.fer_centralUnitID,  C.fer_usedMembers}, true);

  return true;
}
#ifdef ACCESS_GPIO
bool config_gpio_setPinMode(unsigned gpio_number, mcu_pin_mode ps, mcu_pin_level pl) {
  bool result = false;
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ_WRITE))) {
    struct cfg_gpio c = {};
    kvsRb(CB_GPIO, c.gpio);
    gpioCfg_setPin(&c, gpio_number, ps, pl);
    result = (kvsWb(CB_GPIO, c.gpio) == sizeof c.gpio);
    kvs_commit(h);
    kvs_close(h);
  }
  return result;
}
#endif

#endif
