/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */
#include "app/proj_app_cfg.h"

#include "fernotron_auto/timer_data.h"
#include "userio_app/status_output.h"
#ifdef USE_JSON
#include "userio/status_json.h"
#else
#define so_json_config_reply(a,b,c)
#endif
#define so_jto so_jco

#include <string.h>
#include <stdio.h>

#include "fernotron_auto/timer_state.h"
#include "cli_app/cli_imp.h" // FIXME?
#include "cli_app/cli_config.h"
#include "config/config.h"
#include "main/common.h"
#include "fernotron_alias/pairings.h"
#include "main/rtc.h"
#include "fernotron_cuas/cuid_auto_set.h"
#include "misc/int_macros.h"
#include "txtio/inout.h"
#include "net/mqtt.h"
#include "userio/status_json.h"
#include "net/ipnet.h"
#include "app/ota.h"

#define D(x)

u8 so_target;

bool out_cli = true;
#ifdef USE_JSON
bool out_js = true;
#define so_jco cli_isJson
#else
bool out_js = false;
#define so_jco false
#endif

#define so_cco (!cli_isJson)


#define so_cto so_cco


char * ftoa(float f, char *buf, int n) {
  int i;
  i32 mult;
  u32 rop;
  i16 lop = (i16) f;
  char *s = buf;

  itoa(lop, s, 10);
  strcat(s, ".");
  s += strlen(s);

  f -= lop;

  mult = (lop < 0) ? -1 : 1;

  for (i = 0; i < n; ++i)
    mult *= 10;

  rop = (u32) (f * mult);
  ltoa(rop, s, 10);

  return buf;
}

// get the so_msg_t for a config key string (or SO_NONE)
so_msg_t 
so_parse_config_key(const char *k) {
  int i;
  for (i = 0; i < (SO_CFG_end - SO_CFG_begin); ++i) {
    if (0 == strcmp(k, cfg_keys[i]))
      return i + SO_CFG_begin + 1;
  }
  return SO_NONE;
}

// get config key string for a so_msgt_t (or NULL)
static const char *
gk(so_msg_t so_key) {
  if (SO_CFG_begin < so_key && so_key < SO_CFG_end)
    return cfg_keys[so_key - (SO_CFG_begin + 1)];
  return NULL;
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

void so_out_x_reply_entry(so_msg_t key, const char *val) {
  if (so_cco) cli_out_x_reply_entry2(gk(key), val);
}

void so_out_x_reply_entry_s(so_msg_t key, const char *val) {
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

void so_out_x_reply_entry_d(so_msg_t key, int val) {
  so_out_x_reply_entry_sd(gk(key), val);
}

void so_out_x_reply_entry_l(so_msg_t key, int val) {
  so_out_x_reply_entry_sl(gk(key), val);
}

void so_out_x_reply_entry_lx(so_msg_t key, int val) {
  char buf[20];
  ltoa(val, buf, 16);
  if (so_cco) cli_out_x_reply_entry2(gk(key), buf);
  if (so_jco) sj_add_key_value_pair_s(gk(key), buf); //no hex in json. use string
}

void so_out_x_reply_entry_f(so_msg_t key, float val, int n) {
  char buf[20];
  ftoa(val, buf, 5);
  if (so_cco) cli_out_x_reply_entry2(gk(key), buf);
  if (so_jco) sj_add_key_value_pair_f(gk(key), val);
}



static void so_print_timer_event_minutes(u8 g, u8 m);
static void so_print_timer_as_text(u8 g, u8 m, bool wildcard);
static void so_print_timer(u8 g, u8 m);
static void so_print_gmbitmask(gm_bitmask_t mm);
static void so_print_startup_info(void);
static void so_gmbitmask_to_str(char *dst, gm_bitmask_t mm);

void  so_output_message(so_msg_t mt, void *arg) {
  static u16 pras_msgid, cuas_msgid;
  char buf[64];
  int i;

  switch (mt) {
  case SO_FW_START_MSG_PRINT:
    so_print_startup_info();
    break;

  case SO_STATUS_OK:
    so_out_x_reply_entry_ss("status", "ok");
    break;

  case SO_STATUS_ERROR:
    so_out_x_reply_entry_ss("status", "error");
    break;

  case SO_MCU_begin: {
    so_out_x_open("mcu");
  }
    break;

  case SO_MCU_RUN_TIME: {
    so_out_x_reply_entry_sl("run-time", run_time());

  }
    break;

  case SO_MCU_VERSION: {
    so_out_x_reply_entry_ss("chip", MCU_TYPE);

    so_out_x_reply_entry_ss("firmware", strcat(strcpy(buf, "tronferno-mcu-"), APP_VERSION));

    char *p = strcpy(buf, ISO_BUILD_TIME);
    do
      if (*p == ' ')
        *p = '-';
    while (*++p);
    so_out_x_reply_entry_ss("build-time", buf);
  }
  break;

  case SO_MCU_OTA:
#ifdef USE_OTA
    so_out_x_reply_entry_ss("ota-url", arg);
#endif
    break;
  case SO_MCU_OTA_STATE:
#ifdef USE_OTA
    so_out_x_reply_entry_sd("ota-state", ota_getState());
#endif
    break;

  case SO_MCU_end:
    so_out_x_close();
    break;


    /////////////////////////////////////////////////////////////////////////////////
  case SO_SEP_ENABLE:
    D(io_puts("sep enable\n"));
    break;
  case SO_SEP_DISABLE:
    D(io_puts("sep disable\n"));
    break;
  case SO_SEP_BUTTON_PRESSED_ERROR:
    io_puts("error: hardware button is pressed\n");
    break;

  /////////////////////////////////////////////////////////////////////////////////
  case SO_CFG_all: {
    bool old_out_js = out_js, old_out_cli = out_cli;
    char *json_buf = NULL;

    if (arg) {
      const char *f;

      out_js = out_cli = false;
      for (f = arg; *f; ++f)
        switch (*f) {
        case 'j':
          out_js = true;
          break;
        case 'c':
          out_cli = true;
          break;
        }

    }
    if (!out_js || ((json_buf = malloc(256)))) {
      for (i = SO_CFG_begin+1; i < SO_CFG_end; ++i) {
        so_output_message(i, NULL);
      }
      free(json_buf);
    }
    out_js = old_out_js;
    out_cli = old_out_cli;
  }
    break;

  case SO_CFG_BAUD:
#ifndef MCU_ESP32
    so_out_x_reply_entry_l(mt, C.mcu_serialBaud);
#endif
    break;
  case SO_CFG_RTC:
    if (rtc_get_by_string(buf)) {
      so_out_x_reply_entry_s(mt, buf);
    }
    break;
  case SO_CFG_CU:
    so_out_x_reply_entry_lx(mt, C.fer_centralUnitID);
    break;
#ifdef USE_NETWORK
  case SO_CFG_NETWORK:
    so_out_x_reply_entry_s(mt, cfg_args_network[C.network]);
    break;
#endif
#ifdef USE_LAN
  case SO_CFG_LAN_PHY:
    so_out_x_reply_entry_s(mt, cfg_args_lanPhy[C.lan_phy]);
    break;
  case SO_CFG_LAN_PWR_GPIO:
    so_out_x_reply_entry_d(mt, C.lan_pwr_gpio);
    break;
#else
  case SO_CFG_LAN_PHY:
  case SO_CFG_LAN_PWR_GPIO:
    break;
#endif
#ifdef USE_WLAN
  case SO_CFG_WLAN_SSID:
    so_out_x_reply_entry_s(mt, C.wifi_SSID);
    break;
  case SO_CFG_WLAN_PASSWORD:
    so_out_x_reply_entry_s(mt, *C.wifi_password ? "*" : "");
    break;
#endif
#ifdef USE_NTP
  case SO_CFG_NTP_SERVER:
    so_out_x_reply_entry_s(mt, C.ntp_server);
    break;
#endif
#ifdef USE_MQTT
  case SO_CFG_MQTT_ENABLE:
    so_out_x_reply_entry_d(mt, C.mqtt_enable ? 1 : 0);
    break;
  case SO_CFG_MQTT_URL:
    so_out_x_reply_entry_s(mt, C.mqtt_url);
    break;
  case SO_CFG_MQTT_USER:
    so_out_x_reply_entry_s(mt, C.mqtt_user);
    break;
  case SO_CFG_MQTT_PASSWORD:
    so_out_x_reply_entry_s(mt, *C.mqtt_password ? "*" : "");
    break;
#else
  case SO_CFG_MQTT_ENABLE:
  case SO_CFG_MQTT_URL:
  case SO_CFG_MQTT_USER:
  case SO_CFG_MQTT_PASSWORD:
    break;
#endif

#ifdef USE_HTTP
  case SO_CFG_HTTP_ENABLE:
    so_out_x_reply_entry_d(mt, C.http_enable ? 1 : 0);
    break;
  case SO_CFG_HTTP_USER:
    so_out_x_reply_entry_s(mt, C.http_user);
    break;
  case SO_CFG_HTTP_PASSWORD:
    so_out_x_reply_entry_s(mt, *C.http_password ? "*" : "");
    break;
#else
  case SO_CFG_HTTP_ENABLE:
  case SO_CFG_HTTP_USER:
  case SO_CFG_HTTP_PASSWORD:
    break;
#endif

  case SO_CFG_LONGITUDE:
    so_out_x_reply_entry_f(mt, C.geo_longitude, 5);
    break;
  case SO_CFG_LATITUDE:
    so_out_x_reply_entry_f(mt, C.geo_latitude, 5);
    break;

  case SO_CFG_TIMEZONE:
#ifndef POSIX_TIME
    so_out_x_reply_entry_f(mt, C.geo_timezone, 5);
#endif
    break;

  case SO_CFG_VERBOSE:
    so_out_x_reply_entry_d(mt, C.app_verboseOutput);
    break;
    case SO_CFG_TZ:
#ifdef POSIX_TIME
      so_out_x_reply_entry_s(mt, C.geo_tz);
#endif
    break;
  case SO_CFG_DST:
#ifdef MDR_TIME
  {
    const char *dst = (C.geo_dST == dstEU ? "eu" : (C.geo_dST == dstNone ? "0" : "1"));
    so_out_x_reply_entry_s(mt, dst);
  }
#endif
    break;

  case SO_CFG_GM_USED: {
    so_out_x_reply_entry_lx(mt, C.fer_usedMembers);
  }
    break;

    case SO_CFG_GPIO_PIN:
#ifdef ACCESS_GPIO
      {
      int gpio_number = *(int *)arg;
      char key[10] = "gpio";
      strcat(key, itoa(gpio_number, buf, 10));
      char ps[2] = "x";
      if (is_gpio_number_usable(gpio_number, true)) {
        ps[0] = pin_state_args[C.gpio[gpio_number]];
      }
      so_out_x_reply_entry_ss(key, ps);
    }
#endif
    break;

    case SO_CFG_ASTRO_CORRECTION: {
      so_out_x_reply_entry_l(mt, C.astroCorrection);
    }
    break;

  case SO_CFG_begin:
    so_out_x_open("config");
    break;

  case SO_CFG_end:
    so_out_x_close();
    break;

    /////////////////////////////////////////////////////////////////////////////////
  case SO_CFGPASSWD_OK:
    // io_puts("password ok\n");
    break;
  case SO_CFGPASSWD_WRONG:
    io_puts("wrong config password\n");
    break;
  case SO_CFGPASSWD_MISSING:
    io_puts("missing config password\n");
    break;

    /////////////////////////////////////////////////////////////////////////////////
  case SO_CUAS_START:
    cuas_msgid = *(u16 *) arg;
    io_puts("U: Press Stop on the Fernotron central unit\n");
    break;
  case SO_CUAS_TIMEOUT:
    io_puts("U: Nothing received\n");
    reply_id_message(cuas_msgid, "cuas=time-out", 0);
    break;

  case SO_CUAS_DONE:
    io_puts("U: Central Unit received and stored\n");
    reply_message("cuas=ok", 0);
    break;

  case SO_CUAS_STATE: {
    cuas_state_T state = cuas_getState();
    so_out_x_reply_entry_sd("cuas", state);
  }
    break;

    /////////////////////////////////////////////////////////////////////////////////
  case SO_PRAS_START_LISTENING:
    pras_msgid = *(u16 *) arg;
    io_puts("U:pras: start listening at RF\n");
    break;
  case SO_PRAS_STOP_LISTENING:
    if (arg && *(u16 *) arg) {
      io_puts("U:pras: success\n");
    } else {
      io_puts("U:pras: failure\n");
    }
    break;
  case SO_PRAS_TIMEOUT:
    reply_id_message(pras_msgid, "pras=time-out", 0);
    break;

  case SO_PRAS_DONE:
    switch (*(int *) arg) {
    case PC_unpair:
      io_puts("U:pras: success: controller was unpaired\n");
      break;
    case PC_pair:
      io_puts("U:pras: success: controller was paired\n");
      break;
    case -PC_unpair:
      io_puts("U:pras: failure: controller was NOT unpaired\n");
      break;
    case -PC_pair:
      io_puts("U:pras: failure: controller was NOT paired\n");
      break;
    default:
      break;
    }
    {
      bool success = (0 < *(int *) arg);
      reply_id_message(pras_msgid, success ? "pras=ok" : "pras:error", 0);
    }
    break;

    /////////////////////////////////////////////////////////////////////////////////
  case SO_TIMER_EVENT_PRINT: {
    so_arg_gm_t *a = arg;
    so_print_timer_event_minutes(a->g, a->m);
  }
    break;

  case SO_TIMER_PRINT: {
    so_arg_gm_t *a = arg;
    so_print_timer(a->g, a->m);
  }
    break;

  case SO_ASTRO_MINUTES_PRINT: {
    so_arg_gm_t *a = arg;
    u8 g = a->g, m = a->m;
    timer_minutes_t tmi;
    if (get_timer_minutes(&tmi, &g, &m, true)) {
      so_out_x_reply_entry_sl("astro-minute", tmi.minutes[ASTRO_MINTS]);
    }

  }
break;
    /////////////////////////////////////////////////////////////////////////////////

  case SO_SHPREF_begin:
    so_out_x_open("shpref");
    break;
  case SO_SHPREF_end:
    so_out_x_close();
    break;

  case SO_SHPREF_PRINT_GMT: {
    so_arg_gmt_t *a = arg;
    char buf[]="shp00";
    buf[3] = '0' + a->g;
    buf[4] = '0' + a->m;
    so_out_x_open(buf);
    so_out_x_reply_entry_sd("mvut", a->st->move_up_tsecs);
    so_out_x_reply_entry_sd("mvdt", a->st->move_down_tsecs);
    so_out_x_close();
  }
  break;

  case SO_POS_PRINT_GMP: {
    so_arg_gmp_t *a = arg;
    io_puts("A:position:");
    io_puts(" g="), io_putd(a->g);
    io_puts(" m="), io_putd(a->m);
    io_puts(" p="), io_putd(a->p), io_puts(";\n");

    so_out_x_open("position");
    so_out_x_reply_entry_sd("g", a->g);
    so_out_x_reply_entry_sd("m", a->m);
    so_out_x_reply_entry_sd("p", a->p);
    so_out_x_close();


#ifdef USE_MQTT
    io_mqtt_publish_gmp(a);
#endif
  }
    break;

  case SO_POS_PRINT_MMP: {
    so_arg_mmp_t *a = arg;
    io_puts("U:position:"), io_puts(" p="), io_putd(a->p), io_puts(" mm="), so_print_gmbitmask(a->mm), io_puts(";\n");
  }
    break;

  case SO_POS_begin:
    io_puts("U:position:start;\n");
    break;

  case SO_POS_end:
    io_puts("U:position:end;\n");
    break;

  case SO_PAIR_begin:
    so_out_x_open("pair");
    break;
  case SO_PAIR_end:
    so_out_x_close();
    break;
  case SO_PAIR_ALL_begin:
    so_out_x_open("all");
    break;
  case SO_PAIR_ALL_end:
    so_out_x_close();
    break;

  case SO_PAIR_PRINT_AMM: {
    so_arg_amm_t *a = arg;
    io_puts("pair a="), io_print_hex(a->a, false), io_puts(" mm="), so_print_gmbitmask(a->mm), io_puts(";\n");
  }
    break;

  case SO_PAIR_PRINT_KMM: {
    so_arg_kmm_t *a = arg;
    //io_puts("pair a="), io_puts(a->key), io_puts(" mm="), so_print_gmbitmask(a->mm), io_puts(";\n");
    char buf[20];
    so_gmbitmask_to_str(buf, a->mm);
    so_out_x_reply_entry_ss(a->key, buf);
  }
    break;

  case SO_PAIR_PRINT_KMM_SINGLE: {
    so_arg_kmm_t *a = arg;
    //io_puts("pair a="), io_puts(a->key), io_puts(" mm="), so_print_gmbitmask(a->mm), io_puts(";\n");
    char buf[20];
    so_gmbitmask_to_str(buf, a->mm);
    so_out_x_reply_entry_ss("a", a->key);
    so_out_x_reply_entry_ss("mm", buf);
  }
    break;

  case SO_INET_PRINT_ADDRESS: {
    char buf[20];
    ipnet_addr_as_string(buf, 20);
    io_puts("tf: ipaddr: "), io_puts(buf), io_puts(";\n");
  }
  break;

  default:
#ifndef DISTRIBUTION
    io_puts("internal_error:so_output_message() unhandled message: "), io_putd(mt), io_putlf();
#endif
    break;
  }

}

static void print_timer_event_minute(const char *label, minutes_t mins) {
  io_puts(label);
  if (mins == MINUTES_DISABLED)
    io_puts("=none\n");
  else
    io_putc('='), io_putd(mins), io_putlf();
}

static void  so_print_timer_event_minutes(u8 g, u8 m) {
  timer_minutes_t tm;
  if (get_timer_minutes(&tm, &g, &m, true)) {
    print_timer_event_minute("astro-down", tm.minutes[0]);
    print_timer_event_minute("daily-up", tm.minutes[1]);
    print_timer_event_minute("daily-down", tm.minutes[2]);
    print_timer_event_minute("weekly-up", tm.minutes[3]);
    print_timer_event_minute("weekly-down", tm.minutes[4]);
  }
}



static void  so_print_timer_as_text(u8 g, u8 m, bool wildcard) {
  timer_data_t tdr;
  char buf[10];
  u8 g_res = g, m_res = m;
  extern gm_bitmask_t manual_bits; //FIXME
  // read_gm_bitmask("MANU", &manual_bits, 1); //FIXME: not needed
  bool f_manual = GET_BIT(manual_bits[g], m);

  if (so_cto) cli_out_timer_reply_entry(NULL, NULL, 1);

  if (read_timer_data(&tdr, &g_res, &m_res, wildcard)) {


    if (so_cto) cli_out_timer_reply_entry("g", itoa(g, buf, 10), 0);
    if (so_cto) cli_out_timer_reply_entry("m", itoa(m, buf, 10), 0);

    char *p = buf;
    *p++ = f_manual ? 'M' : 'm';
    *p++ = td_is_random(&tdr) ? 'R' : 'r';
    *p++ = td_is_sun_auto(&tdr) ? 'S' : 's';
    *p++ = td_is_astro(&tdr) ? 'A' : 'a';
    *p++ = td_is_daily(&tdr) ? 'D' : 'd';
    *p++ = td_is_weekly(&tdr) ? 'W' : 'w';
    *p++ = '\0';
    if (so_cto) cli_out_timer_reply_entry("f", buf, 0);

    if (td_is_daily(&tdr)) {
      if (so_cto) cli_out_timer_reply_entry("daily", tdr.daily, 0);
    }
    if (td_is_weekly(&tdr)) {
      if (so_cto) cli_out_timer_reply_entry("weekly", tdr.weekly, 0);
    }
    if (td_is_astro(&tdr)) {
      if (so_cto) cli_out_timer_reply_entry("astro", itoa(tdr.astro, buf, 10), 0);
    }

    if (td_is_random(&tdr)) {
      if (so_cto) cli_out_timer_reply_entry("random", "1", 0);
    }
    if (td_is_sun_auto(&tdr)) {
      if (so_cto) cli_out_timer_reply_entry("sun-auto", "1", 0);
    }

  }
  if (so_cto) cli_out_timer_reply_entry(NULL, NULL, -1);
}

void  so_timer_to_json(u8 g, u8 m, bool wildcard) {
  timer_data_t tdr;

  u8 g_res = g, m_res = m;


  if (read_timer_data(&tdr, &g_res, &m_res, wildcard)) {

    {
      char dict[] = "autoGM";
      dict[4] = '0' + g;
      dict[5] = '0' + m;
      sj_add_object(dict);
    }

    {
      extern gm_bitmask_t manual_bits; //FIXME
      bool f_manual = GET_BIT(manual_bits[g], m);
      char flags[10], *p = flags;
      *p++ = f_manual ? 'M' : 'm';
      *p++ = td_is_random(&tdr) ? 'R' : 'r';
      *p++ = td_is_sun_auto(&tdr) ? 'S' : 's';
      *p++ = td_is_astro(&tdr) ? 'A' : 'a';
      *p++ = td_is_daily(&tdr) ? 'D' : 'd';
      *p++ = td_is_weekly(&tdr) ? 'W' : 'w';
      *p++ = '\0';
      sj_add_key_value_pair_s("f", flags);
    }


    if (td_is_daily(&tdr)) {
      sj_add_key_value_pair_s("daily", tdr.daily);
    }

    if (td_is_weekly(&tdr)) {
      sj_add_key_value_pair_s("weekly", tdr.weekly);
    }

    if (td_is_astro(&tdr)) {
      sj_add_key_value_pair_d("astro", tdr.astro);

      timer_minutes_t tmi;
      get_timer_minutes(&tmi, &g_res, &m_res, false);
      sj_add_key_value_pair_d("asmin", tmi.minutes[ASTRO_MINTS]);
    }

      sj_close_object();
  }
}



static void  so_print_timer(u8 g, u8 m) {

  if (so_tgt_test(SO_TGT_CLI) && so_cco)
    so_print_timer_as_text(g, m, true);

#ifdef USE_JSON
  if ((so_tgt_test(SO_TGT_CLI) && so_jco) || so_tgt_test(SO_TGT_MQTT|SO_TGT_HTTP)) {
    so_timer_to_json(g, m, true);
    const char *json = sj_get_json();

#ifdef USE_MQTT
    if (so_tgt_test(SO_TGT_MQTT))
      io_mqtt_publish("tfmcu/timer_out", json);
#endif
    cli_print_json(json);
  }
#endif


}

static void  so_print_gmbitmask(gm_bitmask_t mm) {
  u8 g;

  for (g = 0; g < 8; ++g) {
    io_putx8(mm[g]);
    if (g < 7)
      io_putc(',');
  }
}

static void so_gmbitmask_to_str(char *dst, gm_bitmask_t mm) {
  i8 g;
  bool leading_zeros = true;

  for (g = 7; g >= 0; --g) {
    if (leading_zeros && mm[g] == 0)
      continue; // no leading zeros
    leading_zeros = false;

    if (mm[g] & 0xf0) {
      itoa(mm[g], dst, 16);
    } else {
      *dst++ = '0';
      itoa(mm[g], dst, 16);
    }
    dst = dst + strlen(dst);
  }
  if (leading_zeros) {
    *dst++ = '0';
    *dst++ = '0';
  }
}

static void 
so_print_startup_info(void) {
  static const char msg_starting[] = "\n\n" "tf: info: start: tronferno-mcu\n" "tf: info: build-date: " __DATE__ " " __TIME__ "\n";
  static const char msg_hint[] = "tf: hint: type 'help;' to get a command list\n";

  if (C.app_verboseOutput >= vrbNone) {
    io_puts(msg_starting);
    io_puts(msg_hint);
  }
}
