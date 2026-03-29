/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */

#include "so_out.h"
#include "so_print.h"
#include "fernotron_uout/fer_uo_publish.h"
#include "stdint.h"
#include "app_misc/firmware.h"
#include "app_misc/rtc.h"
#include "../app_misc/include/app_misc/kvstore.hh"
#include "app_settings/config.h"
#include "net/ipnet.h"
#include "uout/uout_builder_json.hh"
#include "cli/cli_out.hh"
#include "app_uout/status_output.h"
#include <unistd.h>
#include <app_uout/so_msg.h>
//#include "utils_misc/int_macros.h"

#include <string.h>
#include <utils_misc/cstring_utils.hh>
#include <stdio.h>
#include <time.h>

#ifdef CONFIG_TF_DEBUG
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "tf.uout"


void soMsg_fw_start_msg_print(class UoutWriter &td) {
  static const char msg[] =
      "\n\n" "tf: info: start: tronferno-mcu\n" "tf: info: build-date: " __DATE__ " " __TIME__ "\n"
      "tf: hint: type 'help;' to get a command list\n";

  td.write(msg, sizeof msg -1);
}

void soMsg_status_ok(class UoutWriter &td) {
  td.so().print("status", "ok");
}

void soMsg_status_error(class UoutWriter &td) {
  td.so().print("status", "error");
}

void soMsg_mcu_begin(class UoutWriter &td) {
  td.so().x_open("mcu");
}

void soMsg_mcu_run_time(class UoutWriter &td) {
  td.so().print("run-time", run_time_s());

}
#ifdef MCU_ESP32
#include "esp_app_desc.h"
void soMsg_mcu_version(class UoutWriter &td) {
  char buf[64];
  const esp_app_desc_t *ad = esp_app_get_description();

  td.so().print("chip", MCU_TYPE);
  td.so().print("firmware", (csu_copy_cat(buf, ad->project_name, "-", ad->version), buf));

  csu_copy_cat(buf, ad->date, "T", ad->time);
  char *p = buf;
  do
    if (*p == ' ')
      *p = '-';
  while (*++p);
  td.so().print("build-time", buf);
}


#else // not esp32
void soMsg_mcu_version(class UoutWriter &td) {
  char buf[64];

  td.so().print("chip", MCU_TYPE);
  //td.so().print("firmware", (csu_copy_cat(buf, "tronferno-mcu-", APP_VERSION), buf));

  char *p = STRCPY(buf, (__DATE__ "T" __TIME__));
  do
    if (*p == ' ')
      *p = '-';
  while (*++p);
  td.so().print("build-time", buf);
}
#endif

void soMsg_mcu_ota(class UoutWriter &td, const char *url) {
#ifdef CONFIG_APP_USE_OTA
  td.so().print("ota-url", url);
#endif
}
void soMsg_mcu_ota_state(class UoutWriter &td) {
#ifdef CONFIG_APP_USE_OTA
  td.so().print("ota-state", ota_getState());
#endif
}

void soMsg_mcu_boot_count(class UoutWriter &td) {
#ifndef TEST_HOST
  extern int32_t boot_counter;
  td.so().print("boot-count", boot_counter);
#else
  td.so().print("boot-count", 0);
#endif
}

void soMsg_mcu_end(class UoutWriter &td) {
  td.so().x_close();
}

/////////////////////////////////////////////////////////////////////////////////
void soMsg_cfgpasswd_ok(class UoutWriter &td) {
  // td.write("password ok\n");
}
void soMsg_cfgpasswd_wrong(class UoutWriter &td) {
  td.write("wrong config password\n");
}

void soMsg_cfgpasswd_missing(class UoutWriter &td) {
  td.write("missing config password\n");
}


/////////////////////////////////////////////////////////////////////////////////

void soMsg_shpref_obj_begin(class UoutWriter &td) {
  td.so().x_open("shs");
}

void soMsg_shpref_obj_end(class UoutWriter &td) {
  td.so().x_close();
}

void soMsg_shpref_obj_gm_begin(class UoutWriter &td, const so_arg_gm_t a) {
  char buf[] = "shs00";
  buf[3] += a.g;
  buf[4] += a.m;
  td.so().x_open(buf);

}

void soMsg_shpref_obj_gm_end(class UoutWriter &td) {
  td.so().x_close();
}

void soMsg_KVS_begin(class UoutWriter &td) {
  td.so().x_open("kvs");
}

void soMsg_KVS_end(class UoutWriter &td) {
  td.so().x_close();
}

bool soMsg_KVS_print(class UoutWriter &td, const char *key) {
  char buf[64];
#ifndef TEST_HOST
  if (kvs_get_string(key, buf, sizeof buf)) {
    td.so().print(key, buf);
    return true;
  }
#endif
  return false;
}

void soMsg_print_kvd(class UoutWriter &td, const so_arg_kvd_t a) {
  td.so().print(a.key, a.val);
}

void soMsg_print_kvs(class UoutWriter &td, so_arg_kvs_t a) {
  td.so().print(a.key, a.val);
}

void soMsg_kv(class UoutWriter &td, const char *key, const char *val) {
  td.so().print(key, val);
}

void soMsg_kv(class UoutWriter &td, const char *key, int val) {
  td.so().print(key, val);
}

void soMsg_kv(class UoutWriter &td, const char *key, bool val);


#ifdef CONFIG_APP_USE_NETWORK
void soMsg_inet_print_address(class UoutWriter &td) {
  char buf[20];
  ipnet_addr_as_string(buf, 20);
  td.so().print("ipaddr", buf);
}

#endif

void soMsg_gpio_pin(class UoutWriter &td, const so_arg_pch_t a, bool broadcast) {

  char buf[64];
  snprintf(buf, sizeof buf, "gpio%d", (int) a.gpio_num);
  td.so().print(buf, (int)a.level_int);

  if (broadcast)
    uoCb_publish_pinChange(a);
}

void soMsg_sep_obj_begin(class UoutWriter &td) {
  td.so().x_open("sep");
}

void soMsg_sep_obj_end(class UoutWriter &td) {
  td.so().x_close();
}
