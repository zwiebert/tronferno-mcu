#include "app_config/proj_app_cfg.h"

#include "cli_app/cli_config.h"
#include <string.h>

#include "fernotron_sep/set_endpos.h"
#include "userio_app/status_output.h"
#include "fernotron_cuas/cuid_auto_set.h"
#include "app_config/proj_app_cfg.h"
#include "config/config.h"
#include "app/rtc.h"
#include "misc/bcd.h"
#include "cli_app/cli_imp.h"
#include "misc/stof.h"
#ifdef USE_MQTT
#include "net/mqtt/app/mqtt.h"
#endif
#ifdef USE_HTTP
#include "net/http/server/http_server.h"
#endif
#ifdef USE_NTP
#include "net/ntp.h"
#endif
#include "fernotron/astro.h"
#include "fernotron/fer_msg_rx.h"
#include "app/fernotron.h"
#include "app/common.h"
#include "misc/int_types.h"
#include "gpio/pin.h"
#include "app/loop.h"

#include <stdlib.h>

#define ENABLE_RESTART 1 // allow software reset


#ifdef ACCESS_GPIO
const char pin_mode_args[] = "diqQoO";
const char pin_level_args[] = "lh";
#endif

const char cli_help_parmConfig[]  =
    "'config' sets or gets options. Use: config option=value ...; to set. Use: config option=? ...; to get, if supported\n\n"
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
#ifdef POSIX_TIME
    "tz=(POSIX_TZ|?)    time zone for RTC/NTP\n"
#endif
    "astro-correction   modifies astro table: 0=average, 1=bright 2=dark\n"
    "verbose=(0..5|?)   diagnose output verbosity level\n"
    "set-pw=password    set a config password. if set every config commands needs the pw option\n"
    "pw=PW              example: config pw=my_passw dst=eu;\n"
#if ENABLE_RESTART
    "restart            restart MCU\n"
#endif
#ifdef ACCESS_GPIO
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


//key strings used for parsing and printing config commands by CLI/HTTP/MQTT
//keys must be in same order as their SO_CFG_xxx counterparts in so_msg_t
const char *const cfg_keys[SO_CFG_size] = {
    "cu", "baud", "rtc",
    "network", "wlan-ssid", "wlan-password", "ntp-server", "lan-phy", "lan-pwr-gpio",
    "longitude", "latitude", "timezone", "dst", "tz", "verbose",
    "mqtt-enable", "mqtt-url", "mqtt-user", "mqtt-password", "mqtt-client-id",
    "http-enable", "http-user", "http-password",
    "gm-used",
    "astro-correction",
    "rf-tx-pin", "rf-rx-pin", "set-button-pin", "gpio",
};

#ifdef USE_NETWORK
const char *const cfg_args_network[nwLEN] = {
    ARG_NW_NONE, ARG_NW_WLAN, ARG_NW_WLAN_AP, ARG_NW_LAN,
};
#endif

const char *const *cfg_args[SO_CFG_size] = {
};

#define CI(cb) static_cast<configItem>(cb)

#define isValid_optStr(cfg, new) true
#define set_optStr(v, cb) config_save_item_s(CI(cb), v)
#define set_optBlob(v, cb) config_save_item_b(cb, &v, sizeof v)
#define set_optStr_ifValid set_optStr
#define set_opt(t, v, cb) (config_save_item_##t(CI(cb),v) && config_item_modified(CI(cb)))
#define set_optN(t, v, cb) (config_save_item_n_##t(CI(cb),v) && config_item_modified(CI(cb)))

#define has_changed() SET_BIT(*changed_mask, so_key)

bool process_parmKvsConfig(so_msg_t so_key, const char *val, u32 *changed_mask);

int 
process_parmConfig(clpar p[], int len) {
  int arg_idx;
  int errors = 0;
  so_msg_t so_key = SO_NONE;
  u32 changed_mask = 0;
  bool flag_isValid = 0;
#ifdef USE_LAN
#define hasChanged_ethernet (changed_mask & (BIT(CB_LAN_PHY)|BIT(CB_LAN_PWR_GPIO)))
#endif
#define hasChanged_mqttClient (changed_mask & (BIT(CB_MQTT_ENABLE)|BIT(CB_MQTT_PASSWD)|\
    BIT(CB_MQTT_USER)|BIT(CB_MQTT_URL)|BIT(CB_MQTT_CLIENT_ID)))
#define hasChanged_httpServer (changed_mask & (BIT(CB_HTTP_ENABLE)|BIT(CB_HTTP_PASSWD)|BIT(CB_HTTP_USER)))
#define hasChanged_txtio (changed_mask & (BIT(CB_VERBOSE)))

  bool hasChanged_geo = false, hasChanged_gpio = false;


  so_output_message(SO_CFG_begin, NULL);

  bool pw_ok = strlen(C.app_configPassword) == 0;

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL || val == NULL) {  // don't allow any default values in config
      return cli_replyFailure();
#if ENABLE_RESTART
    } else if (strcmp(key, "restart") == 0) {
#ifdef MCU_ESP32
      loop_setBit_mcuRestart();
#else
      extern void  mcu_restart(void);
      mcu_restart();
#endif
#endif

    } else if (strcmp(key, "all") == 0) {
      if (*val == '?') {
        so_output_message(SO_CFG_all, "cj");
      }
    } else if (SO_NONE != (so_key = so_parse_config_key(key))) {
      if (0 == strcmp("?", val)) {
        so_output_message(so_key, NULL);
      } else if (process_parmKvsConfig(so_key, val, &changed_mask)) {

      } else switch (so_key) {
       case SO_CFG_RTC: {
          cli_replyResult(val ? rtc_set_by_string(val) : false);
        }
          break;

        case SO_CFG_CU: {
          if (strcmp(val, "auto") == 0) {
            cu_auto_set(cli_msgid, 60);
            cli_replySuccess();
          } else {
            u32 cu = (strcmp(val, "auto-old") == 0) ? FSB_GET_DEVID(&last_received_sender) : strtoul(val, NULL, 16);

            if (!(GET_BYTE_2(cu) == FER_ADDR_TYPE_CentralUnit && GET_BYTE_3(cu) == 0)) {
              return cli_replyFailure();
            }
            (void)set_optN(u32, cu, CB_CUID);
          }

        }
          break;
        case SO_CFG_BAUD: {
          (void)set_opt(u32, val, CB_BAUD);
        }
          break;

#ifdef USE_NETWORK
        case SO_CFG_NETWORK: {
          int i;
          NODEFAULT();
          bool success = false;
          for (i=0; i < nwLEN; ++i) {
#ifndef USE_LAN
            if (i == nwLan)  {
              continue;
            }
#endif
#ifndef USE_WLAN
            if (i == nwWlanSta || i == nwWlanAp)  {
              continue;
            }
#endif
            if (strcmp(val, cfg_args_network[i]) == 0) {
              (void)(set_optN(i8, i, CB_NETWORK_CONNECTION));
              success = true;
              break;
            }
          }
          if (!success)
            cli_replyFailure();
        }
        break;
#endif


        case SO_CFG_LONGITUDE: {
          if (set_opt(f, val, CB_LONGITUDE))
            hasChanged_geo = true;
        }
        break;

        case SO_CFG_LATITUDE: {
          if (set_opt(f, val, CB_LATITUDE))
            hasChanged_geo = true;
        }
        break;

        case SO_CFG_TIMEZONE: {
#ifndef POSIX_TIME
          if (set_opt(f, val, CB_TIZO)) {
            hasChanged_geo = true;
            rtc_setup();
          }
#endif
        }
        break;

        case SO_CFG_TZ: {
#ifdef POSIX_TIME
          if (set_optStr_ifValid(val, CB_TZ)) {
            rtc_setup();
          }

          if (!flag_isValid)
            cli_replyFailure();
#endif
        }
        break;

        case SO_CFG_DST: {
#ifdef MDR_TIME
          i8 v = 0;
          if (strcmp(val, "eu") == 0) {
           v= dstEU;
          } else if (strcmp(val, "0") == 0) {
            v = dstNone;
          } else if (strcmp(val, "1") == 0) {
            v = dstAlways;
          } else {
            cli_warning_optionUnknown(key);
          }
          rtc_setup();
          (void)set_optN(i8, v, CB_DST);
#endif
        }
        break;


        case SO_CFG_GM_USED: {
          u32 gmu = strtoul(val, NULL, 16);
          (void)set_optN(u32, gmu, CB_USED_MEMBERS);
        }
        break;

        case SO_CFG_ASTRO_CORRECTION: {
          NODEFAULT();
          auto ac = static_cast<astroCorrection>(atoi(val));
          if (set_optN(i8, ac, CB_ASTRO_CORRECTION))
            hasChanged_geo = true;
        }

        break;

        case SO_CFG_GPIO_RFIN: {
          if (set_opt(i8, val, CB_RFIN_GPIO))
            hasChanged_gpio = true;
        }
          break;
        case SO_CFG_GPIO_RFOUT: {
          if (set_opt(i8, val, CB_RFOUT_GPIO))
            hasChanged_gpio = true;
        }
          break;
        case SO_CFG_GPIO_SETBUTTON: {
          if (set_opt(i8, val, CB_SETBUTTON_GPIO))
            hasChanged_gpio = true;
        }
          break;

        default:
        break;
      }

    } else if (strcmp(key, "cuas") == 0) {
      if (*val == '?') {
        so_output_message(SO_CUAS_STATE, 0);
      }




#ifdef ACCESS_GPIO
    } else if (strcmp(key, "gpio") == 0) {
      if (*val == '?') {
        so_output_message(SO_CFG_GPIO_MODES, 0);
      } else if (*val == '$') {
        so_output_message(SO_CFG_GPIO_MODES_AS_STRING, 0);
      }
    } else if (strncmp(key, "gpio", 4) == 0) {
      int gpio_number = atoi(key + 4);

      if (*val == '?') {
        so_output_message(SO_CFG_GPIO_PIN, &gpio_number);
      } else if (!is_gpio_number_usable(gpio_number, true)) {
        reply_message("gpio:error", "gpio number cannot be used");
        ++errors;
      } else {
        const char *error = "unknown gpio config";

        for (int i = 0; pin_mode_args[i]; ++i) {
          auto ps = static_cast<mcu_pin_mode>(i);
          if (pin_mode_args[ps] == *val) {
            mcu_pin_level pl = val[1] == 'h' ? PIN_HIGH : val[1] == 'l' ? PIN_LOW : val[1] == 'm' ? PIN_HIGH_LOW : PIN_FLOATING;
            error = pin_set_mode(gpio_number, ps, pl);
            config_gpio_setPinMode(gpio_number, ps, pl);
            hasChanged_gpio = true;
            break;
          }
        }
        if (error) {
          ++errors;
          reply_message("gpio:failure", error);
        }
      }
#endif

    } else if (strcmp(key, "set-pw") == 0) {
      if (set_optStr_ifValid(val, CB_CFG_PASSWD)){}

      if (!flag_isValid)
        cli_replyFailure();

    } else if (strcmp(key, "receiver") == 0) {
      cli_replyResult(config_receiver(val));

    } else if (strcmp(key, "transmitter") == 0) {
      cli_replyResult(config_transmitter(val));

    } else {
      ++errors;
      cli_warning_optionUnknown(key);
    }
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
  so_output_message(SO_CFG_end, NULL);
  cli_replyResult(errors==0);
  return 0;
}
