
#include "app/config/proj_app_cfg.h"
#include "so_out.h"

#include "app/common.h"
#include "app/firmware.h"
#include "app/rtc.h"

#include "app/settings/config.h"

#include "fernotron/auto/fau_tminutes.h"
#include <fernotron/types.h>
#include "net/ipnet.h"
#include "txtio/inout.h"
#include "uout/status_json.hh"
#include "uout/cli_out.h"
#include "app/uout/status_output.h"
#include <app/uout/callbacks.h>

#include "misc/int_macros.h"
#include "misc/ftoa.h"
#include "misc/itoa.h"
#include <string.h>
#include <stdio.h>
#include <time.h>


void print_timer_event_minute(const char *label, minutes_t mins) {
  io_puts(label);
  if (mins == MINUTES_DISABLED)
    io_puts("=none\n");
  else
    io_putc('='), io_putd(mins), io_putlf();
}

void  so_print_timer_event_minutes(u8 g, u8 m) {
  Fer_TimerMinutes tm;
  if (fer_au_get_timer_minutes_now(&tm, &g, &m, true)) {
    print_timer_event_minute("astro-down", tm.minutes[0]);
    print_timer_event_minute("daily-up", tm.minutes[1]);
    print_timer_event_minute("daily-down", tm.minutes[2]);
    print_timer_event_minute("weekly-up", tm.minutes[3]);
    print_timer_event_minute("weekly-down", tm.minutes[4]);
  }
}

void  so_print_gmbitmask(Fer_GmBitMask *mm) {
  u8 g;

  for (g = 0; g < 8; ++g) {
    io_putx8(mm->getByte(g));
    if (g < 7)
      io_putc(',');
  }
}

void so_gmbitmask_to_str(char *dst, Fer_GmBitMask *mm) {
  i8 g;
  bool leading_zeros = true;

  for (g = 7; g >= 0; --g) {
    if (leading_zeros && mm->getByte(g) == 0)
      continue; // no leading zeros
    leading_zeros = false;

    if (mm->getByte(g) & 0xf0) {
      itoa(mm->getByte(g), dst, 16);
    } else {
      *dst++ = '0';
      itoa(mm->getByte(g), dst, 16);
    }
    dst = dst + strlen(dst);
  }
  if (leading_zeros) {
    *dst++ = '0';
    *dst++ = '0';
  }
}
