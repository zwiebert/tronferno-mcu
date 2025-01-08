#include <string.h>

#include "app_misc/firmware.h"
#include "app_mqtt/mqtt.h"
#include  <app_settings/serialize.hh>
#include "fernotron/fer_main.h"
#include "fernotron/sep/set_endpos.h"
#include "fernotron/pos/shutter_pct.h"
#include <fernotron/alias/pairings.h>
#include "gpio/pin.h"
#include "app_uout/status_output.h"
#include <app_uout/so_msg.h>
#include "../app_uout/so_print.h"
#include <fernotron/auto/fau_tevent.hh>
#include "fernotron/auto/fau_tdata_store.h"
#include <fernotron_trx/raw/rf_capture.hh>
#include "kvs/kvs_wrapper.h"
#include "utils_misc/bcd.h"
#include "app_misc/rtc.h"
#include "cli_internal.hh"
#include "app_misc/opt_map.hh"
#include <app_settings/config.h>
#include <cc1101_ook/trx.hh>
#include "debug/dbg.h"

#define logtag "app_cli"

#ifdef TEST_HOST
#define io_printf printf
#else
#include <esp_system.h>
#endif

#include <alloca.h>

const char cli_help_parmMcu[] = "'mcu' handles special commands and data\n\n"
    "print=(rtc|cu|reset-info)\n"
#ifdef CONFIG_APP_GPIO_NUMBER_OF_GPIOS
    "gpioN=(0|1|t|?) clear, set, toggle or read GPIO pin N\n"
#endif
    "up-time=?\n"
    "version=full\n"
    "ota=(tag:beta|tag:master|tag:XXX|?)  Fetch and install firmware from github. ? gets status info\n"
    "cc1101-config=(?|HEX-STRING) Get or set configuration registers\n"
    "cc1101-status=?   Get status registers\n"

;

static void kvs_print_keys(class UoutWriter &td, const char *name_space);
void cc1101_printCfg_asString(class UoutWriter &td);

#define is_kt(k) (kt == otok::k_##k)
#define is_key(k) (strcmp(key, k) == 0)
#define is_val(k) (strcmp(val, k) == 0)

//TODO: add IP address query option
int process_parmMcu(clpar p[], int len, class UoutWriter &td) {
  int arg_idx;

  so_object<void> soObj(soMsg_mcu_begin, soMsg_mcu_end, td);

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL || val == NULL)
      return -1;

    otok kt = optMap_findToken(key);

    switch (kt) {
    case otok::k_boot_count: {
      if (is_val("?")) {
        soMsg_mcu_boot_count(td);
      } else if (strcmp("0", val) == 0) {
        if (kvshT h = kvs_open("misc", kvs_WRITE)) {
          kvs_erase_key(h, "boot_ct");
          kvs_commit(h);
          kvs_close(h);
        }
      }
    }
      break;

    case otok::k_print: {
    }
      break;

    case otok::k_tm: {
      if (strlen(val) == 2) {
        const uint8_t g = val[0] - '0';
        const uint8_t m = val[1] - '0';
        db_loge(logtag, "tm");
        td.so().print("test", "tm");
        so_print_timer_event_minutes(td, g, m);
      }
    }
      break;
    case otok::k_am: {
      if (strlen(val) == 2) {
        uint8_t g = val[0] - '0';
        uint8_t m = val[1] - '0';
        Fer_TimerMinutes tmi;
        if (Fer_TimerData tid; fer_stor_timerData_load(&tid, &g, &m, true) && fer_au_get_timer_minutes_from_timer_data_tm(&tmi, &tid)) {
          soMsg_astro_minutes_print(td, tmi.minutes[FER_MINTS_ASTRO]);
        }
      }
    }
      break;

    case otok::k_te: {
      Fer_TimerEvent tevt;
      time_t now_time = time(NULL);
      tevt.fer_am_get_next_timer_event(&now_time);
      td.so().print("next_event_minute", tevt.te_getEventMinute());
      for (int i = 0; i < 8; ++i) {
        printf("0x%02x, ", (*tevt.te_getMaskUp())[i]);
      }
      puts("");
      for (int i = 0; i < 8; ++i) {
        printf("0x%02x, ", (*tevt.te_getMaskDown())[i]);
      }
      puts("");
    }
      break;
#ifdef CONFIG_APP_USE_PAIRINGS
    case otok::k_dbp: {
      fer_alias_so_output_all_pairings(td);
    }
      break;
#endif
    case otok::k_cs: {
      fer_statPos_printAllPcts(td);

    }
      break;
    case otok::k_up_time: {
      if (is_val("?")) {
        soMsg_mcu_run_time(td);
      } else {
        reply_message(td, "error:mcu:up-time", "option is read-only");
      }

    }
      break;
    case otok::k_version: {
      soMsg_mcu_version(td);
    }
      break;
#ifdef CONFIG_APP_USE_OTA
    case otok::k_ota: {
      constexpr char TAG_KEY[] = "tag:";
      constexpr char URL_HEAD[] = CONFIG_APP_OTA_URL_HEAD;
      constexpr char URL_TAIL[] = CONFIG_APP_OTA_URL_TAIL;
      if (is_val("?")) {
        soMsg_mcu_ota_state(td);
      } else if (0 == strncmp(val, TAG_KEY, strlen(TAG_KEY))) {
        const char *tag = val + strlen(TAG_KEY);
        const size_t url_len = strlen(URL_HEAD) + strlen(URL_TAIL) + strlen(tag);
        char url[url_len + 1];
        csu_copy_cat(url, url_len, URL_HEAD, tag, URL_TAIL);
        app_doFirmwareUpdate(url);
      } else {
#ifdef DISTRIBUTION
        reply_message(td, "ota:failed", "ota update from URL is not allowed in release version");
#else
        app_doFirmwareUpdate(val);
#endif
      }
    }
      break;
#endif

    default:
      // Echo parameter to test quoting in command lines
      if (strcmp(key, "echo") == 0) {
        td.so().print("echo", val);
        break;
      }
#ifdef CONFIG_APP_USE_MQTT
      if (strcmp(key, "test1") == 0) {
        io_mqttApp_test1();
        break;
      }

      if (strcmp(key, "hs-hass-send-config") == 0) {
        if (*val)
          io_mqttApp_HassConfig(fer_usedMemberMask, false, val);
        else
          io_mqttApp_HassConfig(fer_usedMemberMask);
        break;
      }

      if (strcmp(key, "hs-hass-remove-config") == 0) {
        if (*val)
          io_mqttApp_HassConfig(fer_usedMemberMask, true, val);
        else
          io_mqttApp_HassConfig(fer_usedMemberMask, true);
        break;
      }
#endif

#ifdef CONFIG_APP_USE_GPIO_PINS
      if (strncmp(key, "gpio", 4) == 0) {
        int gpio_number = atoi(key + 4);

        const char *error = NULL;
        int psi;
        for (psi = 0; pin_state_args[psi] != 0; ++psi) {
          if (pin_state_args[psi] == *val) {
            break;
          }
        }
        mcu_pin_state ps = (mcu_pin_state) psi, ps_result = PIN_STATE_none;
        switch (ps) {

        case PIN_CLEAR:
        case PIN_SET:
        case PIN_TOGGLE: {
          if (!gpio_isLevelWritable(gpio_number)) {
            reply_message(td, "gpio:error", "gpio not writable");
            return -1;
          }
          error = mcu_access_pin(gpio_number, NULL, ps);
          break;

          case PIN_READ:
          {
            if (!gpio_isLevelReadable(gpio_number)) {
              reply_message(td, "gpio:error", "gpio not readable");
              return -1;
            }
            error = mcu_access_pin(gpio_number, &ps_result, ps);
            int8_t level = (ps_result == PIN_STATE_none) ? -1 : (ps_result == PIN_CLEAR) ? 0 : 1;
            soMsg_gpio_pin(td, so_arg_pch_t { uint8_t(gpio_number), level != 0, level });
          }
        }
          break;

        default:
          error = "invalid command";

        }

        if (error) {
          reply_message(td, "gpio:failure", error);
          return -1;
        }

        break;
      }
#endif

      if (strcmp(key, "test-sj") == 0) {
        auto ftd = UoutWriterFile("/spiffs/settings.json", SO_TGT_FLAG_JSON);
        if (ftd.sj().open_root_object("tfmcu")) {
          if (ftd.sj().add_object("backup")) {
            appSett_all_settings_to_json(ftd);
            ftd.sj().close_object();
          }
          ftd.sj().close_root_object();
        }
        ftd.sj().write_json(); // finish json
        break;
      }

      if (strcmp(key, "test-rj") == 0) {
        appSett_all_settings_to_json(td);
        break;
      }

      if (strcmp(key, "kvs-pk") == 0) {
        kvs_print_keys(td, val);
        break;
      }

#ifdef CONFIG_TRONFERNO_ENABLE_RF_CAPTURE
      if (strcmp(key, "rf-capture") == 0) {
        if (strcmp(val, "?") == 0) {
          uint8_t *buf;
          unsigned buf_size;
          if (rfCapture_get(&buf, &buf_size)) {
            td.sj().add_key("rf-capture");
            td.sj().add_array();
            for (unsigned i = 0; i < buf_size; ++i) {
              auto c = buf[i];
              td.sj().read_json_from_function([c](char *dst, size_t dst_size) -> int {
                return snprintf(dst, dst_size, "%u", c);
              });
            }
            td.sj().close_array();
          }
        } else {
          rfCapture_init(atoi(val));
        }
        break;
      }
#endif

      if (strcmp(key, "error-bits") == 0) {
        td.so().print("error-bits", tfmcu_error_bits, 10);
        break;
      }

      if (strcmp(key, "cc1101-config") == 0) {
        if (*val == '?') {
          soCfg_CC1101_CONFIG(td);
        } else {
          if (!cc1101_ook_updConfig_fromSparse(val))
            return -1;
        }
        break;
      }

      if (strcmp(key, "cc1101-status") == 0) {
        if (*val == '?') {
          uint8_t regFile[14];
          size_t regFileSize = sizeof regFile;
          if (cc1101_ook_dump_status(regFile, &regFileSize)) {
            char rs[regFileSize * 2 + 1];
            for (int i = 0; i < regFileSize; ++i) {
              sprintf(&rs[i * 2], "%02x", regFile[i]);
            }
            td.so().print("cc1101-status", rs);
          }

        }
        break;
      }

#ifndef TEST_HOST
      if (strcmp(key, "loglevel-get") == 0) {
        td.so().print("loglevel", (int) esp_log_level_get(val));
        break;
      }
      if (strcmp(key, "loglevel-none") == 0) {
        esp_log_level_set(val, ESP_LOG_NONE);
        td.so().print("loglevel", (int) esp_log_level_get(val));
        break;
      }
      if (strcmp(key, "loglevel-error") == 0) {
        esp_log_level_set(val, ESP_LOG_ERROR);
        td.so().print("loglevel", (int) esp_log_level_get(val));
        break;
      }
      if (strcmp(key, "loglevel-warn") == 0) {
        esp_log_level_set(val, ESP_LOG_WARN);
        td.so().print("loglevel", (int) esp_log_level_get(val));
        break;
      }
      if (strcmp(key, "loglevel-info") == 0) {
        esp_log_level_set(val, ESP_LOG_INFO);
        td.so().print("loglevel", (int) esp_log_level_get(val));
        break;
      }
      if (strcmp(key, "loglevel-debug") == 0) {
        esp_log_level_set(val, ESP_LOG_DEBUG);
        td.so().print("loglevel", (int) esp_log_level_get(val));
        break;
      }
      if (strcmp(key, "loglevel-verbose") == 0) {
        esp_log_level_set(val, ESP_LOG_VERBOSE);
        td.so().print("loglevel", (int) esp_log_level_get(val));
        break;
      }

      if (strcmp(key, "free-heap-size") == 0) {
        if (*val == '?') {
          const uint32_t fhs = esp_get_free_heap_size();
          td.so().print("free-heap-size", fhs);
        }
        break;
      }

      if (strcmp(key, "ipaddr") == 0) {
        if (*val == '?') {
          soMsg_inet_print_address(td);
        }
        break;
      }
#endif

      cli_warning_optionUnknown(td, key);
      break;
    } // switch
  } // for

  return 0;
}

static void kvs_print_keys(class UoutWriter &td, const char *name_space) {
  kvs_foreach(name_space, KVS_TYPE_ANY,

  /// match any key name
      [](const char*, int) -> bool {
        return true;
      },0,

      /// print each key
      [](const char *key, kvs_type_t type, class UoutWriter &td) -> kvs_cbrT {
        td.so().print(key, (int) type);
        return kvsCb_match;
      }, td);
}

