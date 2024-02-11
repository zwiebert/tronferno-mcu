/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */

#include "so_out.h"
#include "so_print.h"
#include "fernotron_uout/fer_uo_publish.h"
#include "utils_misc/int_types.h"
#include "app_misc/firmware.h"
#include "app_misc/rtc.h"
#include "../app_misc/include/app_misc/kvstore.hh"
#include "app_settings/config.h"
#include "net/ipnet.h"
#include "txtio/inout.h"
#include "uout/status_json.hh"
#include "uout/cli_out.h"
#include "app_uout/status_output.h"
#include <unistd.h>
#include <app_uout/so_msg.h>
//#include "utils_misc/int_macros.h"

#include <string.h>
#include <utils_misc/cstring_utils.hh>
#include <stdio.h>
#include <time.h>

#define D(x)

void soMsg_fw_start_msg_print(const class UoutWriter &td) {
  static const char msg[] =
      "\n\n" "tf: info: start: tronferno-mcu\n" "tf: info: build-date: " __DATE__ " " __TIME__ "\n"
      "tf: hint: type 'help;' to get a command list\n";

  td.write(msg, sizeof msg -1);
}

void soMsg_status_ok(const class UoutWriter &td) {
  td.so().print("status", "ok");
}

void soMsg_status_error(const class UoutWriter &td) {
  td.so().print("status", "error");
}

void soMsg_mcu_begin(const class UoutWriter &td) {
  td.so().x_open("mcu");
}

void soMsg_mcu_run_time(const class UoutWriter &td) {
  td.so().print("run-time", run_time_s());

}
#ifdef MCU_ESP32
#include "esp_app_desc.h"
void soMsg_mcu_version(const class UoutWriter &td) {
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

#if 0
     ESP_EARLY_LOGI(TAG, "Application information:");
#ifndef CONFIG_APP_EXCLUDE_PROJECT_NAME_VAR
     ESP_EARLY_LOGI(TAG, "Project name:     %s", app_desc->project_name);
#endif
#ifndef CONFIG_APP_EXCLUDE_PROJECT_VER_VAR
     ESP_EARLY_LOGI(TAG, "App version:      %s", app_desc->version);
#endif
#ifdef CONFIG_BOOTLOADER_APP_SECURE_VERSION
     ESP_EARLY_LOGI(TAG, "Secure version:   %d", app_desc->secure_version);
#endif
#ifdef CONFIG_APP_COMPILE_TIME_DATE
     ESP_EARLY_LOGI(TAG, "Compile time:     %s %s", app_desc->date, app_desc->time);
#endif
     char buf[17];
     esp_ota_get_app_elf_sha256(buf, sizeof(buf));
     ESP_EARLY_LOGI(TAG, "ELF file SHA256:  %s...", buf);
     ESP_EARLY_LOGI(TAG, "ESP-IDF:          %s", app_desc->idf_ver);
 }
#endif



#else
void soMsg_mcu_version(const class UoutWriter &td) {
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

void soMsg_mcu_ota(const class UoutWriter &td, const char *url) {
#ifdef CONFIG_APP_USE_OTA
  td.so().print("ota-url", url);
#endif
}
void soMsg_mcu_ota_state(const class UoutWriter &td) {
#ifdef CONFIG_APP_USE_OTA
  td.so().print("ota-state", ota_getState());
#endif
}

void soMsg_mcu_boot_count(const class UoutWriter &td) {
#ifndef TEST_HOST
  extern int32_t boot_counter;
  td.so().print("boot-count", boot_counter);
#else
  td.so().print("boot-count", 0);
#endif
}

void soMsg_mcu_end(const class UoutWriter &td) {
  td.so().x_close();
}

/////////////////////////////////////////////////////////////////////////////////
void soMsg_sep_enable(const class UoutWriter &td) {
  D(td.write("sep enable\n"));
}
void soMsg_sep_disable(const class UoutWriter &td) {
  D(td.write("sep disable\n"));
}
void soMsg_sep_button_pressed_error(const class UoutWriter &td) {
  td.write("error: hardware button is pressed\n");
}

/////////////////////////////////////////////////////////////////////////////////
void soMsg_cfgpasswd_ok(const class UoutWriter &td) {
  // td.write("password ok\n");
}
void soMsg_cfgpasswd_wrong(const class UoutWriter &td) {
  td.write("wrong config password\n");
}

void soMsg_cfgpasswd_missing(const class UoutWriter &td) {
  td.write("missing config password\n");
}

/////////////////////////////////////////////////////////////////////////////////
static uint16_t fer_cuas_msgid;
void soMsg_cuas_start(const class UoutWriter &td, uint16_t id) {
  fer_cuas_msgid = id;
  td.write("U: Press Stop on the Fernotron central unit\n");
}

void soMsg_cuas_timeout(const class UoutWriter &td) {
  td.write("U: Nothing received\n");
  reply_id_message(td, fer_cuas_msgid, "cuas=time-out", 0);
}

void soMsg_cuas_done(const class UoutWriter &td) {
  td.write("U: Central Unit received and stored\n");
  reply_message(td, "cuas=ok", 0);
}

void soMsg_cuas_state(const class UoutWriter &td, int state) {
  td.so().print("cuas", state);

}


/////////////////////////////////////////////////////////////////////////////////
int soMsg_timer_print_begin(const class UoutWriter &td, const char *tag) {
  td.so().x_open(tag);
  return 0;
}

void soMsg_timer_print_end(const class UoutWriter &td) {
  td.so().x_close();
}

int soMsg_timer_begin(const class UoutWriter &td, const so_arg_gm_t a) {

  if (so_cco) {
    char val[] = "0";
    val[0] = '0' + a.g;
    td.st().cli_out_x_reply_entry("g", val);
    val[0] = '0' + a.m;
    td.st().cli_out_x_reply_entry("m", val);
  }
  if (so_jco) {
    char dict[] = "autoGM";
    dict[4] = '0' + a.g;
    dict[5] = '0' + a.m;
    td.sj().add_object(dict);
  }
  return 0;
}

void soMsg_timer_end(const class UoutWriter &td) {
  td.so().x_close();
}

void soMsg_astro_minutes_print(const class UoutWriter &td, const int am) {
    td.so().print("astro-minute", am);
}

/////////////////////////////////////////////////////////////////////////////////

void soMsg_shpref_obj_begin(const class UoutWriter &td) {
  td.so().x_open("shs");
}

void soMsg_shpref_obj_end(const class UoutWriter &td) {
  td.so().x_close();
}

void soMsg_shpref_obj_gm_begin(const class UoutWriter &td, const so_arg_gm_t a) {
  char buf[] = "shs00";
  buf[3] += a.g;
  buf[4] += a.m;
  td.so().x_open(buf);

}

void soMsg_shpref_obj_gm_end(const class UoutWriter &td) {
  td.so().x_close();
}

void soMsg_KVS_begin(const class UoutWriter &td) {
  td.so().x_open("kvs");
}

void soMsg_KVS_end(const class UoutWriter &td) {
  td.so().x_close();
}

bool soMsg_KVS_print(const class UoutWriter &td, const char *key) {
  char buf[64];
#ifndef TEST_HOST
  if (kvs_get_string(key, buf, sizeof buf)) {
    td.so().print(key, buf);
    return true;
  }
#endif
  return false;
}

void soMsg_print_kvd(const class UoutWriter &td, const so_arg_kvd_t a) {
  td.so().print(a.key, a.val);
}

void soMsg_print_kvs(const class UoutWriter &td, so_arg_kvs_t a) {
  td.so().print(a.key, a.val);
}

void soMsg_kv(const class UoutWriter &td, const char *key, const char *val) {
  td.so().print(key, val);
}

void soMsg_kv(const class UoutWriter &td, const char *key, int val) {
  td.so().print(key, val);
}

void soMsg_kv(const class UoutWriter &td, const char *key, bool val);



void soMsg_pos_print_gmp(const class UoutWriter &td, const so_arg_gmp_t a) {
  if (so_cco) {
    char buf[64];
    if (int n = snprintf(buf, sizeof buf, "A:position: g=%d m=%d p=%d;\n", (int)a.g, (int)a.m, (int)a.p); n > 0 && n < sizeof buf) {
      td.write(buf, n);
    }
  }

  if (so_jco) {
    td.sj().add_object("pct");
    char buf[] = "00";
    buf[0] += a.g;
    buf[1] += a.m;
    td.sj().add_key_value_pair_d(buf, a.p);
    td.sj().close_object();
  }
}

void soMsg_pos_print_gmpa(const class UoutWriter &td, const so_arg_gmp_t *a) {
  if (so_cco)
    for (int i = 0; a[i].g <= 7; ++i) {
      char buf[64];
      if (int n = snprintf(buf, sizeof buf, "A:position: g=%d m=%d p=%d;\n", (int)a[i].g, (int)a[i].m, (int)a[i].p); n > 0 && n < sizeof buf) {
        td.write(buf, n);
      }
    }

  if (so_jco) {
    td.sj().add_object("pct");
    for (int i = 0; a[i].g <= 7; ++i) {
      char buf[] = "00";
      buf[0] += a[i].g;
      buf[1] += a[i].m;
      td.sj().add_key_value_pair_d(buf, a[i].p);
    }
    td.sj().close_object();
  }
}

void soMsg_pos_print_mmp(const class UoutWriter &td, const so_arg_mmp_t a) {
  if (so_cco) {
    char buf[64];
    if (int n = snprintf(buf, sizeof buf, "U:position: p=%d mm=%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x;\n", a.p,
        a.mm->getGroup(0), a.mm->getGroup(1), a.mm->getGroup(2), a.mm->getGroup(3),
        a.mm->getGroup(4), a.mm->getGroup(5), a.mm->getGroup(6), a.mm->getGroup(7)
        ); n > 0 && n < sizeof buf) {
      td.write(buf, n);
    }
  }
  if (so_jco) {
    for (auto it = a.mm->begin(1); it; ++it) {
      char buf[] = "00";
      buf[0] += it.getG();
      buf[1] += it.getM();
      td.sj().add_key_value_pair_d(buf, a.p);
    }
  }

}

void soMsg_pos_begin(const class UoutWriter &td) {
  if (so_cco)
    td.write("U:position:start;\n");
  if (so_jco)
    td.sj().add_object("pct");
}

void soMsg_pos_end(const class UoutWriter &td) {
  if (so_cco)
    td.write("U:position:end;\n");
  if (so_jco)
    td.sj().close_object();
}

void soMsg_pair_begin(const class UoutWriter &td) {
  td.so().x_open("pair");
}

void soMsg_pair_end(const class UoutWriter &td) {
  td.so().x_close();
}

void soMsg_pair_all_begin(const class UoutWriter &td) {
  td.so().x_open("all");
}

void soMsg_pair_all_end(const class UoutWriter &td) {
  td.so().x_close();
}

void soMsg_pair_print_kmm(const class UoutWriter &td, const so_arg_kmm_t a) {
  //td.write("pair a="), td.write(a.key), td.write(" mm="), so_print_gmbitmask(a.mm), td.write(";\n");
  char buf[20];
  so_gmbitmask_to_str(buf, a.mm);
  td.so().print(a.key, buf);

}

void soMsg_pair_print_kmm_single(const class UoutWriter &td, const so_arg_kmm_t a) {
  //td.write("pair a="), td.write(a.key), td.write(" mm="), so_print_gmbitmask(a.mm), td.write(";\n");
  char buf[20];
  so_gmbitmask_to_str(buf, a.mm);
  td.so().print("a", a.key);
  td.so().print("mm", buf);

}

#ifdef CONFIG_APP_USE_REPEATER
void soMsg_repeater_begin(const class UoutWriter &td) {
  td.so().x_open("repeater");
}

void soMsg_repeater_end(const class UoutWriter &td) {
  td.so().x_close();
}
#endif

#ifdef CONFIG_APP_USE_NETWORK
void soMsg_inet_print_address(const class UoutWriter &td) {
  char buf[20];
  ipnet_addr_as_string(buf, 20);
  td.write("tf: ipaddr: "), td.write(buf), td.write(";\n");

}

#endif
void soMsg_gpio_pin(const class UoutWriter &td, const so_arg_pch_t a, bool broadcast) {

  char buf[64];
  snprintf(buf, sizeof buf, "gpio%d", (int) a.gpio_num);
  td.so().print(buf, (int)a.level_int);

  if (broadcast)
    uoCb_publish_pinChange(a);
}

void soMsg_sep_obj_begin(const class UoutWriter &td) {
  td.so().x_open("sep");
}

void soMsg_sep_obj_end(const class UoutWriter &td) {
  td.so().x_close();
}
