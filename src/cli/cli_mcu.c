#include "user_config.h"

#include <string.h>


#include "userio/inout.h"
#include "userio/status_output.h"
#include "user_config.h"
#include "positions/current_state.h"
#include "automatic/timer_state.h"
#include "setup/set_endpos.h"
#include "misc/bcd.h"
#include "main/rtc.h"
#include "cli_imp.h"

const char help_parmMcu[] = "print=(rtc|cu|reset-info)\n"
#if ENABLE_SPIFFS
    "spiffs=(format|test)\n"
#endif
#ifdef CONFIG_GPIO_SIZE
    "gpioN=(0|1|t|?) clear, set, toggle or read GPIO pin N\n"
#endif
        "up-time=?\n"
        "version=full\n";

int ICACHE_FLASH_ATTR
process_parmMcu(clpar p[], int len) {
  int arg_idx;
  char buf[24];

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL || val == NULL) {
      return -1;
    } else if (strcmp(key, "print") == 0) {
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
        so_arg_gm_t gm;
        gm.g = val[0] - '0';
        gm.m = val[1] - '0';
        so_output_message(SO_TIMER_EVENT_PRINT, &gm);
      }

    } else if (strcmp(key, "te") == 0) {
      int i;

      timer_event_t teu, ted;
      get_next_timer_event(&teu, &ted);
      io_putd(teu.next_event), io_putlf();
      for (i = 0; i < 8; ++i) {
        io_print_hex_8(teu.matching_members[i], true);
      }
      io_putlf();

      io_putd(ted.next_event), io_putlf();
      for (i = 0; i < 8; ++i) {
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
            cli_out_mcu_reply_entry(key, (ps_result == PIN_SET ? "1" : "0"), 0);
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
      if (*val == '?') {
        cli_out_mcu_reply_entry(key, ltoa(run_time(), buf, 10), 0);
      } else {
        reply_message("error:mcu:up-time", "option is read-only");
      }

    } else if (strcmp(key, "version") == 0) {

      cli_out_mcu_reply_entry("chip", MCU_TYPE, 0);

      cli_out_mcu_reply_entry("firmware", strcat(strcpy(buf, "tronferno-mcu-"), APP_VERSION), 0);

      char *p = strcpy(buf, ISO_BUILD_TIME);
      do
        if (*p == ' ')
          *p = '-';
      while (*++p);
      cli_out_mcu_reply_entry("build-time", buf, 0);

    } else {
      warning_unknown_option(key);
    }

  }

  cli_out_mcu_reply_entry(NULL, NULL, -1);

  return 0;
}