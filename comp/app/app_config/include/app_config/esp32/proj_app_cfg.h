/**
 * \file app_config/esp32/proj_app_cfg.h
 *
 */
#pragma once
#define MCU_TYPE "esp32"

#include <sdkconfig.h>

#ifdef UNIT_TESTING
#undef CONFIG_APP_USE_CLI_TASK
#undef CONFIG_APP_USE_NETWORK
#undef CONFIG_APP_USE_WLAN
#undef CONFIG_APP_USE_LAN
#undef CONFIG_APP_USE_WLAN_AP
#undef CONFIG_APP_USE_AP_FALLBACK
#undef CONFIG_APP_USE_MQTT
#undef CONFIG_APP_USE_WS
#define app_main APP_MAIN
#endif

#undef MDR_TIME

//////////////////////////////////////////////////////
#define ISO_BUILD_TIME (__DATE__ "T" __TIME__)

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define NO_SPIFFS
#define CONFIG_GPIO_SIZE 40


#define CFG_NAMESPACE "Tronferno" // XXX: backward compatibility

#include "esp_attr.h"



