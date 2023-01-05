/*
 * config_kvs.h
 *
 *  Created on: 21.04.2020
 *      Author: bertw
 */

#pragma once

#include "app_config/proj_app_cfg.h"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"

template<typename handle_type, typename value_type, typename cb_type>
void kvsRead_i8(handle_type handle, cb_type cb, value_type &value) {
  value = static_cast<value_type>(kvs_get_i8(handle, settings_get_kvsKey(cb), static_cast<int8_t>(value), 0));
}

template<typename handle_type, typename value_type, typename cb_type>
void kvsRead_u32(handle_type handle, cb_type cb, value_type &value) {
  value = static_cast<value_type>(kvs_get_u32(handle, settings_get_kvsKey(cb), static_cast<uint32_t>(value), 0));
}

template<typename handle_type, typename value_type, typename cb_type>
void kvsRead_blob(handle_type h, cb_type cb, value_type &val) {
  kvs_rw_blob(h, settings_get_kvsKey(cb), &val,sizeof (value_type), false);
}

template<typename handle_type, typename value_type, typename cb_type>
void kvsRead_charArray(handle_type h, cb_type cb,  value_type val) {
  kvs_rw_str(h, settings_get_kvsKey(cb), val, sizeof (value_type), false);
}

template<typename handle_type, typename value_type, typename cb_type>
bool kvsWrite_blob(handle_type h, cb_type cb, value_type &val) {
  return kvs_rw_blob(h, settings_get_kvsKey(cb), &val, sizeof (value_type), true) == sizeof (value_type);
}
