#pragma once

#define MCU_TYPE "esp32"

//extern const char *const build_date;
#define ISO_BUILD_TIME (__DATE__ "T" __TIME__)

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

//#undef FER_TRANSMITTER
//#undef FER_RECEIVER
//#undef USE_NTP
//#undef USE_WLAN
//#undef USE_MQTT
#define NO_SPIFFS
#undef CONFIG_BLOB
//#undef ACCESS_GPIO
#define CONFIG_GPIO_SIZE 40
//#undef POSIX_TIME
#undef MDR_TIME
//#undef USE_PAIRINGS

#ifndef FLAVOR_LAN
#undef USE_LAN
#endif

#ifdef DISTRIBUTION
#undef USE_TCPS
#else
#undef USE_TCPS
#endif

#define CFG_NAMESPACE "Tronferno" // XXX: backward compatibility


#include "esp_attr.h"

#undef USE_TIMER_DATA_FS


#ifdef UNIT_TESTING
#undef USE_CLI_TASK
#undef USE_NETWORK
#undef USE_WLAN
#undef USE_LAN
#undef USE_WLAN_AP
#undef USE_AP_FALLBACK
#undef USE_MQTT
#endif

