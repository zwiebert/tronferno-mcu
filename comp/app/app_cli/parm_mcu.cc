#include "app/config/proj_app_cfg.h"

#include <string.h>

#include "app/firmware.h"
#include "fernotron/sep/set_endpos.h"
#include "fernotron/pos/shutter_pct.h"
#include <fernotron/alias/pairings.h>
#include "txtio/inout.h"
#include "gpio/pin.h"
#include "app/uout/status_output.h"
#include <app/uout/so_msg.h>
#include "fernotron/auto/fau_tevent.h"
#include "key_value_store/kvs_wrapper.h"
#include "misc/bcd.h"
#include "app/rtc.h"
#include "cli_imp.h"
#include "app/opt_map.hh"

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



static void kvs_print_keys(const char *name_space);

#define is_kt(k) (kt == otok:: k)
#define is_key(k) (strcmp(key, k) == 0)
#define is_val(k) (strcmp(val, k) == 0)

//TODO: add IP address query option
int process_parmMcu(clpar p[], int len, const struct TargetDesc &td) {
  int arg_idx;

  so_object<void> soObj(soMsg_mcu_begin, soMsg_mcu_end, td);

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL || val == NULL)
      return -1;

    otok kt = optMap_findToken(key);

    if (is_kt(boot_count)) {
      if (is_val("?")) {
        soMsg_mcu_boot_count(td);
      } else if (strcmp("0", val) == 0) {
        // TODO: reset boot counter
      }
    } else if (is_kt(print)) {
#ifdef MCU_ESP8266
      if (is_val("reset-info")) {
        print_reset_info();
      } else  if (is_val("mem-info")) {
        void es_io_putc(char c);
        os_install_putc1(es_io_putc);
        system_set_os_print(1);
        system_print_meminfo();
      }
#endif
#ifndef NO_SPIFFS
    } else if (is_kt(spiffs)) {

      if (is_val("format")) {
        spiffs_format_fs(fs_A);
      } else if (is_val("test")) {
        spiffs_test();
      }
#endif
    } else if (is_kt(kvs_pk)) {

      kvs_print_keys(val);

    } else if (is_kt(tm)) {

      if (strlen(val) == 2) {
        so_arg_gm_t gm;
        gm.g = val[0] - '0';
        gm.m = val[1] - '0';
        soMsg_timer_event_print(td, gm);
      }
    } else if (is_kt(am)) {
      if (strlen(val) == 2) {
        u8 g = val[0] - '0';
        u8 m = val[1] - '0';
        Fer_TimerMinutes tmi;
        if (fer_au_get_timer_minutes_now(&tmi, &g, &m, true)) {
          soMsg_astro_minutes_print(td, tmi.minutes[ASTRO_MINTS]);
        }
      }
#ifdef USE_FREERTOS
    } else if (is_kt(stack)) {
      int words = uxTaskGetStackHighWaterMark(NULL);
      ets_printf("Stack HighWaterMark: %d bytes\n b", words * 4);
#endif
    } else if (is_kt(te)) {
      int i,k;

      Fer_TimerEvent tevt;
      time_t now_time = time(NULL);
      fer_am_get_next_timer_event(&tevt, &now_time);
      io_putd(tevt.next_event), io_putlf();
      for (k = 0; k < 2; ++k) {
        for (i = 0; i < 8; ++i) {
          io_print_hex_8(tevt.member_mask[k][i], true);
        }
        io_putlf();
      }
#ifdef USE_PAIRINGS
    } else if (is_kt(dbp)) {
      fer_alias_so_output_all_pairings(td);
#endif
    } else if (is_kt(cs)) {
      fer_statPos_printAllPcts(td);
#ifdef ACCESS_GPIO
    } else if (strncmp(key, "gpio", 4) == 0) {
      int gpio_number = atoi(key + 4);


      if (!is_gpio_number_usable(gpio_number, true)) {
        reply_message(td, "gpio:error", "gpio number cannot be used");
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
            soMsg_gpio_pin(td, so_arg_pch_t {gpio_number, ps_result});
          }
          break;

          default:
          error = "invalid command";

        }

        if (error) {
          reply_message(td, "gpio:failure", error);
          return -1;
        }
      }
#endif

    } else if (is_kt(up_time)) {
      if (is_val("?")) {
        soMsg_mcu_run_time(td);
      } else {
        reply_message(td, "error:mcu:up-time", "option is read-only");
      }

    } else if (is_kt(version)) {
      soMsg_mcu_version(td);
#ifdef USE_OTA
    } else if (is_kt(ota)) {
      if (is_val("?")) {
        soMsg_mcu_ota_state(td);
      } else if (is_val("github-master")) {
        soMsg_mcu_ota(td, OTA_FWURL_MASTER);
        ota_doUpdate(OTA_FWURL_MASTER, ca_cert_pem);
      } else if (is_val("github-beta")) {
        soMsg_mcu_ota(td, OTA_FWURL_BETA);
        ota_doUpdate(OTA_FWURL_BETA, ca_cert_pem);
      } else if (0 == strncmp(val, OTA_FWURL_TAG_COOKIE, strlen(OTA_FWURL_TAG_COOKIE))) {
        const char *tag = val + strlen(OTA_FWURL_TAG_COOKIE);
        const size_t url_len = strlen(OTA_FWURL_TAG_HEAD) + strlen(OTA_FWURL_TAG_TAIL) + strlen(tag);
        char url[url_len + 1];
        csu_copy_cat(url, url_len, OTA_FWURL_TAG_HEAD, tag, OTA_FWURL_TAG_TAIL);
        ota_doUpdate(url, ca_cert_pem);
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
      cli_warning_optionUnknown(td, key);
    }

  }

  //so_output_message(error_count ? SO_STATUS_ERROR : SO_STATUS_OK, 0);

  return 0;
}

static kvs_cbrT kvs_print_keys_cb(const char *key, kvs_type_t type, void *args) {
  io_printf("key: %s, type: %d\n", key, (int)type);
  return kvsCb_match;
}

static void kvs_print_keys(const char *name_space) {
  kvs_foreach(name_space, KVS_TYPE_ANY, 0, kvs_print_keys_cb, 0);
}

