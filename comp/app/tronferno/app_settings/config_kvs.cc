/*
 * config.c
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */


#include "fernotron/fer_main.h"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include "config_kvs.h"
#include <app_settings/config.h>


#include "utils_misc/int_macros.h"
#include "key_value_store/kvs_wrapper.h"
#include "utils_misc/int_types.h"
#include "utils_misc/stof.h"

#ifdef CONFIG_TF_DEBUG
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "tf"

#define CFG_KEY "global.C"

bool config_item_modified(enum configItem item) {
  return config_item_modified((configAppItem)item);
}
bool config_item_modified(enum configAppItem item) {
  kvshT h;
  bool ferCfg_isModified = false;
  if ((h = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ))) {
    switch ((int)item) {
    case CB_CUID:
    case CB_USED_MEMBERS:
      ferCfg_isModified = true;
      break;
    case CB_BAUD:
      kvsRead_i8(h, item, C.mcu_serialBaud);
      break;
#ifdef CONFIG_APP_USE_MDR_TIME
    case CB_DST:
      kvsRead_u32(h, item, C.geo_dST);
      break;
#endif
    case CB_VERBOSE:
      config_ext_setup_txtio();
      break;

    default:
      break;
    }
    kvs_close(h);
  }
  if (ferCfg_isModified)
     fer_main_setup({ config_read_item(CB_CUID, (uint32_t)CONFIG_APP_FER_CENTRAL_UNIT_ID), config_read_item(CB_USED_MEMBERS, (uint32_t) CONFIG_APP_FER_GM_USE)}, true);

  return true;
}

#ifdef CONFIG_APP_USE_GPIO_PINS
bool config_gpio_setPinMode(unsigned gpio_number, mcu_pin_mode ps, mcu_pin_level pl) {
  bool result = false;
  kvshT h;
  if ((h = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ_WRITE))) {
    struct cfg_gpio c = {};
    kvsRead_blob(h, CB_GPIO, c.gpio);
    gpioCfg_setPin(&c, gpio_number, ps, pl);
    result = kvsWrite_blob(h, CB_GPIO, c.gpio);
    kvs_commit(h);
    kvs_close(h);
  }
  return result;
}
#endif
