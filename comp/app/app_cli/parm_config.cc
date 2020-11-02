#include "app_config/proj_app_cfg.h"

#include "cli_config.h"
#include "app_misc/opt_map.hh"
#include <string.h>

#include "fernotron/sep/set_endpos.h"
#include "app_uout/status_output.h"
#include "fernotron/cuas/cuid_auto_set.h"
#include "app_config/proj_app_cfg.h"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include "app_misc/rtc.h"
#include "utils_misc/bcd.h"
#include "cli_imp.h"
#include "utils_misc/stof.h"
#include "utils_misc/cstring_utils.h"
#ifdef USE_MQTT
#include "app_mqtt/mqtt.h"
#endif
#ifdef USE_HTTP
#include "net_http_server/http_server_setup.h"
#endif
#ifdef USE_NTP
#include "net/ntp_client_setup.h"
#endif
#include "fernotron_trx/astro.h"
#include <fernotron_trx/fer_trx_c_api.h>

#include "utils_misc/int_types.h"
#include "utils_misc/int_types.h"
#include "gpio/pin.h"
#include <utils_misc/mutex.hh>

#include <stdlib.h>
#include <iterator>
#include <algorithm>

#define ENABLE_RESTART 1 // allow software reset

const char cli_help_parmConfig[] = "'config' sets or gets options. Use: config option=value ...; to set. Use: config option=? ...; to get, if supported\n\n"
    "cu=(ID|auto|?)     6-digit hex ID of Central-Unit. auto: capture ID\n"
    "rtc=(ISO_TIME|?)   set local time if NTP not working\n"
#ifndef MCU_ESP32
                                   "baud=(N|?)         serial baud rate\n"
#endif
#ifdef USE_WLAN
    "wlan-ssid=(SSID|?)\n"
    "wlan-password=PW\n"
#endif
#ifdef USE_NETWORK
    "network=(wlan|ap|lan) ap opens wlan access-point at 192.168.4.1 (restart required)\n"
#endif
#ifdef USE_NTP
    "ntp-server=(gateway|dhcp|IP4ADDR|NAME) default: gateway\n"
#endif
#ifdef USE_MQTT
    "mqtt-enable=(0|1)  enable MQTT\n"
    "mqtt-url=URL       broker/server URL (e.g. mqtt://192.168.1.42:7777)\n"
    "mqtt-user=NAME     user name on server\n"
    "mqtt-password=PW   user password on server\n"
    "mqtt-client-id=ID  default: tfmcu\n"
    "mqtt-root-topic=TOPIC default: tfmcu/\n"
#endif
#ifdef USE_HTTP
    "http-enable=(0|1)  enable HTTP\n"
    "http-user=NAME     user name on server\n"
    "http-password=PW   user password on server\n"
#endif
    "longitude=(DEG|?)\n"
    "latitude=(DEG|?)\n"
#ifdef MDR_TIME
                                   "time-zone=(N|?)    time zone hour offset\n"
                                   "dst=(eu|0|1|?)     daylight saving time\n"
#endif
#ifdef USE_POSIX_TIME
    "tz=(POSIX_TZ|?)    time zone for RTC/NTP\n"
#endif
    "astro-correction   modifies astro table: 0=average, 1=bright 2=dark\n"
    "verbose=(0..5|?)   diagnose output verbosity level\n"
    "set-pw=password    set a config password. if set every config commands needs the pw option\n"
    "pw=PW              example: config pw=my_passw dst=eu;\n"
#if ENABLE_RESTART
    "restart            restart MCU\n"
#endif
#ifdef USE_GPIO_PINS
                                   "gpioN=(d|i|o|O|q|Q|?)(h|l|m) Set gpio as pin for input (i) output (O), open drain output (o) or input/output (q,Q).\n"
                                   "                   Add suffix to input to enable pull up (h), pull down (l), pull up/down (m)\n"
                                   "                   Add suffix (h|l) to output to set the initial level high or low at restart\n"
                                   "                   Disable gpio (d)\n"
                                   "                   Example: config gpio19=ol gpio18=ih gpio20=qh gpio21=d;\n"
#endif
    "rf-tx-pin=N        RF output GPIO pin\n"
    "rf-rx-pin=N        RF input GPIO pin\n"
    "set-button-pin     Set-button input GPIO pin\n"
//  "set-expert-password=\n"
;

void (*mcu_restart_cb)();

const char *const*cfg_args[SO_CFG_size] = { };

#define isValid_optStr(cfg, new) true
#define set_optStr(v, cb) config_save_item_s(settings_get_kvsKey(cb), v)
#define set_optBlob(v, cb) config_save_item_b(settings_get_kvsKey(cb), &v, sizeof v)
#define set_optStr_ifValid set_optStr
#define set_opt(t, v, cb) (config_save_item_##t(settings_get_kvsKey(cb), v) && config_item_modified((cb)))
#define set_optN(t, v, cb) (config_save_item_n_##t(settings_get_kvsKey(cb), v) && config_item_modified((cb)))

#define has_changed() SET_BIT(*changed_mask, so_key)

#define is_kt(k) (kt == otok::k_##k)
#define is_key(k) (strcmp(key, k) == 0)
#define is_val(k) (strcmp(val, k) == 0)

static so_msg_t so_soMsg_from_otok(otok kt) {
  so_msg_t result = static_cast<so_msg_t>(SO_CFG_begin + 1 + static_cast<otokBaseT>(kt));
  if (result >= SO_CFG_last || result == SO_CFG_GPIO_MODES)
    return SO_NONE;
  return result;
}

struct SettData {
  const char *kvsKey;
  KvsType kvsType;
  StoreFun storeFun;
};

template<class Sett, typename CfgItem>
SettData settings_getData(const Sett &settings, CfgItem item) {
  SettData res { };
  res.kvsKey = settings.get_kvsKey(item);
  res.kvsType = settings.get_kvsType(item);
  res.storeFun = settings.get_storeFun(item);
  return res;
}

int process_parmConfig(clpar p[], int len, const struct TargetDesc &td) {
  // static RecMutex settings_mutex;
  // std::lock_guard<RecMutex> lock(settings_mutex);

  int arg_idx;
  int errors = 0;
  u32 changed_mask = 0;
  bool flag_isValid = 0;

#define hasChanged_ethernet (changed_mask & CMB_lan)
#define hasChanged_mqttClient (changed_mask & CBM_mqttClient)
#define hasChanged_httpServer (changed_mask & CBM_httpServer)
#define hasChanged_txtio (changed_mask & CBM_txtio)
#define hasChanged_geo (changed_mask & CBM_geo)
#define hasChanged_gpio (hasChanged_gpio2 || (changed_mask & CBM_gpio))
#define hasChanged_time (changed_mask & (BIT(CB_TZ)))

  bool hasChanged_gpio2 = false;
  {
    so_object<void> cfgObj(soCfg_begin, soCfg_end, td);

    for (int pass = 0; pass < 2; ++pass) {
      for (arg_idx = 1; arg_idx < len; ++arg_idx) {
        const char *key = p[arg_idx].key, *val = p[arg_idx].val;



        if (key == NULL || val == NULL) { // don't allow any default values in config
          return cli_replyFailure(td);
        }

        bool endsWithQuestionMark = csu_endsWith(val, "?");

        if (pass == 0 && endsWithQuestionMark)
          continue;
        if (pass == 1 && !endsWithQuestionMark)
          continue;

        otok kt = optMap_findToken(key);

        if (kt != otok::NONE) {
          if (endsWithQuestionMark) {

            switch (kt) {

            case otok::k_all: {
              if (is_val("?")) {
                soCfg_all(td);
              } else if (is_val("net?")) {
                soCfg_all_net(td);
              } else if (is_val("gpio?")) {
                soCfg_all_gpio(td);
              }
            }
              continue;

            case otok::k_rtc:
              soCfg_RTC(td);
              continue;

            case otok::k_cuas:
              soMsg_cuas_state(td, fer_cuas_getState());
              continue;

            default: {
              if (auto so_key = so_soMsg_from_otok(kt); so_key != SO_NONE) {
                if (auto fun = settings_get_soCfgFun(kt)) {
                  fun(td);
                  continue;
                }
              }
              break;
            }
            }
          }

          SettData settData { };
          if (auto item = comp_settings.get_item(kt); item != CBC_NONE) {
            settData = settings_getData(comp_settings, item);
            SET_BIT(changed_mask, item);
          } else if (auto appItem = app_settings.get_item(kt); appItem != CBA_NONE) {
            settData = settings_getData(app_settings, appItem);
            SET_BIT(changed_mask, appItem);
          }

          if (settData.storeFun == STF_direct) {
            switch (settData.kvsType) {
            case CBT_str:
              if (!config_save_item_s(settData.kvsKey, val))
                ++errors;
              continue;

            case CBT_i8:
              if (!config_save_item_i8(settData.kvsKey, val))
                ++errors;
              continue;

            case CBT_u32:
              if (!config_save_item_u32(settData.kvsKey, val))
                ++errors;
              continue;

            case CBT_f:
              if (!config_save_item_f(settData.kvsKey, val))
                ++errors;
              continue;

            default:
              break;
            }
          } else if (settData.storeFun == STF_direct_hex) {
            switch (settData.kvsType) {
            case CBT_u32:
              if (!config_save_item_u32(settData.kvsKey, val, 16))
                ++errors;
              continue;
            default:
              break;
            }
          }
        }

        switch (kt) {
#if ENABLE_RESTART
        case otok::k_restart:
          if (mcu_restart_cb)
            mcu_restart_cb();
          break;
#endif

          ////////////////////////////////////////////////////////////
#ifdef USE_LAN
        case otok::k_lan_phy: {
          NODEFAULT();
          if (auto it = std::find(std::begin(cfg_args_lanPhy), std::end(cfg_args_lanPhy), val); it != std::end(cfg_args_lanPhy)) {
            int idx = std::distance(std::begin(cfg_args_lanPhy), it);
            set_optN(i8, idx, CB_LAN_PHY);
          }
        }
          break;
        case otok::k_lan_pwr_gpio: {
          NODEFAULT();
          set_opt(i8, val, CB_LAN_PWR_GPIO);
        }
          break;
#endif // USE_LAN

#ifdef USE_MQTT
        case otok::k_mqtt_enable:
          set_optN(i8, (*val == '1'), CB_MQTT_ENABLE);
          break;
#endif //USE_MQTT
#ifdef USE_HTTP
        case otok::k_http_enable:
          set_optN(i8, (*val == '1'), CB_HTTP_ENABLE);
          break;
#endif
          ////////////////////////////////////////////////////////////////

        case otok::k_rtc: {
          cli_replyResult(td, val ? rtc_set_by_string(val) : false);
        }
          break;

        case otok::k_cu: {
          if (is_val("auto")) {
            fer_cuas_set(cli_msgid, 60);
            cli_replySuccess(td);
          } else {
            u32 cu = strtoul(val, NULL, 16);

            if (!(GET_BYTE_2(cu) == FER_ADDR_TYPE_CentralUnit && GET_BYTE_3(cu) == 0)) {
              return cli_replyFailure(td);
            }
            (void) set_optN(u32, cu, CB_CUID);
          }
        }
          break;

#ifdef USE_NETWORK
        case otok::k_network: {
          int i;
          NODEFAULT();
          bool success = false;
          for (i = 0; i < nwLEN; ++i) {
#ifndef USE_LAN
          if (i == nwLan)
          {
            continue;
          }
#endif
#ifndef USE_WLAN
          if (i == nwWlanSta || i == nwWlanAp)
          {
            continue;
          }
#endif
            if (is_val(cfg_args_network[i])) {
              (void) (set_optN(i8, i, CB_NETWORK_CONNECTION));
              success = true;
              break;
            }
          }
          if (!success)
            cli_replyFailure(td);
        }
          break;
#endif

        case otok::k_receiver:
          cli_replyResult(td, config_receiver(val));
          break;

        case otok::k_transmitter:
          cli_replyResult(td, config_transmitter(val));
          break;

#ifdef USE_GPIO_PINS
      case otok::k_gpio:
      {
        if (is_val("?"))
        {
          soCfg_GPIO_MODES(td);
        }
        else if (*val == '$')
        {
          soCfg_GPIO_MODES_AS_STRING(td);
        }
      }
      break;
#endif

        case otok::NONE:
#ifdef USE_GPIO_PINS
        if (strncmp(key, "gpio", 4) == 0)
        {
          int gpio_number = atoi(key + 4);

          if (is_val("?"))
          {
            soCfg_GPIO_PIN(td, gpio_number);
          }
          else if (!is_gpio_number_usable(gpio_number, true))
          {
            reply_message(td, "gpio:error", "gpio number cannot be used");
            ++errors;
          }
          else
          {
            const char *error = "unknown gpio config";

            for (int i = 0; pin_mode_args[i]; ++i)
            {
              auto ps = static_cast<mcu_pin_mode>(i);
              if (pin_mode_args[ps] == *val)
              {
                mcu_pin_level pl = val[1] == 'h' ? PIN_HIGH : val[1] == 'l' ? PIN_LOW : val[1] == 'm' ? PIN_HIGH_LOW : PIN_FLOATING;
                error = pin_set_mode(gpio_number, ps, pl);
                config_gpio_setPinMode(gpio_number, ps, pl);
                hasChanged_gpio2 = true;
                break;
              }
            }
            if (error)
            {
              ++errors;
              reply_message(td, "gpio:failure", error);
            }
          }
          break;
        }
#endif

        default:
          ++errors;
          cli_warning_optionUnknown(td, key);
          break;
        }
      }

      if (pass == 0) {
        if (hasChanged_time) {
          rtc_setup();
        }

        if (hasChanged_gpio) {
          config_setup_gpio();
        }

        if (hasChanged_geo) {
          config_setup_astro();
        }
#ifdef USE_LAN
        if (hasChanged_ethernet) {
          config_setup_ethernet();
        }
#endif
#ifdef USE_MQTT
        if (hasChanged_mqttClient) {
          config_setup_mqttAppClient();
        }
#endif
#ifdef USE_HTTP
        if (hasChanged_httpServer) {
          config_setup_httpServer();
        }
#endif
        if (hasChanged_txtio) {
          config_setup_txtio();
        }
      }
    }
  }
  cli_replyResult(td, errors == 0);
  return 0;
}
