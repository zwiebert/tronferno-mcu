/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "so_out.h"
#include <app_config/options.hh>
#if 0
#include "cli_app/cli_app.h" // FIXME?
#include "cli_app/cli_config.h"
#include "app/opt_map.hh"
#endif
#include "txtio/inout.h"
#include "uout/status_json.h"
#include "debug/dbg.h"
#include "misc/int_macros.h"
#include "misc/ftoa.h"
#include "misc/itoa.h"
#include <app/opt_map.hh>
#include <string.h>
#include <stdio.h>

#define D(x)

u8 so_target;

// get the SO_MSG_T for a config key string (or SO_NONE)
SO_MSG_T so_parse_config_key(const char *k) {
  otok kt = optMap_findToken(k);
  if (kt == otok::NONE) {
    return SO_NONE;
  }

  SO_MSG_T result = static_cast<SO_MSG_T>(SO_CFG_begin + 1 + static_cast<otokBaseT>(kt));

  return result;
}

// get config key string for a so_msgt_t (or NULL)
static const char *
gk(SO_MSG_T so_key) {
  if (SO_CFG_begin < so_key && so_key < SO_CFG_end)
    return otok_strings[so_key - (SO_CFG_begin + 1)];
  return nullptr;
}

void so_out_x_open(const char *name) {
  if (so_cco) cli_out_set_x(name);
  if (so_jco) {
    sj_add_object(name);
  }
}

void so_out_x_close() {
  if (so_cco) cli_out_close();
  if (so_jco) {
    sj_close_object();

  }
}

// provide a version of this cli-function without the third parameter
static void cli_out_x_reply_entry2(const char *key, const char *val) {
  cli_out_x_reply_entry(key, val, 0);
}

void so_out_x_reply_entry(SO_MSG_T key, const char *val) {
  if (so_cco) cli_out_x_reply_entry2(gk(key), val);
}

void so_out_x_reply_entry_s(SO_MSG_T key, const char *val) {
  if (so_cco) cli_out_x_reply_entry2(gk(key), val);
  if (so_jco) sj_add_key_value_pair_s(gk(key), val);
}

void so_out_x_reply_entry_ss(const char *key, const char *val) {
  if (so_cco) cli_out_x_reply_entry2(key, val);
  if (so_jco) sj_add_key_value_pair_s(key, val);
}
void so_out_x_reply_entry_sd(const char *key, int val) {
  char buf[20];
  itoa(val, buf, 10);
  if (so_cco) cli_out_x_reply_entry2(key, buf);
  if (so_jco) sj_add_key_value_pair_d(key, val);
}

void so_out_x_reply_entry_sl(const char *key, int val) {
  char buf[20];
  ltoa(val, buf, 10);
  if (so_cco) cli_out_x_reply_entry2(key, buf);
  if (so_jco) sj_add_key_value_pair_d(key, val);
}

void so_out_x_reply_entry_d(SO_MSG_T key, int val) {
  so_out_x_reply_entry_sd(gk(key), val);
}

void so_out_x_reply_entry_l(SO_MSG_T key, int val) {
  so_out_x_reply_entry_sl(gk(key), val);
}

void so_out_x_reply_entry_lx(SO_MSG_T key, int val) {
  char buf[20];
  ltoa(val, buf, 16);
  if (so_cco) cli_out_x_reply_entry2(gk(key), buf);
  if (so_jco) sj_add_key_value_pair_s(gk(key), buf); //no hex in json. use string
}

void so_out_x_reply_entry_f(SO_MSG_T key, float val, int n) {
  char buf[20];
  ftoa(val, buf, 5);
  if (so_cco) cli_out_x_reply_entry2(gk(key), buf);
  if (so_jco) sj_add_key_value_pair_f(gk(key), val);
}
