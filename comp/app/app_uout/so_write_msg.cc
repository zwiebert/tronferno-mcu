/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "so_out.h"
#include "so_print.h"
#include "app_uout/callbacks.h"
#include "utils_misc/int_types.h"
#include "app_misc/firmware.h"
#include "app_misc/rtc.h"
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
#include <stdio.h>
#include <time.h>

#define D(x)

void soMsg_fw_start_msg_print(const struct TargetDesc &td) {
  static const char msg[] =
      "\n\n" "tf: info: start: tronferno-mcu\n" "tf: info: build-date: " __DATE__ " " __TIME__ "\n"
      "tf: hint: type 'help;' to get a command list\n";

  td.write(msg, sizeof msg -1);
}

void soMsg_status_ok(const struct TargetDesc &td) {
  td.so().print("status", "ok");
}

void soMsg_status_error(const struct TargetDesc &td) {
  td.so().print("status", "error");
}

void soMsg_mcu_begin(const struct TargetDesc &td) {
  td.so().x_open("mcu");
}

void soMsg_mcu_run_time(const struct TargetDesc &td) {
  td.so().print("run-time", run_time_s());

}
#ifdef MCU_ESP32
#include "esp_ota_ops.h"
void soMsg_mcu_version(const struct TargetDesc &td) {
  char buf[64];
  const esp_app_desc_t *ad = esp_ota_get_app_description();

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
void soMsg_mcu_version(const struct TargetDesc &td) {
  char buf[64];

  td.so().print("chip", MCU_TYPE);
  //td.so().print("firmware", (csu_copy_cat(buf, "tronferno-mcu-", APP_VERSION), buf));

  char *p = STRCPY(buf, ISO_BUILD_TIME);
  do
    if (*p == ' ')
      *p = '-';
  while (*++p);
  td.so().print("build-time", buf);
}
#endif

void soMsg_mcu_ota(const struct TargetDesc &td, const char *url) {
#ifdef USE_OTA
  td.so().print("ota-url", url);
#endif
}
void soMsg_mcu_ota_state(const struct TargetDesc &td) {
#ifdef USE_OTA
  td.so().print("ota-state", ota_getState());
#endif
}
#ifndef TEST_HOST
void soMsg_mcu_boot_count(const struct TargetDesc &td) {
  extern i32 boot_counter;
  td.so().print("boot-count", boot_counter);

}
#endif
void soMsg_mcu_end(const struct TargetDesc &td) {
  td.so().x_close();
}

/////////////////////////////////////////////////////////////////////////////////
void soMsg_sep_enable(const struct TargetDesc &td) {
  D(td.write("sep enable\n"));
}
void soMsg_sep_disable(const struct TargetDesc &td) {
  D(td.write("sep disable\n"));
}
void soMsg_sep_button_pressed_error(const struct TargetDesc &td) {
  td.write("error: hardware button is pressed\n");
}

/////////////////////////////////////////////////////////////////////////////////
void soMsg_cfgpasswd_ok(const struct TargetDesc &td) {
  // td.write("password ok\n");
}
void soMsg_cfgpasswd_wrong(const struct TargetDesc &td) {
  td.write("wrong config password\n");
}

void soMsg_cfgpasswd_missing(const struct TargetDesc &td) {
  td.write("missing config password\n");
}

/////////////////////////////////////////////////////////////////////////////////
static uint16_t fer_cuas_msgid;
void soMsg_cuas_start(const struct TargetDesc &td, uint16_t id) {
  fer_cuas_msgid = id;
  td.write("U: Press Stop on the Fernotron central unit\n");
}

void soMsg_cuas_timeout(const struct TargetDesc &td) {
  td.write("U: Nothing received\n");
  reply_id_message(td, fer_cuas_msgid, "cuas=time-out", 0);
}

void soMsg_cuas_done(const struct TargetDesc &td) {
  td.write("U: Central Unit received and stored\n");
  reply_message(td, "cuas=ok", 0);
}

void soMsg_cuas_state(const struct TargetDesc &td, int state) {
  td.so().print("cuas", state);

}

/////////////////////////////////////////////////////////////////////////////////
static uint16_t pras_msgid;

void soMsg_pras_start_listening(const struct TargetDesc &td, uint16_t id) {
  pras_msgid = id;
  td.write("U:pras: start listening at RF\n");
}

void soMsg_pras_stop_listening(const struct TargetDesc &td) {
  if (pras_msgid) {
    td.write("U:pras: success\n");
  } else {
    td.write("U:pras: failure\n");
  }
}

void soMsg_pras_timeout(const struct TargetDesc &td) {
  reply_id_message(td, pras_msgid, "pras=time-out", 0);
}

void soMsg_pras_done(const struct TargetDesc &td, bool success, bool unpair) {
  char buf[64];
  snprintf(buf, sizeof buf, "U:pras: %s: controller was%s %spaired\n", success ? "success" : "failure", success ? "" : " NOT", unpair ? "un" : "");
  reply_id_message(td, pras_msgid, success ? "pras=ok" : "pras:error", 0);

}
/////////////////////////////////////////////////////////////////////////////////
void soMsg_timer_event_print(const struct TargetDesc &td, const so_arg_gm_t a) {
  so_print_timer_event_minutes(a.g, a.m);
}

int soMsg_timer_print_begin(const struct TargetDesc &td, const char *tag) {
  td.so().x_open(tag);
  return 0;
}

void soMsg_timer_print_end(const struct TargetDesc &td) {
  td.so().x_close();
}

int soMsg_timer_begin(const struct TargetDesc &td, const so_arg_gm_t a) {

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

void soMsg_timer_end(const struct TargetDesc &td) {
  td.so().x_close();
}

void soMsg_astro_minutes_print(const struct TargetDesc &td, const int am) {
    td.so().print("astro-minute", am);
}

/////////////////////////////////////////////////////////////////////////////////

void soMsg_shpref_obj_begin(const struct TargetDesc &td) {
  td.sj().add_object("shs");
}

void soMsg_shpref_obj_end(const struct TargetDesc &td) {
  td.sj().close_object();
}

void soMsg_shpref_obj_gm_begin(const struct TargetDesc &td, const so_arg_gm_t a) {
  char buf[] = "shs00";
  buf[3] += a.g;
  buf[4] += a.m;
  td.so().x_open(buf);

}

void soMsg_shpref_obj_gm_end(const struct TargetDesc &td) {
  td.so().x_close();
}

void soMsg_print_kvd(const struct TargetDesc &td, const so_arg_kvd_t a) {
  td.so().print(a.key, a.val);
}

void soMsg_print_kvs(const struct TargetDesc &td, so_arg_kvs_t a) {
  td.so().print(a.key, a.val);
}

void soMsg_kv(const struct TargetDesc &td, const char *key, const char *val) {
  td.so().print(key, val);
}

void soMsg_kv(const struct TargetDesc &td, const char *key, int val) {
  td.so().print(key, val);
}

void soMsg_kv(const struct TargetDesc &td, const char *key, bool val);



void soMsg_pos_print_gmp(const struct TargetDesc &td, const so_arg_gmp_t a) {
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

void soMsg_pos_print_gmpa(const struct TargetDesc &td, const so_arg_gmp_t *a) {
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

void soMsg_pos_print_mmp(const struct TargetDesc &td, const so_arg_mmp_t a) {
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
#ifdef USE_PCT_ARRAY
    char buf[] = "pct255";
    itoa(a.p, buf+3, 10);
    td.sj().add_array(buf);
    for (auto it = a.mm->begin(1); it; ++it) {
      td.sj().add_value_d(it.getG() * 10 + it.getM());
    }
    td.sj().close_array();
#else
    for (auto it = a.mm->begin(1); it; ++it) {
      char buf[] = "00";
      buf[0] += it.getG();
      buf[1] += it.getM();
      td.sj().add_key_value_pair_d(buf, a.p);
    }
#endif
  }

}

void soMsg_pos_begin(const struct TargetDesc &td) {
  if (so_cco)
    td.write("U:position:start;\n");
#ifndef USE_PCT_ARRAY
  if (so_jco)
    td.sj().add_object("pct");
#endif
}

void soMsg_pos_end(const struct TargetDesc &td) {
  if (so_cco)
    td.write("U:position:end;\n");
#ifndef USE_PCT_ARRAY
  if (so_jco)
    td.sj().close_object();
#endif
}

void soMsg_pair_begin(const struct TargetDesc &td) {
  td.so().x_open("pair");
}

void soMsg_pair_end(const struct TargetDesc &td) {
  td.so().x_close();
}

void soMsg_pair_all_begin(const struct TargetDesc &td) {
  td.so().x_open("all");
}

void soMsg_pair_all_end(const struct TargetDesc &td) {
  td.so().x_close();
}

void soMsg_pair_print_amm(const struct TargetDesc &td, const so_arg_amm_t a) {
  td.write("pair a="), io_print_hex(a.a, false), td.write(" mm="), so_print_gmbitmask(a.mm), td.write(";\n");

}

void soMsg_pair_print_kmm(const struct TargetDesc &td, const so_arg_kmm_t a) {
  //td.write("pair a="), td.write(a.key), td.write(" mm="), so_print_gmbitmask(a.mm), td.write(";\n");
  char buf[20];
  so_gmbitmask_to_str(buf, a.mm);
  td.so().print(a.key, buf);

}

void soMsg_pair_print_kmm_single(const struct TargetDesc &td, const so_arg_kmm_t a) {
  //td.write("pair a="), td.write(a.key), td.write(" mm="), so_print_gmbitmask(a.mm), td.write(";\n");
  char buf[20];
  so_gmbitmask_to_str(buf, a.mm);
  td.so().print("a", a.key);
  td.so().print("mm", buf);

}

#ifdef USE_NETWORK
void soMsg_inet_print_address(const struct TargetDesc &td) {
  char buf[20];
  ipnet_addr_as_string(buf, 20);
  td.write("tf: ipaddr: "), td.write(buf), td.write(";\n");

}

#endif
void soMsg_gpio_pin(const struct TargetDesc &td, const so_arg_pch_t a, bool broadcast) {

  if (so_cco) {
    io_printf("tf:reply: mcu gpio%d=%d;\n", (int)a.gpio_num, (int)a.level);
  }

  if (so_jco) {
    char buf[64];
    snprintf(buf, sizeof buf, "gpio%d", (int)a.gpio_num);
    td.sj().add_key_value_pair_d(buf, (int)a.level);
  }

  if (broadcast)
    uoApp_publish_pinChange(a);
}


