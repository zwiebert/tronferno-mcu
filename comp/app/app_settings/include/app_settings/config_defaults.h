/**
 * \file   app_settings/config_defaults.h
 * \brief  default values for settings XXX: Refactor this
 */

#pragma once

#if !defined MCU_ESP8266
#if __has_include("sensitive/defaults.h")
#define HAVE_USER_DEFAULTS
#endif
#endif

#if defined HAVE_USER_DEFAULTS && ! defined DISTRIBUTION
#include "sensitive/defaults.h"
#endif

#ifndef MY_FER_CENTRAL_UNIT_ID
#ifdef CONFIG_MY_FER_CENTRAL_UNIT_ID
#define MY_FER_CENTRAL_UNIT_ID CONFIG_MY_FER_CENTRAL_UNIT_ID
#else
#define MY_FER_CENTRAL_UNIT_ID 0x801234
#endif
#endif

#ifndef MY_MCU_UART_BAUD_RATE
#ifdef CONFIG_MY_MCU_UART_BAUD_RATE
#define MY_MCU_UART_BAUD_RATE CONFIG_MY_MCU_UART_BAUD_RATE
#else
#define MY_MCU_UART_BAUD_RATE 115200
#endif
#endif

// Berlin/Germany
#ifndef MY_GEO_LONGITUDE
#ifdef CONFIG_MY_GEO_LONGITUDE
#define MY_GEO_LONGITUDE CONFIG_MY_GEO_LONGITUDE
#else
#define MY_GEO_LONGITUDE +13.38
#endif
#endif

#ifndef MY_GEO_LATITUDE
#ifdef CONFIG_MY_GEO_LATITUDE
#define MY_GEO_LATITUDE CONFIG_MY_GEO_LATITUDE
#else
#define MY_GEO_LATITUDE +52.53
#endif
#endif

#ifndef MY_GEO_TZ
#ifdef CONFIG_MY_GEO_TZ
#define MY_GEO_TZ CONFIG_MY_GEO_TZ
#else
#define MY_GEO_TZ "CET-1CEST-2,M3.5.0,M10.5.0" // POSIX TZ format
#endif
#endif

#ifndef MY_GEO_TIMEZONE
#ifdef CONFIG_MY_GEO_TIMEZONE
#define MY_GEO_TIMEZONE CONFIG_MY_GEO_TIMEZONE
#else
#define MY_GEO_TIMEZONE +1.0
#endif
#endif

#ifndef MY_GEO_DST
#ifdef CONFIG_MY_GEO_DST
#define MY_GEO_DST CONFIG_MY_GEO_DST
#else
#define MY_GEO_DST dstEU
#endif
#endif



#ifndef MY_APP_CONFIG_PASSWORD
#ifdef CONFIG_MY_APP_CONFIG_PASSWORD
#define MY_APP_CONFIG_PASSWORD CONFIG_MY_APP_CONFIG_PASSWORD
#else
#define MY_APP_CONFIG_PASSWORD ""
#endif
#endif

#ifndef MY_APP_EXPERT_PASSWORD
#ifdef CONFIG_MY_APP_EXPERT_PASSWORD
#define MY_APP_EXPERT_PASSWORD CONFIG_MY_APP_EXPERT_PASSWORD
#else
#define MY_APP_EXPERT_PASSWORD ""
#endif
#endif

#ifndef MY_NETWORK_CONNECTION
#ifdef USE_LAN
#define MY_NETWORK_CONNECTION nwLan
#else
#define MY_NETWORK_CONNECTION nwWlanAp
#endif
#endif
#define MY_NETWORK_CONNECTION_OLD_USERS nwWlanSta

#ifndef MY_RFOUT_GPIO
#ifdef MCU_ESP32
#ifndef USE_LAN
#define MY_RFOUT_GPIO 22
#else
#define MY_RFOUT_GPIO 16
#endif
#elif defined MCU_ESP8266
#define MY_RFOUT_GPIO 4
#endif
#endif

#ifndef MY_RFIN_GPIO
#ifdef MCU_ESP32
#ifndef USE_LAN
#define MY_RFIN_GPIO 17
#else
#define MY_RFIN_GPIO 15 //XXX: GPIO15 is used by JTAG
#endif
#elif defined MCU_ESP8266
#define MY_RFIN_GPIO 5
#endif
#endif

#ifndef MY_SETBUTTON_GPIO
#ifdef MCU_ESP32
#define MY_SETBUTTON_GPIO -1
#elif defined MCU_ESP8266
#define MY_SETBUTTON_GPIO 0
#endif
#endif

/*
 esp-idf: kconfig does not support float numbers...
config MY_GEO_LONGITUDE
  float "geographical longitude"
  default +13.38

config MY_GEO_LATITUDE
  float "geographical latitude"
  default +52.53

 */

#ifndef MY_FER_GM_USE
#define MY_FER_GM_USE 0x77777777
#endif


