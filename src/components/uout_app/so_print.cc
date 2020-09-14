
#include "app_config/proj_app_cfg.h"
#include "so_out.h"

#include "app/common.h"
#include "app/firmware.h"
#include "app/rtc.h"

#include "config/app/config.h"

#include "fernotron/auto/fau_tminutes.h"
#include <fernotron/types.h>
#include "net/ipnet.h"
#include "txtio/inout.h"
#include "uout/status_json.h"
#include "uout/cli_out.h"
#include "uout_app/status_output.h"
#include <uout_app/callbacks.h>

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
  timer_minutes_t tm;
  if (fau_get_timer_minutes_now(&tm, &g, &m, true)) {
    print_timer_event_minute("astro-down", tm.minutes[0]);
    print_timer_event_minute("daily-up", tm.minutes[1]);
    print_timer_event_minute("daily-down", tm.minutes[2]);
    print_timer_event_minute("weekly-up", tm.minutes[3]);
    print_timer_event_minute("weekly-down", tm.minutes[4]);
  }
}

void  so_print_gmbitmask(GmBitMask *mm) {
  u8 g;

  for (g = 0; g < 8; ++g) {
    io_putx8(mm->getByte(g));
    if (g < 7)
      io_putc(',');
  }
}

void so_gmbitmask_to_str(char *dst, GmBitMask *mm) {
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

void so_print_startup_info(void) {
  static const char msg_starting[] = "\n\n" "tf: info: start: tronferno-mcu\n" "tf: info: build-date: " __DATE__ " " __TIME__ "\n";
  static const char msg_hint[] = "tf: hint: type 'help;' to get a command list\n";

  io_puts(msg_starting);
  io_puts(msg_hint);

}
