/**
 * \file     app_misc/kvs_store.hh
 * \brief    store some key/value pairs hassle free (without open/close/commit/...)
 */

#pragma once

#include <stddef.h>
#include "utils_misc/int_types.h"
#include "key_value_store/kvs_wrapper.h"

bool kvs_store_string(const char *key, const char *val);
bool kvs_get_string(const char *key, char *buf, size_t buf_size);
//int kvs_foreach_string(const char *key_match, void (*cb)(const char *key, void *args), void *args);

bool kvs_store_int32(const char *key, int32_t val);
bool kvs_get_int32(const char *key, int32_t *dest);
int kvs_foreach_int32(const char *key_match, void (*cb)(const char *key, void *args), void *args);

bool kvs_erase_key(const char *key);

template<typename cmp_fun_type, typename cmp_arg_type, typename cb_fun_type, typename cb_arg_type>
int kvs_foreach_string(cmp_fun_type cmp, cmp_arg_type &&cmp_arg, cb_fun_type cb, cb_arg_type &&cb_arg) {
  return kvs_foreach("kvs", KVS_TYPE_STR, cmp, cmp_arg, cb, cb_arg);
}

