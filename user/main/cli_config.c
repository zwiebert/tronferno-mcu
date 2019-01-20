#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../user_config.h"

#include "all.h"
#include "set_endpos.h"
#include "misc/bcd.h"
#include "cli_imp.h"
#include "misc/stof.h"

#define ENABLE_RESTART 1 // allow software reset


#ifdef CONFIG_GPIO_SIZE
//PIN_DEFAULT=0, PIN_INPUT, PIN_INPUT_PULLUP, PIN_OUTPUT, PIN_ERROR, PIN_READ, PIN_CLEAR, PIN_SET, PIN_TOGGLE

const char pin_state_args[] = "dipo ?01t";
#endif

const char help_parmConfig[] PROGMEM =
    "'config' sets or gets options. Use: config option=value ...; to set. Use: config option=? ...; to get, if supported\n\n"
    "cu=(ID|auto|?)     6-digit hex ID of Central-Unit. auto: capture ID using connected RF receiver\n"
    "rtc=(ISO_TIME|?)   set local time with this (or use NTP). example: config rtc=2017-12-31T23:59:59;\n"
    "baud=(N|?)         serial baud rate (... baud=115200)\n"
#ifdef USE_WLAN
    "wlan-ssid=(SSID|?)\n"
    "wlan-password=PW   example: config wlan-ssid=\"1234\" wlan-password=\"abcd\" restart=1;\n"
#endif
    "longitude=(DEG|?)\n"
    "latitude=(DEG|?)\n"
 #if !POSIX_TIME
    "time-zone=(N|?)    time zone hour offset for astro and rtc\n"
    "dst=(eu|0|1|?)     daylight saving time: automatic: eu=europe. manually: 0=off, 1=on\n"
#else
    "time-zone=(N|?)    time offset for astro\n"
    "tz=(POSIX_TZ|?)    time zone for RTC/NTP. example: config  tz=CET-1CEST-2,M3.5.0,M10.5.0;\n"
#endif
    "verbose=(0..5|?)   set text output verbosity level. 0 for none ... 5 for max\n"
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
  const char *cfgSep = "config ";

  bool pw_ok = strlen(C.app_configPassword) == 0;

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL || val == NULL) {  // don't allow any default values in config
      return reply_failure();   } else if (strcmp(key, "pw") == 0) {
      if (val && strcmp(C.app_configPassword, val) == 0) {
        pw_ok = true;
        io_puts("password ok\n");

      } else {
        io_puts("wrong config password\n");
        return reply_failure();
      }
    } else if (!pw_ok) {
      io_puts("missing config password\n");
      return reply_failure();


#if ENABLE_RESTART
    } else if (strcmp(key, "restart") == 0) {
      mcu_restart();
#endif


    } else if (strcmp(key, "rtc") == 0) {
      if (*val == '?') {
        char buf[20];
        if (rtc_get_by_string(buf)) {
          io_puts(cfgSep), io_puts(key), io_puts("="), io_puts(buf), (cfgSep = " ");
        }
      } else {
        reply(val ? rtc_set_by_string(val) : false);
      }


    } else if (strcmp(key, "cu") == 0) {
      if (*val == '?') {
        io_puts(cfgSep), io_puts(key), io_puts("="), io_print_hex_32(C.fer_centralUnitID, false), (cfgSep = " ");
      } else if (strcmp(val, "auto") == 0) {
        cu_auto_set(60);
        io_puts("U: Press Stop on the Fernotron central unit (60 secs remaining)\n");
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
        io_puts(cfgSep), io_puts(key), io_puts("="), io_print_dec_32(C.mcu_serialBaud, false), (cfgSep = " ");
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
        io_puts(cfgSep), io_puts(key), io_putc('='), io_putc(pin_state_args[C.gpio[gpio_number]]), (cfgSep = " ");

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
        io_puts(cfgSep), io_puts(key), io_puts("="), io_putd(C.app_verboseOutput), (cfgSep = " ");
      } else {
        NODEFAULT();
        enum verbosity level = atoi(val);
        C.app_verboseOutput = level;
        save_config(CONFIG_VERBOSE);
      }


#ifdef USE_WLAN
    } else if (strcmp(key, "wlan-ssid") == 0) {
      if (*val=='?') {
        io_puts(cfgSep), io_puts(key), io_puts("=\""), io_puts(C.wifi_SSID), io_puts("\""), (cfgSep = " ");
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
        io_puts(cfgSep), io_puts(key), io_puts("="), io_print_float(C.geo_longitude, 5), (cfgSep = " ");
      } else {
        float longitude = stof(val);
        C.geo_longitude = longitude;
        rtc_setup();
        save_config(CONFIG_LONGITUDE);
      }


    } else if (strcmp(key, "latitude") == 0) {
      if (*val=='?') {
        io_puts(cfgSep), io_puts(key), io_puts("="), io_print_float(C.geo_latitude, 5), (cfgSep = " ");
      } else {
        float latitude = stof(val);
        C.geo_latitude = latitude;
        rtc_setup();
        save_config(CONFIG_LATITUDE);
      }


    } else if (strcmp(key, "time-zone") == 0) {
      if (*val=='?') {

        io_puts(cfgSep), io_puts(key), io_puts("="), io_print_float(C.geo_timezone, 2), (cfgSep = " ");
      } else {
        C.geo_timezone = stof(val);

        rtc_setup();
        save_config(CONFIG_TZ);
      }

#if POSIX_TIME
    } else if (strcmp(key, "tz") == 0) {
      if (*val=='?') {
        io_puts(cfgSep), io_puts(key), io_puts("="), io_puts(C.geo_tz), (cfgSep = " ");

      } else {
        strncpy(C.geo_tz, val, sizeof (C.geo_tz) -1);

        rtc_setup();
        save_config(CONFIG_TZ);
      }
#endif
    } else if (strcmp(key, "dst") == 0) {
#if POSIX_TIME
      if (*val=='?') {
        io_puts(cfgSep), io_puts("dst=0"), (cfgSep = " ");
      }
#else
      if (*val=='?') {
        io_puts(cfgSep), io_puts(key), io_puts("="), io_puts((C.geo_dST == dstEU ? "eu": (C.geo_dST == dstNone ? "0" : "1"))), (cfgSep = " ");
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

if (strcmp(cfgSep, " ") == 0)
  io_puts(";\n");

  reply(errors==0);
  return 0;
}


bool cuas_active;
bool ICACHE_FLASH_ATTR cu_auto_set(unsigned init_seconds) {
  static time_t end_time;
  static uint16_t cuas_msgid;

  if (init_seconds > 0) {
    end_time = run_time(NULL) + init_seconds;
    last_received_sender.data[0] = 0;
    cuas_msgid = msgid;
    cuas_active = true;
  } else if (end_time == 0) {

  } else if (end_time < run_time(NULL)) {
    uint16_t global_msgid = msgid;
    end_time = 0;
    io_puts("U: Nothing received\n");
    msgid = cuas_msgid;
    reply_message("cuas=time-out", 0);
    msgid = global_msgid;
    cuas_active = false;
  } else if (FSB_ADDR_IS_CENTRAL(&last_received_sender)) {
    uint32_t cu = FSB_GET_DEVID(&last_received_sender);

    FSB_PUT_DEVID(&default_sender, cu);
    C.fer_centralUnitID = cu;
    end_time = 0;
    io_puts("U: Central Unit received and stored\n");
    reply_message("cuas=ok", 0);
    save_config(CONFIG_CUID);
    cuas_active = false;
    return true;
  }

  return false;
}
