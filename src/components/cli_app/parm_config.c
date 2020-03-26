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
#include "cli_imp.h"
#include "misc/stof.h"
#ifdef USE_MQTT
#include "net/mqtt/app/mqtt.h"
#endif
#ifdef USE_HTTP
#include "net/http/server/http_server.h"
#endif
#include "fernotron/astro.h"
#include "fernotron/fer_msg_rx.h"
#include "app/fernotron.h"
#include "app/common.h"
#include "misc/int_types.h"

#define ENABLE_RESTART 1 // allow software reset


#ifdef CONFIG_GPIO_SIZE
//PIN_DEFAULT=0, PIN_INPUT, PIN_INPUT_PULLUP, PIN_OUTPUT, PIN_ERROR, PIN_READ, PIN_CLEAR, PIN_SET, PIN_TOGGLE

const char pin_state_args[] = "dipo ?01t";
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
     "mqtt-enable=(0|1) enable MQTT\n"
     "mqtt-url=URL      broker/server URL (e.g. mqtt://192.168.1.42:7777)\n"
     "mqtt-user=NAME    user name on server\n"
     "mqtt-password=PW  user password on server\n"
#endif
#ifdef USE_HTTP
     "http-enable=(0|1) enable HTTP\n"
     "http-user=NAME    user name on server\n"
     "http-password=PW  user password on server\n"
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
    "verbose=(0..5|?)   diagnose output verbosity level\n"
    "set-pw=password    set a config password. if set every config commands needs the pw option\n"
    "pw=PW              example: config pw=my_passw dst=eu;\n"
#if ENABLE_RESTART
    "restart            restart MCU\n"
#endif
#ifdef ACCESS_GPIO
    "gpioN=(i|p|o|0|1|d|?) Set gpio pin as input (i,p) or output (o,0,1) or use default\n"
#endif
    "astro-correction   modifies astro table: 0=average, 1=bright 2=dark\n"
//  "set-expert-password=\n"
;

//key strings used for parsing and printing config commands by CLI/HTTP/MQTT
//keys must be in same order as their SO_CFG_xxx counterparts in so_msg_t
const char *const cfg_keys[SO_CFG_size] = {
    "cu", "baud", "rtc",
    "network", "wlan-ssid", "wlan-password", "ntp-server", "lan-phy", "lan-pwr-gpio",
    "longitude", "latitude", "timezone", "dst", "tz", "verbose",
    "mqtt-enable", "mqtt-url", "mqtt-user", "mqtt-password",
    "http-enable", "http-user", "http-password",
    "gm-used", "astro-correction",
};

#ifdef USE_NETWORK
const char *const cfg_args_network[nwLEN] = {
    ARG_NW_NONE, ARG_NW_WLAN, ARG_NW_WLAN_AP, ARG_NW_LAN,
};
#endif

#ifdef USE_LAN
const char *const cfg_args_lanPhy[lanPhyLEN] = {
    "none", "lan8270", "rtl8201", "ip101",
};
#endif

const char *const *cfg_args[SO_CFG_size] = {

};



int 
process_parmConfig(clpar p[], int len) {
  int arg_idx;
  int errors = 0;
  so_msg_t so_key = SO_NONE;

  so_output_message(SO_CFG_begin, NULL);

  bool pw_ok = strlen(C.app_configPassword) == 0;

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL || val == NULL) {  // don't allow any default values in config
      return cli_replyFailure();
    } else if (strcmp(key, "pw") == 0) {
      if (val && strcmp(C.app_configPassword, val) == 0) {
        pw_ok = true;
        so_output_message(SO_CFGPASSWD_OK, NULL);
      } else {
        so_output_message(SO_CFGPASSWD_WRONG, NULL);
        return cli_replyFailure();
      }
    } else if (!pw_ok) {
      so_output_message(SO_CFGPASSWD_MISSING, NULL);
      return cli_replyFailure();


#if ENABLE_RESTART
    } else if (strcmp(key, "restart") == 0) {
      mcu_restart();
#endif

    } else if (strcmp(key, "all") == 0) {
      if (*val == '?') {
        so_output_message(SO_CFG_all, "cj");
      }
    } else if (SO_NONE != (so_key = so_parse_config_key(key))) {
      if (0 == strcmp("?", val)) {
        so_output_message(so_key, NULL);
      } else
        switch (so_key) {

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
            FSB_PUT_DEVID(&default_sender, cu);
            C.fer_centralUnitID = cu;
            save_config_item(CB_CUID);
          }

        }
          break;
        case SO_CFG_BAUD: {
          u32 baud = strtoul(val, NULL, 10);
          C.mcu_serialBaud = baud;
          save_config_item(CB_BAUD);
        }
          break;
        case SO_CFG_VERBOSE: {
          NODEFAULT();
          enum verbosity level = atoi(val);
          C.app_verboseOutput = level;
          save_config_item(CB_VERBOSE);
        }
        break;
#ifdef USE_NETWORK
        case SO_CFG_NETWORK: {
          NODEFAULT();
          bool success = false;
          for (int i=0; i < nwLEN; ++i) {
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
              C.network = i;
              save_config_item(CB_NETWORK_CONNECTION);
              success = true;
              break;
            }
          }
          if (!success)
            cli_replyFailure();
        }
        break;
#endif
#ifdef USE_WLAN
        case SO_CFG_WLAN_SSID: {
          if (strlen(val) < sizeof (C.wifi_SSID)) {
            strcpy (C.wifi_SSID, val);
            save_config_item(CB_WIFI_SSID);
          } else {
            cli_replyFailure();
          }
        }
        break;

        case SO_CFG_WLAN_PASSWORD: {
          if (strlen(val) < sizeof (C.wifi_password)) {
            strcpy (C.wifi_password, val);
            save_config_item(CB_WIFI_PASSWD);
          } else {
            cli_replyFailure();
          }
        }
        break;
#endif // USE_WLAN
#ifdef USE_LAN
        case SO_CFG_LAN_PHY: {
          NODEFAULT();
           bool success = false;
           u8 i;
           for (i=0; i < lanPhyLEN; ++i) {
             if (strcasecmp(val, cfg_args_lanPhy[i]) == 0) {
               C.lan_phy = i;
               save_config_item(CB_LAN_PHY);
               success = true;
               break;
             }
           }
           if (!success)
             cli_replyFailure();
         }
         break;
        case SO_CFG_LAN_PWR_GPIO: {
          NODEFAULT();
          C.lan_pwr_gpio = atoi(val);
          save_config_item(CB_LAN_PWR_GPIO);
        }
        break;
#endif // USE_LAN

#ifdef USE_NTP
        case SO_CFG_NTP_SERVER: {
          if (strlen(val) < sizeof (C.ntp_server)) {
            strcpy (C.ntp_server, val);
            save_config_item(CB_NTP_SERVER);
          } else {
            cli_replyFailure();
          }
        }
        break;
#endif

#ifdef USE_MQTT
        case SO_CFG_MQTT_ENABLE: {
          C.mqtt_enable = (*val == '1') ? 1 : 0;
          io_mqtt_enable(C.mqtt_enable);
          save_config_item(CB_MQTT_ENABLE);
        }
        break;

        case SO_CFG_MQTT_PASSWORD: {
          if (strlen(val) < sizeof (C.mqtt_password)) {
            strcpy (C.mqtt_password, val);
            save_config_item(CB_MQTT_PASSWD);
          } else {
            cli_replyFailure();
          }

        }
        break;

        case SO_CFG_MQTT_USER: {
          if (strlen(val) < sizeof (C.mqtt_user)) {
            strcpy (C.mqtt_user, val);
            save_config_item(CB_MQTT_USER);
          } else {
            cli_replyFailure();
          }
        }
        break;

        case SO_CFG_MQTT_URL: {
          if (strlen(val) < sizeof (C.mqtt_url)) {
            strcpy (C.mqtt_url, val);
            save_config_item(CB_MQTT_URL);
          } else {
            cli_replyFailure();
          }
        }
        break;
#endif //USE_MQTT

#ifdef USE_HTTP
        case SO_CFG_HTTP_ENABLE: {
          C.http_enable = (*val == '1') ? 1 : 0;
          hts_enable_http_server(C.http_enable);
          save_config_item(CB_HTTP_ENABLE);
        }
        break;

        case SO_CFG_HTTP_PASSWORD: {
          if (strlen(val) < sizeof (C.http_password)) {
            strcpy (C.http_password, val);
            save_config_item(CB_HTTP_PASSWD);
          } else {
            cli_replyFailure();
          }

        }
        break;

        case SO_CFG_HTTP_USER: {
          if (strlen(val) < sizeof (C.http_user)) {
            strcpy (C.http_user, val);
            save_config_item(CB_HTTP_USER);
          } else {
            cli_replyFailure();
          }
        }
        break;
#endif //USE_HTTP

        case SO_CFG_LONGITUDE: {
          float longitude = stof(val);
          C.geo_longitude = longitude;
          rtc_setup();
          save_config_item(CB_LONGITUDE);
        }
        break;

        case SO_CFG_LATITUDE: {
          float latitude = stof(val);
          C.geo_latitude = latitude;
          rtc_setup();
          save_config_item(CB_LATITUDE);
        }
        break;

        case SO_CFG_TIMEZONE: {
#ifndef POSIX_TIME
          C.geo_timezone = stof(val);
          rtc_setup();
          save_config_item(CB_TIZO);
#endif
        }
        break;

        case SO_CFG_TZ: {
#ifdef POSIX_TIME
          strncpy(C.geo_tz, val, sizeof (C.geo_tz) -1);
          rtc_setup();
          save_config_item(CB_TZ);
          cfg_tz2timezone();
#endif
        }
        break;

        case SO_CFG_DST: {
#ifdef MDR_TIME
          if (strcmp(val, "eu") == 0) {
            C.geo_dST = dstEU;
          } else if (strcmp(val, "0") == 0) {
            C.geo_dST = dstNone;
          } else if (strcmp(val, "1") == 0) {
            C.geo_dST = dstAlways;
          } else {
            cli_warning_optionUnknown(key);
          }
          rtc_setup();
          save_config_item(CB_DST);
#endif
        }
        break;


        case SO_CFG_GM_USED: {
           u32 gmu = strtoul(val, NULL, 16);
           C.fer_usedMembers = gmu;
           save_config_item(CB_USED_MEMBERS);
        }
        break;

        case SO_CFG_ASTRO_CORRECTION: {
          NODEFAULT();
          enum astroCorrection ac = atoi(val);
          C.astroCorrection = ac;
          save_config_item(CB_ASTRO_CORRECTION);
          astro_init_and_reinit();
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
    } else if (strncmp(key, "gpio", 4) == 0) {
      int gpio_number = atoi(key + 4);
      mcu_pin_state ps;

      if (*val == '?') {
        so_output_message(SO_CFG_GPIO_PIN, &gpio_number);
      } else if (!is_gpio_number_usable(gpio_number, true)) {
        reply_message("gpio:error", "gpio number cannot be used");
        ++errors;
      } else {
        const char *error = NULL;

        for (ps = 0; pin_state_args[ps] != 0; ++ps) {
          if (pin_state_args[ps] == *val) {
            break;
          }
        }

        switch (ps) {

          case PIN_DEFAULT:
          break;

          case PIN_CLEAR:
          case PIN_SET:
          case PIN_OUTPUT:
          error = mcu_access_pin(gpio_number, NULL, PIN_OUTPUT);
          if (!error && ps != PIN_OUTPUT) {
            error = mcu_access_pin(gpio_number, NULL, ps);
          }
          break;

          case PIN_INPUT:
          case PIN_INPUT_PULLUP:
          error = mcu_access_pin(gpio_number, NULL, ps);
          break;

          default:
          error = "unknown gpio config";
          ++errors;
        }

        if (error) {
          reply_message("gpio:failure", error);
        } else {
          C.gpio[gpio_number] = ps;
          save_config_item(CB_GPIO);
        }
      }
#endif

    } else if (strcmp(key, "set-pw") == 0) {
      if (strlen(val) < sizeof (C.app_configPassword)) {
        strcpy (C.app_configPassword, val);
        save_config_item(CB_CFG_PASSWD);
      } else {
        cli_replyFailure();
      }

    } else if (strcmp(key, "receiver") == 0) {
      cli_replyResult(config_receiver(val));

    } else if (strcmp(key, "transmitter") == 0) {
      cli_replyResult(config_transmitter(val));

    } else {
      ++errors;
      cli_warning_optionUnknown(key);
    }
  }

  so_output_message(SO_CFG_end, NULL);
  cli_replyResult(errors==0);
  return 0;
}
