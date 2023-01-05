/*
 * store.c
 *
 *  Created on: 12.06.2019
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#include "app_misc/kvstore.h"

bool kvs_store_string(const char *key, const char *val) {
  return false;
}

bool kvs_get_string(const char *key, char *buf, size_t buf_size) {
  return false;
}

bool kvs_store_int32(const char *key, int32_t val) {
  return false;
}

bool kvs_get_int32(const char *key, int32_t *dest) {
  return false;
}

bool kvs_erase_key(const char *key) {
  return false;
}
