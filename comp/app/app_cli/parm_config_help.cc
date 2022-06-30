#include "app_config/proj_app_cfg.h"
#include "app_cli/cli_app.h"


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
    "rf-tx-pin=N          RF output GPIO pin\n"
    "rf-rx-pin=N          RF input GPI pin\n"
    "set-button-pin=N     Set-button input GPI pin\n"
    "rf-sclk-pin=N        CC1101 SPI clock GPIO pin\n"
    "rf-ss-pin=N          CC1101 SPI chip-select GPIO pin\n"
    "rf-mosi-pin=N        CC1101 SPI MOSI GPIO pin\n"
    "rf-miso-pin=N        CC1101 SPI MISO GPI pin\n"
    "rf-trx=(none|rx-tx|rx-itx|cc1101)   Which RF hardware is connected\n"
    "rf-repeater=(+ID|-ID|IDs|) Add or Remove IDs (6digit Hex) for RF-repeater\n"
//  "set-expert-password=\n"
;
