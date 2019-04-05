/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */
#include "user_config.h"

#include "automatic/timer_data.h"
#include "userio/status_output.h"
#if defined USE_MQTT || defined USE_HTTP
#include "userio/status_json.h"
#else
#define so_json_config_reply(a,b,c)
#define so_jco false
#endif
#define so_jto so_jco

#include <string.h>
#include <stdio.h>

#include "automatic/timer_state.h"
#include "cli/cli_imp.h" // FIXME?
#include "config/config.h"
#include "main/common.h"
#include "main/pairings.h"
#include "main/rtc.h"
#include "misc/int_macros.h"
#include "userio/inout.h"
#include "userio/mqtt.h"
#include "userio/status_json.h"
#include "userio/ipnet.h"

#define D(x)

const char * const cfg_keys[] = {
    "cu", "baud", "rtc", "wlan-ssid", "wlan-password", "longitude", "latitude",
    "timezone", "dst", "tz", "verbose",
    "mqtt-enable", "mqtt-url", "mqtt-user", "mqtt-password",
    "http-enable", "http-user", "http-password",
    "gm-used",
};

bool out_cli = true;
bool out_js = true;

#define so_cco out_cli
#define so_cto so_cco
#define so_jco out_js

char *ICACHE_FLASH_ATTR ftoa(float f, char *buf, int n) {
  int i;
  int32_t mult;
  uint32_t rop;
  int16_t lop = (int16_t) f;
  char *s = buf;

  itoa(lop, s, 10);
  strcat(s, ".");
  s += strlen(s);

  f -= lop;

  mult = (lop < 0) ? -1 : 1;

  for (i = 0; i < n; ++i)
    mult *= 10;

  rop = (uint32_t) (f * mult);
  ltoa(rop, s, 10);

  return buf;
}

so_msg_t ICACHE_FLASH_ATTR
so_parse_config_key(const char *k) {
  int i;
  for (i = 0; i < (sizeof cfg_keys / sizeof cfg_keys[0]); ++i) {
    if (0 == strcmp(k, cfg_keys[i]))
      return i + SO_CFG_begin + 1;
  }
  return SO_NONE;
}

static const char *
gk(so_msg_t so_key) {
  if (SO_CFG_begin < so_key && so_key < SO_CFG_end)
    return cfg_keys[so_key - (SO_CFG_begin + 1)];
  return NULL;
}

void so_out_config_reply_entry2(const char *key, const char *val) {
  if (so_cco) cli_out_config_reply_entry(key, val, 0);
}

void so_out_config_reply_entry(so_msg_t key, const char *val) {
  if (so_cco) so_out_config_reply_entry2(gk(key), val);
}

void so_out_config_reply_entry_s(so_msg_t key, const char *val) {
  if (so_cco) so_out_config_reply_entry2(gk(key), val);
  if (so_jco) so_json_config_reply(gk(key), val, false);
}

void so_out_config_reply_entry_d(so_msg_t key, int val) {
  char buf[20];
  itoa(val, buf, 10);
  if (so_cco) so_out_config_reply_entry2(gk(key), buf);
  if (so_jco) so_json_config_reply(gk(key), buf, true);
}

void so_out_config_reply_entry_l(so_msg_t key, int val) {
  char buf[20];
  ltoa(val, buf, 10);
  if (so_cco) so_out_config_reply_entry2(gk(key), buf);
  if (so_jco) so_json_config_reply(gk(key), buf, true);
}

void so_out_config_reply_entry_lx(so_msg_t key, int val) {
  char buf[20];
  ltoa(val, buf, 16);
  if (so_cco) so_out_config_reply_entry2(gk(key), buf);
  if (so_jco) so_json_config_reply(gk(key), buf, false); //no hex in jason. use string
}

void so_out_config_reply_entry_f(so_msg_t key, float val, int n) {
  char buf[20];
  ftoa(val, buf, 5);
  if (so_cco) so_out_config_reply_entry2(gk(key), buf);
  if (so_jco) so_json_config_reply(gk(key), buf, true);
}


static void so_print_timer_event_minutes(uint8_t g, uint8_t m);
static void so_print_timer(uint8_t g, uint8_t m, bool wildcard);
static void so_print_gmbitmask(gm_bitmask_t mm);
static void so_print_startup_info(void);

void ICACHE_FLASH_ATTR so_output_message(so_msg_t mt, void *arg) {
  static uint16_t pras_msgid, cuas_msgid;
  char buf[20];
  int i;

  switch (mt) {
  case SO_FW_START_MSG_PRINT:
    so_print_startup_info();
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
    if (arg) {
      const char *f;
      bool old_out_js = out_js, old_out_cli = out_cli;
      out_js = out_cli = false;
      for (f=arg; *f; ++f)
        switch(*f) {
        case 'j': out_js = true; break;
        case 'c': out_cli = true; break;
        }
      out_js = old_out_js; out_cli = old_out_cli;
    }
    for (i = SO_CFG_begin; i <= SO_CFG_end; ++i) {
      so_output_message(i, NULL);
    }
  }
    break;

  case SO_CFG_BAUD:
    so_out_config_reply_entry_l(mt, C.mcu_serialBaud);
    break;
  case SO_CFG_RTC:
    if (rtc_get_by_string(buf)) {
      so_out_config_reply_entry(mt, buf);
    }
    break;
  case SO_CFG_CU:
    so_out_config_reply_entry_lx(mt, C.fer_centralUnitID);
    break;
  case SO_CFG_WLAN_SSID:
    so_out_config_reply_entry_s(mt, C.wifi_SSID);
    break;
  case SO_CFG_WLAN_PASSWORD:
    so_out_config_reply_entry_s(mt, *C.wifi_password ? "*" : "");
    break;

#ifdef USE_MQTT
  case SO_CFG_MQTT_ENABLE:
    so_out_config_reply_entry_d(mt, C.mqtt_enable ? 1 : 0);
    break;
  case SO_CFG_MQTT_URL:
    so_out_config_reply_entry_s(mt, C.mqtt_url);
    break;
  case SO_CFG_MQTT_USER:
    so_out_config_reply_entry_s(mt, C.mqtt_user);
    break;
  case SO_CFG_MQTT_PASSWORD:
    so_out_config_reply_entry_s(mt, *C.mqtt_password ? "*" : "");
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
    so_out_config_reply_entry_d(mt, C.http_enable ? 1 : 0);
    break;
  case SO_CFG_HTTP_USER:
    so_out_config_reply_entry_s(mt, C.http_user);
    break;
  case SO_CFG_HTTP_PASSWORD:
    so_out_config_reply_entry_s(mt, *C.http_password ? "*" : "");
    break;
#else
  case SO_CFG_HTTP_ENABLE:
  case SO_CFG_HTTP_USER:
  case SO_CFG_HTTP_PASSWORD:
    break;
#endif

  case SO_CFG_LONGITUDE:
    so_out_config_reply_entry_f(mt, C.geo_longitude, 5);
    break;
  case SO_CFG_LATITUDE:
    so_out_config_reply_entry_f(mt, C.geo_latitude, 5);
    break;
  case SO_CFG_TIMEZONE:
    so_out_config_reply_entry_f(mt, C.geo_timezone, 5);
    break;
  case SO_CFG_VERBOSE:
    so_out_config_reply_entry_d(mt, C.app_verboseOutput);
    break;
    case SO_CFG_TZ:
#ifdef POSIX_TIME
      so_out_config_reply_entry_s(mt, C.geo_tz);
#endif
    break;
  case SO_CFG_DST:
#ifdef MDR_TIME
  {
    const char *dst = (C.geo_dST == dstEU ? "eu" : (C.geo_dST == dstNone ? "0" : "1"));
    so_out_config_reply_entry_s(mt, dst);
  }
#endif
    break;

  case SO_CFG_GM_USED: {
    so_out_config_reply_entry_lx(mt, C.fer_usedMembers);
  }
    break;

    case SO_CFG_GPIO_PIN:
#ifdef ACCESS_GPIO
      {
      int gpio_number = *(int *)arg;
      char key[10] = "gpio";
      strcat(key, itoa(gpio_number, buf, 10));
      buf[0] = pin_state_args[C.gpio[gpio_number]]; buf[1] = '\0';
      so_out_config_reply_entry2(key, buf);
    }
#endif
    break;

  case SO_CFG_begin:
    break;

  case SO_CFG_end:
    so_out_config_reply_entry_s(SO_NONE, NULL);
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
    cuas_msgid = *(uint16_t *) arg;
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

    /////////////////////////////////////////////////////////////////////////////////
  case SO_PRAS_START_LISTENING:
    pras_msgid = *(uint16_t *) arg;
    io_puts("U:pras: start listening at RF\n");
    break;
  case SO_PRAS_STOP_LISTENING:
    if (arg && *(uint16_t *) arg) {
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
    so_print_timer(a->g, a->m, true);

    if (so_jto) io_mqtt_publish("tfmcu/timer_out", sj_timer2json(a->g, a->m));
  }
    break;

    /////////////////////////////////////////////////////////////////////////////////


  case SO_POS_PRINT_GMP: {
    so_arg_gmp_t *a = arg;
    io_puts("A:position:");
    io_puts(" g="), io_putd(a->g);
    io_puts(" m="), io_putd(a->m);
    io_puts(" p="), io_putd(a->p), io_puts(";\n");
    sj_gmp2json(a);
    io_mqtt_publish_gmp(a);
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

  case SO_PAIR_PRINT_AMM: {
    so_arg_amm_t *a = arg;
    io_puts("pair a="), so_print_gmbitmask(a->mm), io_puts(";\n");
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

static void ICACHE_FLASH_ATTR so_print_timer_event_minutes(uint8_t g, uint8_t m) {
  timer_minutes_t tm;
  if (get_timer_minutes(&tm, &g, &m, true)) {
    print_timer_event_minute("astro-down", tm.minutes[0]);
    print_timer_event_minute("daily-up", tm.minutes[1]);
    print_timer_event_minute("daily-down", tm.minutes[2]);
    print_timer_event_minute("weekly-up", tm.minutes[3]);
    print_timer_event_minute("weekly-down", tm.minutes[4]);
  }
}

static void ICACHE_FLASH_ATTR so_print_timer(uint8_t g, uint8_t m, bool wildcard) {
  timer_data_t tdr;
  char buf[10];
  uint8_t g_res = g, m_res = m;
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

static void ICACHE_FLASH_ATTR so_print_gmbitmask(gm_bitmask_t mm) {
  uint8_t g;

  for (g = 0; g < 8; ++g) {
    io_putx8(mm[g]);
    if (g < 7)
      io_putc(',');
  }
}

static void ICACHE_FLASH_ATTR
so_print_startup_info(void) {
  static const char msg_starting[] = "\n\n" "tf: info: start: tronferno-mcu\n" "tf: info: build-date: " __DATE__ " " __TIME__ "\n";
  static const char msg_hint[] = "tf: hint: type 'help;' to get a command list\n";

  if (C.app_verboseOutput >= vrbNone) {
    io_puts(msg_starting);
    io_puts(msg_hint);
  }
}

