
#include "app_config/proj_app_cfg.h"
#include "so_out.h"

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

#include "misc/int_macros.h"
#include "misc/ftoa.h"
#include <string.h>
#include <stdio.h>



void print_timer_event_minute(const char *label, minutes_t mins) {
  io_puts(label);
  if (mins == MINUTES_DISABLED)
    io_puts("=none\n");
  else
    io_putc('='), io_putd(mins), io_putlf();
}

void  so_print_timer_event_minutes(u8 g, u8 m) {
  timer_minutes_t tm;
  if (get_timer_minutes(&tm, &g, &m, true)) {
    print_timer_event_minute("astro-down", tm.minutes[0]);
    print_timer_event_minute("daily-up", tm.minutes[1]);
    print_timer_event_minute("daily-down", tm.minutes[2]);
    print_timer_event_minute("weekly-up", tm.minutes[3]);
    print_timer_event_minute("weekly-down", tm.minutes[4]);
  }
}



void  so_print_timer_as_text(u8 g, u8 m, bool wildcard) {
  timer_data_t tdr;
  char buf[10];
  u8 g_res = g, m_res = m;
  // read_gm_bitmask("MANU", &manual_bits, 1); //FIXME: not needed
  bool f_manual = GET_BIT(manual_bits[g], m);

  if (so_cco) cli_out_timer_reply_entry(NULL, NULL, 1);

  if (read_timer_data(&tdr, &g_res, &m_res, wildcard)) {


    if (so_cco) cli_out_timer_reply_entry("g", itoa(g, buf, 10), 0);
    if (so_cco) cli_out_timer_reply_entry("m", itoa(m, buf, 10), 0);

    char *p = buf;
    *p++ = f_manual ? 'M' : 'm';
    *p++ = td_is_random(&tdr) ? 'R' : 'r';
    *p++ = td_is_sun_auto(&tdr) ? 'S' : 's';
    *p++ = td_is_astro(&tdr) ? 'A' : 'a';
    *p++ = td_is_daily(&tdr) ? 'D' : 'd';
    *p++ = td_is_weekly(&tdr) ? 'W' : 'w';
    *p++ = '\0';
    if (so_cco) cli_out_timer_reply_entry("f", buf, 0);

    if (td_is_daily(&tdr)) {
      if (so_cco) cli_out_timer_reply_entry("daily", tdr.daily, 0);
    }
    if (td_is_weekly(&tdr)) {
      if (so_cco) cli_out_timer_reply_entry("weekly", tdr.weekly, 0);
    }
    if (td_is_astro(&tdr)) {
      if (so_cco) cli_out_timer_reply_entry("astro", itoa(tdr.astro, buf, 10), 0);
    }

    if (td_is_random(&tdr)) {
      if (so_cco) cli_out_timer_reply_entry("random", "1", 0);
    }
    if (td_is_sun_auto(&tdr)) {
      if (so_cco) cli_out_timer_reply_entry("sun-auto", "1", 0);
    }

  }
  if (so_cco) cli_out_timer_reply_entry(NULL, NULL, -1);
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



void  so_print_timer(u8 g, u8 m) {

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

void  so_print_gmbitmask(gm_bitmask_t *mm) {
  u8 g;

  for (g = 0; g < 8; ++g) {
    io_putx8(gm_GetByte(mm, g));
    if (g < 7)
      io_putc(',');
  }
}

void so_gmbitmask_to_str(char *dst, gm_bitmask_t *mm) {
  i8 g;
  bool leading_zeros = true;

  for (g = 7; g >= 0; --g) {
    if (leading_zeros && gm_GetByte(mm, g) == 0)
      continue; // no leading zeros
    leading_zeros = false;

    if (gm_GetByte(mm, g) & 0xf0) {
      itoa(gm_GetByte(mm, g), dst, 16);
    } else {
      *dst++ = '0';
      itoa(gm_GetByte(mm, g), dst, 16);
    }
    dst = dst + strlen(dst);
  }
  if (leading_zeros) {
    *dst++ = '0';
    *dst++ = '0';
  }
}

void
so_print_startup_info(void) {
  static const char msg_starting[] = "\n\n" "tf: info: start: tronferno-mcu\n" "tf: info: build-date: " __DATE__ " " __TIME__ "\n";
  static const char msg_hint[] = "tf: hint: type 'help;' to get a command list\n";

  if (C.app_verboseOutput >= vrbNone) {
    io_puts(msg_starting);
    io_puts(msg_hint);
  }
}
