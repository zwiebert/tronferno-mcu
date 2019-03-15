/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */
#include "user_config.h"

#include "automatic/timer_data.h"
#include "userio/status_output.h"

#include <string.h>

#include "automatic/timer_state.h"
#include "cli/cli_imp.h" // FIXME?
#include "config/config.h"
#include "main/common.h"
#include "main/pairings.h"
#include "main/rtc.h"
#include "misc/int_macros.h"
#include "userio/inout.h"

#define D(x)

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
    for (i = SO_CFG_begin; i <= SO_CFG_end; ++i) {
      so_output_message(i, NULL);
    }
  }
    break;

  case SO_CFG_BAUD:
    cli_out_config_reply_entry("baud", ltoa(C.mcu_serialBaud, buf, 10), 0);
    break;
  case SO_CFG_RTC:
    if (rtc_get_by_string(buf)) {
      cli_out_config_reply_entry("rtc", buf, 0);
    }
    break;
  case SO_CFG_CU:
    cli_out_config_reply_entry("cu", ltoa(C.fer_centralUnitID, buf, 16), 0);
    break;
  case SO_CFG_WLAN_SSID:
    cli_out_config_reply_entry("wlan-ssid", C.wifi_SSID, 0);
    break;
  case SO_CFG_LONGITUDE:
    cli_out_config_reply_entry("longitude", NULL, 8), io_print_float(C.geo_longitude, 5);
    break;
  case SO_CFG_LATITUDE:
    cli_out_config_reply_entry("latitude", NULL, 8), io_print_float(C.geo_latitude, 5);
    break;
  case SO_CFG_TIMEZONE:
    cli_out_config_reply_entry("time-zone", NULL, 8), io_print_float(C.geo_timezone, 5);
    break;

  case SO_CFG_VERBOSE:
    cli_out_config_reply_entry("verbose", ltoa(C.app_verboseOutput, buf, 10), 0);
    break;
#if POSIX_TIME
    case SO_CFG_TZ:
    cli_out_config_reply_entry("tz", C.geo_tz, 0);
    break;
#else
  case SO_CFG_DST: {
    const char *dst = (C.geo_dST == dstEU ? "eu" : (C.geo_dST == dstNone ? "0" : "1"));
    cli_out_config_reply_entry("dst", dst, 0);
  }
    break;
#endif
#ifdef CONFIG_GPIO_SIZE
    case SO_CFG_GPIO_PIN: {
      int gpio_number = *(int *)arg;
      char key[10] = "gpio";
      strcat(key, itoa(gpio_number, buf, 10));
      buf[0] = pin_state_args[C.gpio[gpio_number]]; buf[1] = '\0';
      cli_out_config_reply_entry(key, buf, 0);
    }
    break;
#endif

  case SO_CFG_begin:
    break;

  case SO_CFG_end:
    cli_out_config_reply_entry(NULL, NULL, -1);
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
  }
    break;

    /////////////////////////////////////////////////////////////////////////////////


  case SO_POS_PRINT_GMP: {
    so_arg_gmp_t *a = arg;
    io_puts("A:position:");
    io_puts(" g="), io_putd(a->g);
    io_puts(" m="), io_putd(a->m);
    io_puts(" p="), io_putd(a->p), io_puts(";\n");
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

  default:
    io_puts("error:so_output_message() unhandled message: "), io_putd(mt), io_putlf();
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
  extern gm_bitmask_t manual_bits; //FIXME
  // read_gm_bitmask("MANU", &manual_bits, 1); //FIXME: not needed
  bool f_manual = GET_BIT(manual_bits[g], m);

  cli_out_timer_reply_entry(NULL, NULL, 1);

  if (read_timer_data(&tdr, &g, &m, wildcard)) {

    cli_out_timer_reply_entry("g", itoa(g, buf, 10), 0);
    cli_out_timer_reply_entry("m", itoa(m, buf, 10), 0);

    char *p = buf;
    *p++ = f_manual ? 'M' : 'm';
    *p++ = td_is_random(&tdr) ? 'R' : 'r';
    *p++ = td_is_sun_auto(&tdr) ? 'S' : 's';
    *p++ = td_is_astro(&tdr) ? 'A' : 'a';
    *p++ = td_is_daily(&tdr) ? 'D' : 'd';
    *p++ = td_is_weekly(&tdr) ? 'W' : 'w';
    *p++ = '\0';
    cli_out_timer_reply_entry("f", buf, 0);

    if (td_is_daily(&tdr)) {
      cli_out_timer_reply_entry("daily", tdr.daily, 0);
    }
    if (td_is_weekly(&tdr)) {
      cli_out_timer_reply_entry("weekly", tdr.weekly, 0);
    }
    if (td_is_astro(&tdr)) {
      cli_out_timer_reply_entry("astro", itoa(tdr.astro, buf, 10), 0);
    }

    if (td_is_random(&tdr)) {
      cli_out_timer_reply_entry("random", "1", 0);
    }
    if (td_is_sun_auto(&tdr)) {
      cli_out_timer_reply_entry("sun-auto", "1", 0);
    }

  }
  cli_out_timer_reply_entry(NULL, NULL, -1);
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

