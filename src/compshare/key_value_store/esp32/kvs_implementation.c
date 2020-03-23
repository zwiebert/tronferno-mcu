/*
 * kvs_implementation.c
 *
 *  Created on: 24.02.2020
 *      Author: bertw
 */

#include "kvs_wrapper.h"

#include "app_config/proj_app_cfg.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#define CFG_PARTNAME "nvs"

struct kvs_handle {
nvs_handle handle; // XXX allow more than one handle
};

#define VP2H(vp)((vp)->handle)

kvshT kvs_open(const char *name, unsigned flags) {
  static struct kvs_handle kvs_handle;
  int nvs_flag = (flags & kvs_WRITE) ? NVS_READWRITE : NVS_READONLY;

  esp_err_t err  = nvs_open(name, nvs_flag, &kvs_handle.handle);
  if (err == ESP_OK)  {
    return &kvs_handle;
  }

  return 0;
}

bool kvs_commit(kvshT handle) {
  return nvs_commit(VP2H(handle)) == ESP_OK;
}

void kvs_close(kvshT handle) {
  nvs_close(VP2H(handle));
  handle = 0;
}

bool kvs_erase_key(kvshT handle, const char *key) {
  return ESP_OK == nvs_erase_key(VP2H(handle), key);
}

size_t kvs_rw_str(kvshT handle, const char *key, void *src_or_dst, unsigned length, bool write) {
  if (write) {
    if (nvs_set_str(VP2H(handle), key, src_or_dst) == ESP_OK)
      return 1;
  } else {
    size_t dst_len = length;
    if (nvs_get_str(VP2H(handle), key, src_or_dst, &dst_len) == ESP_OK) {
      return (int)dst_len;
    }
  }
  return 0;
}

size_t kvs_rw_blob(kvshT handle, const char *key, void *src_or_dst, unsigned length, bool write) {
  if (write) {
    if (nvs_set_blob(VP2H(handle), key, src_or_dst, length) == ESP_OK)
      return 1;
  } else {
    size_t dst_len = length;
    if (nvs_get_blob(VP2H(handle), key, src_or_dst, &dst_len) == ESP_OK) {
      return (int)dst_len;
    }
  }
  return 0;
}


#define SET_DT_FUN(DT) \
bool kvs_set_##DT (kvshT handle, const char *key, DT val) { \
  return nvs_set_##DT(VP2H(handle), key, val) == ESP_OK; \
} \

#define GET_DT_FUN(DT) \
DT kvs_get_##DT(kvshT handle, const char *key, DT default_val, bool *res_success) { \
  DT temp; \
  if (ESP_OK == nvs_get_##DT(VP2H(handle), key, &temp)) { \
    if (res_success)*res_success = true; \
    return temp; \
  } \
  if (res_success) *res_success = false; \
  return default_val; \
} \

#define SET_GET_DT_FUN(dt) SET_DT_FUN(dt) GET_DT_FUN(dt);


SET_GET_DT_FUN(i8);
SET_GET_DT_FUN(u8);
SET_GET_DT_FUN(i16);
SET_GET_DT_FUN(u16);
SET_GET_DT_FUN(i32);
SET_GET_DT_FUN(u32);
SET_GET_DT_FUN(i64);
SET_GET_DT_FUN(u64);


int kvs_foreach(const char *name_space, kvs_type_t type, const char *key_match, kvs_foreach_cbT cb) {
  nvs_iterator_t it = nvs_entry_find(CFG_PARTNAME, name_space, type);
  int count = 0;
  while (it != NULL) {
    nvs_entry_info_t info;
    nvs_entry_info(it, &info);
    it = nvs_entry_next(it);

    if (key_match) {
      unsigned len = strlen(key_match);
      if (strlen(info.key) < len)
        continue;
      if (0 != strncmp(info.key, key_match, len))
        continue;
    }

    if (cb) {
      switch (cb(info.key, info.type)) {
      case kvsCb_match:
        ++count;
        break;
      case kvsCb_done:
        ++count;
        return count;
      case kvsCb_noMatch:
        break;
      }
    } else {
      ++count;
    }
  }
  return count;
}

void kvs_setup(void) {
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK( err );
}
