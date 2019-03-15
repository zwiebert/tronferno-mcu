#include "user_config.h"
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
    "longitude=(DEG|?)\n"
    "latitude=(DEG|?)\n"
 #if !POSIX_TIME
    "time-zone=(N|?)    time zone hour offset\n"
    "dst=(eu|0|1|?)     daylight saving time\n"
#else
    "tz=(POSIX_TZ|?)    time zone for RTC/NTP\n"
#endif
    "verbose=(0..5|?)   diagnose output verbosity level\n"
    "set-pw=password    set a config password. if set every config commands needs the pw option\n"
    "pw=PW              example: config pw=my_passw dst=eu;\n"
#if ENABLE_RESTART
    "restart            restart MCU\n"
#endif
#ifdef CONFIG_GPIO_SIZE
    "gpioN=(i|p|o|0|1|d|?) Set gpio pin as input (i,p) or output (o,0,1) or use default\n"
#endif
//  "set-expert-password=\n"
;
#define slf "\n"
#define cfg "config "

int ICACHE_FLASH_ATTR
process_parmConfig(clpar p[], int len) {
  int arg_idx;
  int errors = 0;

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
        so_output_message(SO_CFG_all, NULL);
      }
    } else if (strcmp(key, "rtc") == 0) {
      if (*val == '?') {
        so_output_message(SO_CFG_RTC, NULL);
      } else {
        reply(val ? rtc_set_by_string(val) : false);
      }


    } else if (strcmp(key, "cu") == 0) {
      if (*val == '?') {
        so_output_message(SO_CFG_CU, NULL);
      } else if (strcmp(val, "auto") == 0) {
        cu_auto_set(msgid, 60);
        reply_success();
      } else {
        uint32_t cu = (strcmp(val, "auto-old") == 0) ? FSB_GET_DEVID(&last_received_sender) : strtoul(val, NULL, 16);

        if (!(GET_BYTE_2(cu) == FER_ADDR_TYPE_CentralUnit && GET_BYTE_3(cu) == 0)) {
          return reply_failure();
        }
        FSB_PUT_DEVID(&default_sender, cu);
        C.fer_centralUnitID = cu;
	save_config(CONFIG_CUID);
      }


    } else if (strcmp(key, "gm-used") == 0) {
      uint32_t gmu = strtoul(val, NULL, 16);
      C.fer_usedMembers = gmu;
      save_config(CONFIG_USED_MEMBERS);


    } else if (strcmp(key, "baud") == 0) {
      if (*val == '?') {
        so_output_message(SO_CFG_BAUD, NULL);
      } else {
        uint32_t baud = strtoul(val, NULL, 10);
        C.mcu_serialBaud = baud;
        save_config(CONFIG_BAUD);
      }

#ifdef CONFIG_GPIO_SIZE
    } else if (strncmp(key, "gpio", 4) == 0) {
      int gpio_number = atoi(key + 4);
      mcu_pin_state ps;

      if (!is_gpio_number_usable(gpio_number, true)) {
        reply_message("gpio:error", "gpio number cannot be used");
        ++errors;

      } else if (*val == '?') {
        so_output_message(SO_CFG_GPIO_PIN, &gpio_number);
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

    } else if (strcmp(key, "verbose") == 0) {
      if (*val == '?') {
        so_output_message(SO_CFG_VERBOSE, NULL);
      } else {
        NODEFAULT();
        enum verbosity level = atoi(val);
        C.app_verboseOutput = level;
        save_config(CONFIG_VERBOSE);
      }


#ifdef USE_WLAN
    } else if (strcmp(key, "wlan-ssid") == 0) {
      if (*val=='?') {
        so_output_message(SO_CFG_WLAN_SSID, NULL);
      } else {
        if (strlen(val) < sizeof (C.wifi_SSID)) {
          strcpy (C.wifi_SSID, val);
          save_config(CONFIG_WIFI_SSID);
        } else {
          reply_failure();
        }
      }


    } else if (strcmp(key, "wlan-password") == 0) {
      if (strlen(val) < sizeof (C.wifi_password)) {
        strcpy (C.wifi_password, val);
        save_config(CONFIG_WIFI_PASSWD);
      } else {
        reply_failure();
      }
#endif // USE_WLAN


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


    } else if (strcmp(key, "longitude") == 0) {
      if (*val=='?') {
        so_output_message(SO_CFG_LONGITUDE, NULL);
      } else {
        float longitude = stof(val);
        C.geo_longitude = longitude;
        rtc_setup();
        save_config(CONFIG_LONGITUDE);
      }


    } else if (strcmp(key, "latitude") == 0) {
      if (*val=='?') {
        so_output_message(SO_CFG_LONGITUDE, NULL);
      } else {
        float latitude = stof(val);
        C.geo_latitude = latitude;
        rtc_setup();
        save_config(CONFIG_LATITUDE);
      }


    } else if (strcmp(key, "time-zone") == 0) {
      if (*val=='?') {
        so_output_message(SO_CFG_TIMEZONE, NULL);
      } else {
        C.geo_timezone = stof(val);

        rtc_setup();
        save_config(CONFIG_TZ);
      }

#if POSIX_TIME
    } else if (strcmp(key, "tz") == 0) {
      if (*val=='?') {
        so_output_message(SO_CFG_TZ, NULL);
      } else {
        strncpy(C.geo_tz, val, sizeof (C.geo_tz) -1);

        rtc_setup();
        save_config(CONFIG_TZ);
      }
#endif
    } else if (strcmp(key, "dst") == 0) {
#if POSIX_TIME
      if (*val=='?') {

      }
#else
      if (*val=='?') {
        so_output_message(SO_CFG_DST, NULL);
      } else {

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

      }
#endif

    } else {
      ++errors;
      warning_unknown_option(key);
    }
  }



  so_output_message(SO_CFG_end, NULL);
  reply(errors==0);
  return 0;
}

