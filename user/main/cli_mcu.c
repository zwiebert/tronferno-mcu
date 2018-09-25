#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../user_config.h"
#include "current_state.h"
#include "timer_state.h"

#include "all.h"
#include "set_endpos.h"
#include "misc/bcd.h"
#include "cli_imp.h"


const char help_parmMcu[] PROGMEM =
"print=(rtc|cu|reset-info)\n"
#if ENABLE_SPIFFS
    "spiffs=(format|test)\n"
#endif
#ifdef CONFIG_GPIO_SIZE
    "gpioN=(0|1|t|?) clear, set, toggle or read GPIO pin N\n"
#endif
    "up-time=?\n"
    "version=full\n"
;

/*static*/ int ICACHE_FLASH_ATTR
process_parmMcu(clpar p[], int len) {
  int arg_idx;
  const char *mcuSep = "mcu ";

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL || val == NULL) {
      return -1;
    } else if (strcmp(key, "print") == 0) {

      if (strcmp(val, "rtc") == 0) {
        char buf[20];
        if (rtc_get_by_string(buf)) {
          io_puts(buf);
          io_putlf();
        }
      } else if (strcmp(val, "cu") == 0) {
        io_puts("cu=0x");
        io_putl(C.fer_centralUnitID, 16);
        io_putlf();
      }
#ifdef MCU_ESP8266
      if (strcmp(val, "reset-info") == 0) {
        print_reset_info();
      }
#endif
#if ENABLE_SPIFFS
    } else if (strcmp(key, "spiffs") == 0) {

      if (strcmp(val, "format") == 0) {
        spiffs_format_fs(fs_A);
      } else if (strcmp(val, "test") == 0) {
        spiffs_test();
      }
#endif
    } else if (strcmp(key, "tm") == 0) {

      if (strlen(val) == 2) {
	uint8_t g = val[0] - '0';
	uint8_t m = val[1] - '0';
	timer_minutes_t tm;
	if (get_timer_minutes(&tm, &g, &m, true)) {
	  int i;
	  for (i=0; i < SIZE_MINTS; ++i) {
	    io_putd(tm.minutes[i]), io_putlf();
	  }
	}
      }

    } else if (strcmp(key, "te") == 0) {
      int i;

      timer_event_t teu, ted;
      get_next_timer_event(&teu, &ted);
      io_putd(teu.next_event), io_putlf();
      for (i=0; i < 8; ++i) {
	io_print_hex_8(teu.matching_members[i], true);
      }
      io_putlf();

      io_putd(ted.next_event), io_putlf();
      for (i=0; i < 8; ++i) {
	io_print_hex_8(ted.matching_members[i], true);
      }
      io_putlf();

    } else if (strcmp(key, "cs") == 0) {
      print_shutter_positions();

#ifdef CONFIG_GPIO_SIZE
    } else if (strncmp(key, "gpio", 4) == 0) {
      int gpio_number = atoi(key + 4);
      mcu_pin_state ps = 0, ps_result = 0;

      if (!is_gpio_number_usable(gpio_number, true)) {
        reply_message("gpio:error", "gpio number cannot be used");
        return -1;
      } else {

        const char *error = NULL;
        for (ps = 0; pin_state_args[ps] != 0; ++ps) {
          if (pin_state_args[ps] == *val) {
            break;
          }
        }

        switch (ps) {

        case PIN_CLEAR:
        case PIN_SET:
        case PIN_TOGGLE:
          error = mcu_access_pin(gpio_number, NULL, ps);
          break;

        case PIN_READ:
          error = mcu_access_pin(gpio_number, &ps_result, ps);
          if (!error) {
            io_puts(mcuSep), io_puts(key), io_puts((ps_result == PIN_SET ? "=1" : "=0")), (mcuSep = " ");
          }
          break;

        default:
          error = "invalid command";

        }

        if (error) {
          reply_message("gpio:failure", error);
          return -1;
        }
      }
#endif

    } else if (strcmp(key, "up-time") == 0) {
      if (*val=='?') {
        io_puts(mcuSep), io_puts(key), io_puts("="), io_print_dec_32(run_time(), false), (mcuSep = " ");
      } else {
        reply_message("error:mcu:up-time", "option is read-only");
      }

    } else if (strcmp(key, "version") == 0) {
	static const char mcu_type[] PROGMEM = MCU_TYPE;
	static const char firmware_version[] PROGMEM = APP_VERSION;

	if (strcmp(val, "full") == 0) {
	  io_puts(mcuSep), io_puts(key), io_puts("="), io_puts_P(mcu_type), io_puts(","), io_puts_P(firmware_version), io_puts(", "),
	    io_puts(ISO_BUILD_TIME), (mcuSep = " ");
      }


    } else {
      warning_unknown_option(key);
    }

  }

  if (strcmp(mcuSep, " ") == 0)
    io_puts(";\n");

  return 0;
}
