/*
 * config.c
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#include "fernotron/fer_main.h"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
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

bool config_item_modified(enum configAppItem item) {
  kvshT h;
  bool ferCfg_isModified = false;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    switch ((int)item) {
    case CB_CUID:
    case CB_USED_MEMBERS:
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
    case CB_VERBOSE:
      config_setup_txtio();
      break;

    default:
      break;
    }
    kvs_close(h);
  }
  if (ferCfg_isModified)
     fer_main_setup({ config_read_item(CB_CUID, MY_FER_CENTRAL_UNIT_ID), config_read_item(CB_USED_MEMBERS, MY_FER_GM_USE)}, true);

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
