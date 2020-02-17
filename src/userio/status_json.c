/*
 * status_json.c
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */


#include "user_config.h"
#include "status_json.h"

#include "userio/status_output.h"
#include "cli/cli_imp.h" // FIXME?
#include "misc/int_macros.h"
#include "userio/inout.h"

#include <string.h>
#include <stdlib.h>

char *ftoa(float f, char *buf, int n);

#define D(x)

void (*sj_callback_onClose_ifNotEmpty)(const char *s);

int json_idx;
char *ext_buf;
u16 ext_buf_size;

#define BUF (ext_buf)
#define BUF_SIZE (ext_buf_size)
#define USE_CALLBACK (!ext_buf && sj_callback_onClose_ifNotEmpty)
#define DO_CALLBACK() sj_callback_onClose_ifNotEmpty(BUF);
#define BUF_MAX_SIZE 1024

char *sj_get_json() { return BUF; }

bool sj_realloc_buffer(size_t buf_size) {
  precond(buf_size > json_idx);
  if (buf_size > BUF_MAX_SIZE)
    return false;

  ext_buf = realloc(ext_buf, buf_size);

  if (!ext_buf)
    return false;

  ext_buf_size = buf_size;
  return true;
}

void sj_free_buffer() {
  precond(ext_buf);
  free(ext_buf);
  ext_buf = 0;
  ext_buf_size = 0;
}

bool sj_buffer_grow() {
   size_t new_size = 0;

  if (ext_buf_size == 0) {
    new_size = 128;
  } else {
    new_size = ext_buf_size * 2;
  }
  return sj_realloc_buffer(new_size);
}



static bool sj_not_enough_buffer(const char *key, const char *val) {
    size_t required_size = strlen(key);
      required_size += val ? strlen(val) : 10;
      required_size += 10;

      if (BUF_SIZE < json_idx + required_size)
        sj_buffer_grow();

      if (BUF_SIZE < json_idx + required_size){
        D(ets_printf("json buffer overflow: idx=%u, buf_size=%u\n", json_idx, BUF_SIZE));
        return true;
      }

      return false;
}


bool sj_open_root_object(const char *id) {
  json_idx = 0;
  if (sj_not_enough_buffer(id, 0))
    return false;

  strcat(strcat(strcpy(BUF + json_idx, "{\"from\":\""), id), "\",");
  json_idx += strlen(BUF + json_idx);
  return true;
}

bool sj_add_object(const char *key) {
  precond(json_idx);
  if (sj_not_enough_buffer(key, 0))
    return false;

  strcat(strcat(strcpy(BUF + json_idx, "\""), key), "\":{");
  json_idx += strlen(BUF + json_idx);
  return true;
}


void sj_close_object() {
  precond(json_idx);
  if (BUF[json_idx - 1] == ',') { // remove trailing comma...
    --json_idx;
  }
  strcpy(BUF + json_idx, "},");
  json_idx += strlen(BUF + json_idx);
}

void sj_close_root_object() {
  if (BUF[json_idx - 1] == ',') { // remove trailing comma...
    --json_idx;
  }
  strcpy(BUF + json_idx, "}");
  json_idx = 0;

  if (USE_CALLBACK)
    DO_CALLBACK();

sj_callback_onClose_ifNotEmpty = 0;
}


bool sj_add_key_value_pair_f(const char *key, float val) {
  D(ets_printf("so_json(): %s, %s, %f\n", key, val));
  precond(json_idx > 0);
  precond(key);

  if (sj_not_enough_buffer(key, 0))
    return false;

  char buf[20];
  ftoa(val, buf, 5);
  strcat(strcat(strcat(strcat(strcpy(BUF + json_idx, "\""), key), "\":"), buf), ",");

  json_idx += strlen(BUF + json_idx);
  D(ets_printf("json_idx: %u, buf: %s\n", json_idx, BUF));
  return true;
}


bool sj_add_key_value_pair_d(const char *key, int val) {
  D(ets_printf("so_json(): %s, %d\n", key, val));
  precond(json_idx > 0);
  precond(key);

  if (sj_not_enough_buffer(key, 0))
    return false;

  char buf[20];
  ltoa(val, buf, 10);
  strcat(strcat(strcat(strcat(strcpy(BUF + json_idx, "\""), key), "\":"), buf), ",");

  json_idx += strlen(BUF + json_idx);
  D(ets_printf("json_idx: %u, buf: %s\n", json_idx, BUF));
  return true;
}

bool sj_add_key_value_pair_s(const char *key, const char *val) {
  D(ets_printf("so_json(): %s, %s, %d\n", key, val, is_number));
  precond(json_idx > 0);
  precond(key);

  if (sj_not_enough_buffer(key, val))
    return false;

  strcat(strcat(strcat(strcat(strcpy(BUF + json_idx, "\""), key), "\":\""), val), "\",");

  json_idx += strlen(BUF + json_idx);
  D(ets_printf("json_idx: %u, buf: %s\n", json_idx, BUF));
  return true;
}

