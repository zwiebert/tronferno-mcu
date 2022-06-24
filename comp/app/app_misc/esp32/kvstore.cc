/*
 * store.c
 *
 *  Created on: 12.06.2019
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_err.h"

#include "app_misc/kvstore.h"

bool kvs_store_string(const char *key, const char *val) {
  esp_err_t err = 0;
  nvs_handle handle;

  if ((err = nvs_open("kvs", NVS_READWRITE, &handle)) == ESP_OK) {
    nvs_set_str(handle, key, val);
    nvs_commit(handle);
    nvs_close(handle);
  }
  return err == ESP_OK;
}

bool kvs_get_string(const char *key, char *buf, size_t buf_size) {
  esp_err_t err = 0;
  nvs_handle handle;
  size_t len;

  if ((err = nvs_open("kvs", NVS_READONLY, &handle)) == ESP_OK) {
    (len = buf_size), (err = nvs_get_str(handle, key, buf, &len));
    nvs_close(handle);


  }
  return err == ESP_OK;
}

bool kvs_store_int32(const char *key, i32 val) {
  esp_err_t err = 0;
  nvs_handle handle;

  if ((err = nvs_open("kvs", NVS_READWRITE, &handle)) == ESP_OK) {
    nvs_set_i32(handle, key, val);
    nvs_commit(handle);
    nvs_close(handle);
  }
  return err == ESP_OK;
}

bool kvs_get_int32(const char *key, i32 *dest) {
  esp_err_t err = 0;
  nvs_handle handle;

  if ((err = nvs_open("kvs", NVS_READONLY, &handle)) == ESP_OK) {
    nvs_get_i32(handle, key, dest);
    nvs_close(handle);


  }
  return err == ESP_OK;
}
