#include "user_config.h"

#include "cli/cli_config.h"
#include <string.h>

#include "../userio/status_output.h"
#include "main/cuid_auto_set.h"
#include "user_config.h"
#include "config/config.h"
#include "main/rtc.h"
#include "setup/set_endpos.h"
#include "misc/bcd.h"
#include "cli_imp.h"
#include "misc/stof.h"
#include "userio/mqtt.h"
#include "userio/http_server.h"

#define ENABLE_RESTART 1 // allow software reset


#ifdef CONFIG_GPIO_SIZE
//PIN_DEFAULT=0, PIN_INPUT, PIN_INPUT_PULLUP, PIN_OUTPUT, PIN_ERROR, PIN_READ, PIN_CLEAR, PIN_SET, PIN_TOGGLE

const char pin_state_args[] = "dipo ?01t";
#endif

const char help_parmConfig[]  =
    "'config' sets or gets options. Use: config option=value ...; to set. Use: config option=? ...; to get, if supported\n\n"
    "cu=(ID|auto|?)     6-digit hex ID of Central-Unit. auto: capture ID\n"
    "rtc=(ISO_TIME|?)   set local time it NTP is not working.;\n"
    "baud=(N|?)         serial baud rate\n"
#ifdef USE_WLAN
    "wlan-ssid=(SSID|?)\n"
    "wlan-password=PW\n"
#endif
#ifdef USE_NETWORK
    "network=(wlan|ap|lan) ap opens wlan access-point at 192.168.4.1 (restart required)"
#endif
#ifdef USE_NTP
    "ntp-server=(dhcp|IP4ADDR|NAME) default: ntp.pool.org (TODO: implement list of servers)\n"
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
//  "set-expert-password=\n"
;

//key strings used for parsing and printing config commands by CLI/HTTP/MQTT
//keys must be in same order as their SO_CFG_xxx counterparts in so_msg_t
const char *const cfg_keys[SO_CFG_size] = {
    "cu", "baud", "rtc",
    "network", "wlan-ssid", "wlan-password", "ntp-server",
    "longitude", "latitude", "timezone", "dst", "tz", "verbose",
    "mqtt-enable", "mqtt-url", "mqtt-user", "mqtt-password",
    "http-enable", "http-user", "http-password",
    "gm-used",
};

#ifdef USE_NETWORK
const char *const cfg_args_network[nwLEN] = {
    ARG_NW_NONE, ARG_NW_WLAN, ARG_NW_WLAN_AP, ARG_NW_LAN,
};
#endif

const char *const *cfg_args[SO_CFG_size] = {

};



int ICACHE_FLASH_ATTR
process_parmConfig(clpar p[], int len) {
  int arg_idx;
  int errors = 0;
  so_msg_t so_key = SO_NONE;

  so_output_message(SO_CFG_begin, NULL);

  bool pw_ok = strlen(C.app_configPassword) == 0;

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL || val == NULL) {  // don't allow any default values in config
      return reply_failure();
    } else if (strcmp(key, "pw") == 0) {
      if (val && strcmp(C.app_configPassword, val) == 0) {
        pw_ok = true;
        so_output_message(SO_CFGPASSWD_OK, NULL);
      } else {
        so_output_message(SO_CFGPASSWD_WRONG, NULL);
        return reply_failure();
      }
    } else if (!pw_ok) {
      so_output_message(SO_CFGPASSWD_MISSING, NULL);
      return reply_failure();


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
          reply(val ? rtc_set_by_string(val) : false);
        }
          break;

        case SO_CFG_CU: {
          if (strcmp(val, "auto") == 0) {
            cu_auto_set(msgid, 60);
            reply_success();
          } else {
            u32 cu = (strcmp(val, "auto-old") == 0) ? FSB_GET_DEVID(&last_received_sender) : strtoul(val, NULL, 16);

            if (!(GET_BYTE_2(cu) == FER_ADDR_TYPE_CentralUnit && GET_BYTE_3(cu) == 0)) {
              return reply_failure();
            }
            FSB_PUT_DEVID(&default_sender, cu);
            C.fer_centralUnitID = cu;
            save_config(CONFIG_CUID);
          }

        }
          break;

        case SO_CFG_BAUD: {
          u32 baud = strtoul(val, NULL, 10);
          C.mcu_serialBaud = baud;
          save_config(CONFIG_BAUD);
        }
          break;

        case SO_CFG_VERBOSE: {
          NODEFAULT();
          enum verbosity level = atoi(val);
          C.app_verboseOutput = level;
          save_config(CONFIG_VERBOSE);
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
              save_config(CONFIG_NETWORK_CONNECTION);
              success = true;
              break;
            }
          }
          if (!success)
            reply_failure();
        }
        break;
#endif
#ifdef USE_WLAN
        case SO_CFG_WLAN_SSID: {
          if (strlen(val) < sizeof (C.wifi_SSID)) {
            strcpy (C.wifi_SSID, val);
            save_config(CONFIG_WIFI_SSID);
          } else {
            reply_failure();
          }
        }
        break;

        case SO_CFG_WLAN_PASSWORD: {
          if (strlen(val) < sizeof (C.wifi_password)) {
            strcpy (C.wifi_password, val);
            save_config(CONFIG_WIFI_PASSWD);
          } else {
            reply_failure();
          }
        }
        break;
#endif // USE_WLAN
#ifdef USE_NTP
        case SO_CFG_NTP_SERVER: {
          if (strlen(val) < sizeof (C.ntp_server)) {
            strcpy (C.ntp_server, val);
            save_config(CONFIG_NTP_SERVER);
          } else {
            reply_failure();
          }
        }
        break;
#endif

#ifdef USE_MQTT
        case SO_CFG_MQTT_ENABLE: {
          C.mqtt_enable = (*val == '1') ? 1 : 0;
          io_mqtt_enable(C.mqtt_enable);
          save_config(CONFIG_MQTT_ENABLE);
        }
        break;

        case SO_CFG_MQTT_PASSWORD: {
          if (strlen(val) < sizeof (C.mqtt_password)) {
            strcpy (C.mqtt_password, val);
            save_config(CONFIG_MQTT_PASSWD);
          } else {
            reply_failure();
          }

        }
        break;

        case SO_CFG_MQTT_USER: {
          if (strlen(val) < sizeof (C.mqtt_user)) {
            strcpy (C.mqtt_user, val);
            save_config(CONFIG_MQTT_USER);
          } else {
            reply_failure();
          }
        }
        break;

        case SO_CFG_MQTT_URL: {
          if (strlen(val) < sizeof (C.mqtt_url)) {
            strcpy (C.mqtt_url, val);
            save_config(CONFIG_MQTT_URL);
          } else {
            reply_failure();
          }
        }
        break;
#endif //USE_MQTT

#ifdef USE_HTTP
        case SO_CFG_HTTP_ENABLE: {
          C.http_enable = (*val == '1') ? 1 : 0;
          hts_enable_http_server(C.http_enable);
          save_config(CONFIG_HTTP_ENABLE);
        }
        break;

        case SO_CFG_HTTP_PASSWORD: {
          if (strlen(val) < sizeof (C.http_password)) {
            strcpy (C.http_password, val);
            save_config(CONFIG_HTTP_PASSWD);
          } else {
            reply_failure();
          }

        }
        break;

        case SO_CFG_HTTP_USER: {
          if (strlen(val) < sizeof (C.http_user)) {
            strcpy (C.http_user, val);
            save_config(CONFIG_HTTP_USER);
          } else {
            reply_failure();
          }
        }
        break;
#endif //USE_HTTP

        case SO_CFG_LONGITUDE: {
          float longitude = stof(val);
          C.geo_longitude = longitude;
          rtc_setup();
          save_config(CONFIG_LONGITUDE);
        }
        break;

        case SO_CFG_LATITUDE: {
          float latitude = stof(val);
          C.geo_latitude = latitude;
          rtc_setup();
          save_config(CONFIG_LATITUDE);
        }
        break;

        case SO_CFG_TIMEZONE: {
          C.geo_timezone = stof(val);
          rtc_setup();
          save_config(CONFIG_TIZO);
        }
        break;

        case SO_CFG_TZ: {
#ifdef POSIX_TIME
          strncpy(C.geo_tz, val, sizeof (C.geo_tz) -1);
          rtc_setup();
          save_config(CONFIG_TZ);
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
            warning_unknown_option(key);
          }
          rtc_setup();
          save_config(CONFIG_DST);
#endif
        }
        break;


        case SO_CFG_GM_USED: {
           u32 gmu = strtoul(val, NULL, 16);
           C.fer_usedMembers = gmu;
           save_config(CONFIG_USED_MEMBERS);
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
          save_config(CONFIG_GPIO);
        }
      }
#endif

    } else if (strcmp(key, "set-pw") == 0) {
      if (strlen(val) < sizeof (C.app_configPassword)) {
        strcpy (C.app_configPassword, val);
        save_config(CONFIG_CFG_PASSWD);
      } else {
        reply_failure();
      }

    } else if (strcmp(key, "receiver") == 0) {
      reply(config_receiver(val));

    } else if (strcmp(key, "transmitter") == 0) {
      reply(config_transmitter(val));

    } else {
      ++errors;
      warning_unknown_option(key);
    }
  }

  so_output_message(SO_CFG_end, NULL);
  reply(errors==0);
  return 0;
}
