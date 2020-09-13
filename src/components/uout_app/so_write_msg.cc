/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "so_out.h"
#include "so_print.h"
#include "uout_app/callbacks.h"
#include "app/common.h"
#include "app/firmware.h"
#include "app/rtc.h"
#include "config/config.h"
#include "net/ipnet.h"
#include "txtio/inout.h"
#include "uout/status_json.h"
#include "uout/cli_out.h"
#include "uout_app/status_output.h"

//#include "misc/int_macros.h"

#include <string.h>
#include <stdio.h>
#include <time.h>

#define D(x)


void soMsg_fw_start_msg_print() {
  so_print_startup_info();
}

void soMsg_status_ok() {
  so_out_x_reply_entry_ss("status", "ok");
}

void soMsg_status_error() {
  so_out_x_reply_entry_ss("status", "error");
}

void soMsg_mcu_begin() {
  so_out_x_open("mcu");
}

void soMsg_mcu_run_time() {
  so_out_x_reply_entry_sl("run-time", run_time_s());

}

void soMsg_mcu_version() {
  char buf[64];

  so_out_x_reply_entry_ss("chip", MCU_TYPE);
  so_out_x_reply_entry_ss("firmware", strcat(strcpy(buf, "tronferno-mcu-"), APP_VERSION));

  char *p = strcpy(buf, ISO_BUILD_TIME);
  do
    if (*p == ' ')
      *p = '-';
  while (*++p);
  so_out_x_reply_entry_ss("build-time", buf);
}

void soMsg_mcu_ota(const char *url) {
#ifdef USE_OTA
  so_out_x_reply_entry_ss("ota-url", url);
#endif
}
void soMsg_mcu_ota_state() {
#ifdef USE_OTA
  so_out_x_reply_entry_sd("ota-state", ota_getState());
#endif
}
#ifndef TEST_HOST
void soMsg_mcu_boot_count() {
  extern i32 boot_counter;
  so_out_x_reply_entry_sl("boot-count", boot_counter);

}
#endif
void soMsg_mcu_end() {
  so_out_x_close();
}

/////////////////////////////////////////////////////////////////////////////////
void soMsg_sep_enable() {
  D(io_puts("sep enable\n"));
}
void soMsg_sep_disable() {
  D(io_puts("sep disable\n"));
}
void soMsg_sep_button_pressed_error() {
  io_puts("error: hardware button is pressed\n");
}

/////////////////////////////////////////////////////////////////////////////////
void soMsg_cfgpasswd_ok() {
  // io_puts("password ok\n");
}
void soMsg_cfgpasswd_wrong() {
  io_puts("wrong config password\n");
}

void soMsg_cfgpasswd_missing() {
  io_puts("missing config password\n");
}

/////////////////////////////////////////////////////////////////////////////////
static uint16_t cuas_msgid;
void soMsg_cuas_start(uint16_t id) {
  cuas_msgid = id;
  io_puts("U: Press Stop on the Fernotron central unit\n");
}

void soMsg_cuas_timeout() {
  io_puts("U: Nothing received\n");
  reply_id_message(cuas_msgid, "cuas=time-out", 0);
}

void soMsg_cuas_done() {
  io_puts("U: Central Unit received and stored\n");
  reply_message("cuas=ok", 0);
}

void soMsg_cuas_state(int state) {
  so_out_x_reply_entry_sd("cuas", state);

}

/////////////////////////////////////////////////////////////////////////////////
static uint16_t pras_msgid;

void soMsg_pras_start_listening(uint16_t id) {
  pras_msgid = id;
  io_puts("U:pras: start listening at RF\n");
}

void soMsg_pras_stop_listening() {
  if (pras_msgid) {
    io_puts("U:pras: success\n");
  } else {
    io_puts("U:pras: failure\n");
  }
}

void soMsg_pras_timeout() {
  reply_id_message(pras_msgid, "pras=time-out", 0);
}

void soMsg_pras_done(bool success, bool unpair) {
  char buf[64];
  snprintf(buf, sizeof buf, "U:pras: %s: controller was%s %spaired\n", success ? "success" : "failure", success ? "" : " NOT", unpair ? "un" : "");
  reply_id_message(pras_msgid, success ? "pras=ok" : "pras:error", 0);

}
/////////////////////////////////////////////////////////////////////////////////
void soMsg_timer_event_print(const so_arg_gm_t a) {
  so_print_timer_event_minutes(a.g, a.m);
}

int soMsg_timer_print_begin() {
  if (so_jco)
    return sj_add_object("auto");
  return -1;
}

void soMsg_timer_print_end() {
  if (so_jco)
     sj_close_object(); // XXX: disabled
}

int soMsg_timer_begin(const so_arg_gm_t a) {
  char dict[] = "autoGM";
  dict[4] = '0' + a.g;
  dict[5] = '0' + a.m;
  return sj_add_object(dict);
}

void soMsg_timer_end() {
  if (so_jco)
    sj_close_object();
}

void soMsg_astro_minutes_print(const int am) {
    so_out_x_reply_entry_sl("astro-minute", am);
}

/////////////////////////////////////////////////////////////////////////////////

void soMsg_shpref_obj_begin() {
  sj_add_object("shs");
}

void soMsg_shpref_obj_end() {
  sj_close_object();
}

void soMsg_shpref_obj_gm_begin(const so_arg_gm_t a) {
  char buf[] = "shs00";
  buf[3] += a.g;
  buf[4] += a.m;
  so_out_x_open(buf);

}

void soMsg_shpref_obj_gm_end() {
  so_out_x_close();
}

void soMsg_print_kvd(const so_arg_kvd_t a) {
  so_out_x_reply_entry_sd(a.key, a.val);
}

void soMsg_print_kvs(so_arg_kvs_t a) {
  so_out_x_reply_entry_ss(a.key, a.val);
}

void soMsg_kv(const char *key, const char *val) {
  so_out_x_reply_entry_ss(key, val);
}

void soMsg_kv(const char *key, int val) {
  so_out_x_reply_entry_sd(key, val);
}

void soMsg_kv(const char *key, bool val);



void soMsg_pos_print_gmp(const so_arg_gmp_t a, bool broadcast) {
  if (so_cco) {
    io_puts("A:position:");
    io_puts(" g="), io_putd(a.g);
    io_puts(" m="), io_putd(a.m);
    io_puts(" p="), io_putd(a.p), io_puts(";\n");
  }

  if (so_jco || broadcast) {
    sj_add_object("pct");
    char buf[] = "00";
    buf[0] += a.g;
    buf[1] += a.m;
    sj_add_key_value_pair_d(buf, a.p);
    sj_close_object();

    if (broadcast)
      uoApp_publish_gmpJson(sj_get_json());
  }

  if (so_mqt || broadcast)
    uoApp_publish_gmpObj(a);
}

void soMsg_pos_print_gmpa(const so_arg_gmp_t *a, bool broadcast) {
  if (so_cco)
    for (int i = 0; a[i].g <= 7; ++i) {
      io_puts("A:position:");
      io_puts(" g="), io_putd(a[i].g);
      io_puts(" m="), io_putd(a[i].m);
      io_puts(" p="), io_putd(a[i].p), io_puts(";\n");
    }

  if (so_jco || broadcast) {
    sj_add_object("pct");
    for (int i = 0; a[i].g <= 7; ++i) {
      char buf[] = "00";
      buf[0] += a[i].g;
      buf[1] += a[i].m;
      sj_add_key_value_pair_d(buf, a[i].p);
    }
    sj_close_object();

    if (broadcast)
      uoApp_publish_gmpJson(sj_get_json());
  }

  if (so_mqt || broadcast) // XXX
    for (int i = 0; a[i].g <= 7; ++i) {
      uoApp_publish_gmpObj(a[i]);
    }
}

void soMsg_pos_print_mmp(const so_arg_mmp_t a) {
  if (so_cco)
    io_puts("U:position:"), io_puts(" p="), io_putd(a.p), io_puts(" mm="), so_print_gmbitmask(a.mm), io_puts(";\n");
  if (so_jco) {
#ifdef USE_PCT_ARRAY
    char buf[] = "pct255";
    itoa(a.p, buf+3, 10);
    sj_add_array(buf);
    for (auto it = a.mm->begin(1); it; ++it) {
      sj_add_value_d(it.getG() * 10 + it.getM());
    }
    sj_close_array();
#else
    for (auto it = a.mm->begin(1); it; ++it) {
      char buf[] = "00";
      buf[0] += it.getG();
      buf[1] += it.getM();
      sj_add_key_value_pair_d(buf, a.p);
    }
#endif
  }

}

void soMsg_pos_begin() {
  if (so_cco)
    io_puts("U:position:start;\n");
#ifndef USE_PCT_ARRAY
  if (so_jco)
    sj_add_object("pct");
#endif
}

void soMsg_pos_end() {
  if (so_cco)
    io_puts("U:position:end;\n");
#ifndef USE_PCT_ARRAY
  if (so_jco)
    sj_close_object();
#endif
}

void soMsg_pair_begin() {
  so_out_x_open("pair");
}

void soMsg_pair_end() {
  so_out_x_close();
}

void soMsg_pair_all_begin() {
  so_out_x_open("all");
}

void soMsg_pair_all_end() {
  so_out_x_close();
}

void soMsg_pair_print_amm(const so_arg_amm_t a) {
  io_puts("pair a="), io_print_hex(a.a, false), io_puts(" mm="), so_print_gmbitmask(a.mm), io_puts(";\n");

}

void soMsg_pair_print_kmm(const so_arg_kmm_t a) {
  //io_puts("pair a="), io_puts(a.key), io_puts(" mm="), so_print_gmbitmask(a.mm), io_puts(";\n");
  char buf[20];
  so_gmbitmask_to_str(buf, a.mm);
  so_out_x_reply_entry_ss(a.key, buf);

}

void soMsg_pair_print_kmm_single(const so_arg_kmm_t a) {
  //io_puts("pair a="), io_puts(a.key), io_puts(" mm="), so_print_gmbitmask(a.mm), io_puts(";\n");
  char buf[20];
  so_gmbitmask_to_str(buf, a.mm);
  so_out_x_reply_entry_ss("a", a.key);
  so_out_x_reply_entry_ss("mm", buf);

}

#ifdef USE_NETWORK
void soMsg_inet_print_address() {
  char buf[20];
  ipnet_addr_as_string(buf, 20);
  io_puts("tf: ipaddr: "), io_puts(buf), io_puts(";\n");

}

#endif
void soMsg_gpio_pin(const so_arg_pch_t a, bool broadcast) {

  if (so_cco) {
    io_printf("tf:reply: mcu gpio%d=%d;\n", (int)a.gpio_num, (int)a.level);
  }

  if (so_jco) {
    char buf[64];
    snprintf(buf, sizeof buf, "gpio%d", (int)a.gpio_num);
    sj_add_key_value_pair_d(buf, (int)a.level);
  }

  if (broadcast)
    uoApp_publish_pinChange(a);
}


