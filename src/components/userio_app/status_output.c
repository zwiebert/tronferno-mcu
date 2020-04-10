/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "so_out.h"
#include "so_print.h"

#include "app/common.h"
#include "app/ota.h"
#include "app/rtc.h"
#include "cli_app/cli_config.h"
#include "cli_app/cli_fer.h"
#include "cli_app/cli_imp.h" // FIXME?
#include "config/config.h"
#include "fernotron_alias/pairings.h"
#include "fernotron_auto/timer_data.h"
#include "fernotron_auto/timer_state.h"
#include "fernotron_cuas/cuid_auto_set.h"
#include "fernotron_pos/shutter_prefs.h"
#include "net/ipnet.h"
#include "net/mqtt/app/mqtt.h"
#include "txtio/inout.h"
#include "userio/status_json.h"
#include "userio_app/status_output.h"

//#include "misc/int_macros.h"

#include <string.h>
#include <stdio.h>
#include <time.h>

#define D(x)



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
    so_out_x_reply_entry_sl("run-time", run_time_s());

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
    for (i = SO_CFG_begin + 1; i < SO_CFG_end; ++i) {
      so_output_message(i, NULL);
    }
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
    so_out_x_reply_entry_lx(mt, cfg_getCuId());
    break;

  case SO_CFG_NETWORK:
#ifdef USE_NETWORK
    so_out_x_reply_entry_s(mt, cfg_args_network[C.network]);
#endif
    break;

#ifdef USE_LAN
  case SO_CFG_LAN_PHY:
    so_out_x_reply_entry_s(mt, cfg_args_lanPhy[C.lan.phy]);
    break;
  case SO_CFG_LAN_PWR_GPIO:
    so_out_x_reply_entry_d(mt, C.lan.pwr_gpio);
    break;
#else
  case SO_CFG_LAN_PHY:
  case SO_CFG_LAN_PWR_GPIO:
    break;
#endif
#ifdef USE_WLAN
  case SO_CFG_WLAN_SSID:
    so_out_x_reply_entry_s(mt, C.wifi.SSID);
    break;
  case SO_CFG_WLAN_PASSWORD:
    so_out_x_reply_entry_s(mt, *C.wifi.password ? "*" : "");
    break;
#endif
#ifdef USE_NTP
  case SO_CFG_NTP_SERVER:
    so_out_x_reply_entry_s(mt, C.ntp.server);
    break;
#endif
#ifdef USE_MQTT
  case SO_CFG_MQTT_ENABLE:
    so_out_x_reply_entry_d(mt, C.mqtt.enable ? 1 : 0);
    break;
  case SO_CFG_MQTT_URL:
    so_out_x_reply_entry_s(mt, C.mqtt.url);
    break;
  case SO_CFG_MQTT_USER:
    so_out_x_reply_entry_s(mt, C.mqtt.user);
    break;
  case SO_CFG_MQTT_PASSWORD:
    so_out_x_reply_entry_s(mt, *C.mqtt.password ? "*" : "");
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
    so_out_x_reply_entry_d(mt, C.http.enable ? 1 : 0);
    break;
  case SO_CFG_HTTP_USER:
    so_out_x_reply_entry_s(mt, C.http.user);
    break;
  case SO_CFG_HTTP_PASSWORD:
    so_out_x_reply_entry_s(mt, *C.http.password ? "*" : "");
    break;
#else
  case SO_CFG_HTTP_ENABLE:
  case SO_CFG_HTTP_USER:
  case SO_CFG_HTTP_PASSWORD:
    break;
#endif

  case SO_CFG_LONGITUDE:
    so_out_x_reply_entry_f(mt, C.astro.geo_longitude, 5);
    break;
  case SO_CFG_LATITUDE:
    so_out_x_reply_entry_f(mt, C.astro.geo_latitude, 5);
    break;

  case SO_CFG_TIMEZONE:
#ifndef POSIX_TIME
    so_out_x_reply_entry_f(mt, C.astro.geo_timezone, 5);
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
      so_out_x_reply_entry_l(mt, C.astro.astroCorrection);
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
    time_t now_time = time(NULL);
    if (get_timer_minutes(&tmi, &g, &m, true, &now_time)) {
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
    so_out_x_reply_entry_sd("mvspdt", a->st->move_sundown_tsecs);
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

