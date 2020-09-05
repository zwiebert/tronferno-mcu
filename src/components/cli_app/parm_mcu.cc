#include "app_config/proj_app_cfg.h"

#include <string.h>

#include "app/ota.h"
#include "fernotron/sep/set_endpos.h"
#include "fernotron/pos/shutter_pct.h"
#include "txtio/inout.h"
#include "gpio/pin.h"
#include "userio_app/status_output.h"
#include "fernotron/auto/fau_tevent.h"
#include "key_value_store/kvs_wrapper.h"
#include "misc/bcd.h"
#include "app/rtc.h"
#include "cli_imp.h"

#include "debug/dbg.h"
#ifdef USE_HTTP
#include "http_server_content/hts_clients.h"
#endif

#ifdef USE_FREERTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif
#ifdef MCU_ESP8266
#include "user_interface.h"
#ifndef NO_SPIFFS
#include "storage/esp8266/spiffs_fs.h"
#endif
#endif

#ifdef TEST_HOST
#define ets_printf printf
#endif


#include <alloca.h>

const char cli_help_parmMcu[] = "print=(rtc|cu|reset-info)\n"
#ifndef NO_SPIFFS
    "spiffs=(format|test)\n"
#endif
#ifdef CONFIG_GPIO_SIZE
    "gpioN=(0|1|t|?) clear, set, toggle or read GPIO pin N\n"
#endif
        "up-time=?\n"
        "version=full\n";

const char pin_state_args[] = "?01t";

static void kvs_print_keys(const char *name_space);

//TODO: add IP address query option
int process_parmMcu(clpar p[], int len) {
  int arg_idx;

  so_output_message(SO_MCU_begin, NULL);

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL || val == NULL) {
      return -1;
    } else if (strcmp(key, "boot-count") == 0) {
      if (*val == '?') {
        so_output_message(SO_MCU_BOOT_COUNT, 0);
      } else if (strcmp("0", val) == 0) {
        // TODO: reset boot counter
      }
    } else if (strcmp(key, "print") == 0) {
#ifdef MCU_ESP8266
      if (strcmp(val, "reset-info") == 0) {
        print_reset_info();
      } else  if (strcmp(val, "mem-info") == 0) {
        void es_io_putc(char c);
        os_install_putc1(es_io_putc);
        system_set_os_print(1);
        system_print_meminfo();
      }
#endif
#ifndef NO_SPIFFS
    } else if (strcmp(key, "spiffs") == 0) {

      if (strcmp(val, "format") == 0) {
        spiffs_format_fs(fs_A);
      } else if (strcmp(val, "test") == 0) {
        spiffs_test();
      }
#endif
    } else if (strcmp(key, "kvs-pk") == 0) {

      kvs_print_keys(val);

    } else if (strcmp(key, "tm") == 0) {

      if (strlen(val) == 2) {
        so_arg_gm_t gm;
        gm.g = val[0] - '0';
        gm.m = val[1] - '0';
        so_output_message(SO_TIMER_EVENT_PRINT, &gm);
      }
    } else if (strcmp(key, "am") == 0) {
      if (strlen(val) == 2) {
        so_arg_gm_t gm;
        gm.g = val[0] - '0';
        gm.m = val[1] - '0';
        so_output_message(SO_ASTRO_MINUTES_PRINT, &gm);
      }
#ifdef USE_FREERTOS
    } else if (strcmp(key, "stack") == 0) {
      int words = uxTaskGetStackHighWaterMark(NULL);
      ets_printf("Stack HighWaterMark: %d bytes\n b", words * 4);
#endif
    } else if (strcmp(key, "te") == 0) {
      int i,k;

      timer_event_t tevt;
      time_t now_time = time(NULL);
      fam_get_next_timer_event(&tevt, &now_time);
      io_putd(tevt.next_event), io_putlf();
      for (k = 0; k < 2; ++k) {
        for (i = 0; i < 8; ++i) {
          io_print_hex_8(tevt.member_mask[k][i], true);
        }
        io_putlf();
      }
#ifdef USE_PAIRINGS
    } else if (strcmp(key, "dbp") == 0) {
      bool pair_so_output_all_pairings(void);
      pair_so_output_all_pairings();
#endif
    } else if (strcmp(key, "cs") == 0) {
      statPos_printAllPcts();
#ifdef ACCESS_GPIO
    } else if (strncmp(key, "gpio", 4) == 0) {
      int gpio_number = atoi(key + 4);


      if (!is_gpio_number_usable(gpio_number, true)) {
        reply_message("gpio:error", "gpio number cannot be used");
        return -1;
      } else {

        const char *error = NULL;
        int psi;
        for (psi = 0; pin_state_args[psi] != 0; ++psi) {
          if (pin_state_args[psi] == *val) {
            break;
          }
        }
        mcu_pin_state ps = (mcu_pin_state)psi, ps_result = PIN_READ;
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
        so_output_message(SO_MCU_RUN_TIME, NULL);
      } else {
        reply_message("error:mcu:up-time", "option is read-only");
      }

    } else if (strcmp(key, "version") == 0) {
      so_output_message(SO_MCU_VERSION, NULL);
#ifdef USE_OTA
    } else if (strcmp(key, "ota") == 0) {
      if (*val == '?') {
        so_output_message(SO_MCU_OTA_STATE, 0);
      } else if (strcmp(val, "github-master") == 0) {
        so_output_message(SO_MCU_OTA, OTA_FWURL_MASTER);
        ota_doUpdate(OTA_FWURL_MASTER, ca_cert_pem);
      } else if (strcmp(val, "github-beta") == 0) {
        so_output_message(SO_MCU_OTA, OTA_FWURL_BETA);
        ota_doUpdate(OTA_FWURL_BETA, ca_cert_pem);
      } else if (0 == strncmp(val, OTA_FWURL_TAG_COOKIE, strlen(OTA_FWURL_TAG_COOKIE))) {
        const char *tag = val + strlen(OTA_FWURL_TAG_COOKIE);
        int url_len = strlen(OTA_FWURL_TAG_HEAD) + strlen(OTA_FWURL_TAG_TAIL) + strlen(tag);
        char *url;
        if ((url = (char*)alloca(url_len + 1))) {
          strcat(strcat(strcpy(url, OTA_FWURL_TAG_HEAD), tag), OTA_FWURL_TAG_TAIL);
          ota_doUpdate(url, ca_cert_pem);
        }
      } else {
#ifdef DISTRIBUTION
        ets_printf("forbidden: ota update from given URL\n");
#else
        ets_printf("doing ota update from given URL\n");
        ota_doUpdate(val, ca_cert_pem);
#endif
      }
#endif
    } else {
      cli_warning_optionUnknown(key);
    }

  }

  //so_output_message(error_count ? SO_STATUS_ERROR : SO_STATUS_OK, 0);
  so_output_message(SO_MCU_end, NULL);

  return 0;
}

static kvs_cbrT kvs_print_keys_cb(const char *key, kvs_type_t type, void *args) {
  io_printf("key: %s, type: %d\n", key, (int)type);
  return kvsCb_match;
}

static void kvs_print_keys(const char *name_space) {
  kvs_foreach(name_space, KVS_TYPE_ANY, 0, kvs_print_keys_cb, 0);
}

