/*
 * kvs_implementation.c
 *
 *  Created on: 24.02.2020
 *      Author: bertw
 */

#include "kvs_wrapper.h"

#include "app_config/proj_app_cfg.h"
#include "misc/int_types.h"

struct kvs_handle {

};

#define VP2H(vp)((vp)->handle)

kvshT kvs_open(const char *name, unsigned flags) {
  return 0;
}

bool kvs_commit(kvshT handle) {
 return false;
}

void kvs_close(kvshT handle) {
}

unsigned kvs_rw_str(kvshT handle, const char *key, void *src_or_dst, unsigned length, bool write) {

  return 0;
}

unsigned kvs_rw_blob(kvshT handle, const char *key, void *src_or_dst, unsigned length, bool write) {
  return 0;
}


#define SET_DT_FUN(DT) \
bool kvs_set_##DT (kvshT handle, const char *key, DT val) { \
  return false; \
} \

#define GET_DT_FUN(DT) \
DT kvs_get_##DT(kvshT handle, const char *key, DT default_val, bool *res_success) { \
  if (res_success) *res_success = false; \
  return 0; \
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



