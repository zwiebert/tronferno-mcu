

#include "so_out.h"

#include "utils_misc/int_types.h"
#include "app_misc/firmware.h"
#include "app_misc/rtc.h"

#include "app_settings/config.h"

#include "fernotron/auto/fau_tminutes.h"
#include "fernotron/auto/fau_tdata_store.h"
#include <fernotron/fer_pct.h>
#include "net/ipnet.h"
#include "txtio/inout.h"
#include "uout/uout_builder_json.hh"
#include "cli/cli_out.hh"
#include "app_uout/status_output.h"
#include <fernotron_uout/fer_uo_publish.h>

#include "utils_misc/int_macros.h"
#include "utils_misc/ftoa.h"
#include "utils_misc/itoa.h"
#include <string.h>
#include <stdio.h>
#include <time.h>


void print_timer_event_minute(const char *label, fer_au_minutesT mins) {
  io_puts(label);
  if (mins == MINUTES_DISABLED)
    io_puts("=none\n");
  else
    io_putc('='), io_putd(mins), io_putlf();
}

void  so_print_timer_event_minutes(uint8_t g, uint8_t m) {
  Fer_TimerMinutes tm;
  if (Fer_TimerData tid; fer_stor_timerData_load(&tid, &g, &m, true) && fer_au_get_timer_minutes_from_timer_data_tm(&tm, &tid)) {
    print_timer_event_minute("astro-down", tm.minutes[0]);
    print_timer_event_minute("daily-up", tm.minutes[1]);
    print_timer_event_minute("daily-down", tm.minutes[2]);
    print_timer_event_minute("weekly-up", tm.minutes[3]);
    print_timer_event_minute("weekly-down", tm.minutes[4]);
  }
}

void  so_print_gmbitmask(Fer_GmSet *mm) {
  uint8_t g;

  for (g = 0; g < 8; ++g) {
    io_putx8(mm->getGroup(g));
    if (g < 7)
      io_putc(',');
  }
}

void so_gmbitmask_to_str(char *dst, Fer_GmSet *mm) {
  int8_t g;
  bool leading_zeros = true;

  for (g = 7; g >= 0; --g) {
    if (leading_zeros && mm->getGroup(g) == 0)
      continue; // no leading zeros
    leading_zeros = false;

    if (mm->getGroup(g) & 0xf0) {
      itoa(mm->getGroup(g), dst, 16);
    } else {
      *dst++ = '0';
      itoa(mm->getGroup(g), dst, 16);
    }
    dst = dst + strlen(dst);
  }
  if (leading_zeros) {
    *dst++ = '0';
    *dst++ = '0';
  }
}
