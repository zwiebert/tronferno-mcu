/*
 * config_defaults.c
 *
 *  Created on: 16.09.2017
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "config/config.h"
#include "../config_stor.h"

#if !defined NO_SPIFFS || LEGACY_STORAGE_WRITE || LEGACY_STORAGE_READ
#include "esp8266/data_flash.h"
#endif

void  mcu_read_config(u32 mask) {
#if (!defined NO_SPIFFS || LEGACY_STORAGE_WRITE) && !defined TEST_HOST
  read_data(); // FIXME: support old storage in parallel for some time
#endif
#ifndef NO_SPIFFS
  config_read();
#endif

}

void  mcu_save_config(uint32_t mask) {
#ifndef NO_SPIFFS
  config_save();
#endif
#if (!defined NO_SPIFFS || LEGACY_STORAGE_WRITE) && !defined TEST_HOST
  save_data(); // FIXME: support old storage in parallel for some time
#endif
}
