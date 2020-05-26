#pragma once

#define MCU_TYPE "esp8266"

extern const char *const build_time;
#define ISO_BUILD_TIME build_time

#undef USE_MQTT
#undef USE_WS
#undef USE_HTTP
#undef USE_CLI_MUTEX
#undef USE_CLI_TASK
#undef USE_OTA
#undef USE_LAN
#undef WLAN_AP
#undef USE_TCPS_TASK
#undef USE_FREERTOS

#undef USE_ESP_GET_TIME
#define CONFIG_GPIO_SIZE 17
#undef POSIX_TIME
#undef USE_TXTIO_MUTEX


#include "user_interface.h"
#define IRAM_ATTR  __attribute__((section(".iram0.text")))

#include <osapi.h>

#define snprintf ets_snprintf
#define sprintf ets_sprintf

