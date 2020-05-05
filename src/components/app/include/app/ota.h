/*
 * ota.h
 *
 *  Created on: 17.05.2019
 *      Author: bertw
 */

#pragma once

#include "app_config/proj_app_cfg.h"
#include <stdbool.h>

#ifdef USE_LAN
#define OTA_FWURL_MASTER "https://raw.githubusercontent.com/zwiebert/tronferno-mcu-bin/master/firmware/esp32/tronferno-mcu-lan.bin"
#define OTA_FWURL_BETA "https://raw.githubusercontent.com/zwiebert/tronferno-mcu-bin/beta/firmware/esp32/tronferno-mcu-lan.bin"
#else
#define OTA_FWURL_MASTER "https://raw.githubusercontent.com/zwiebert/tronferno-mcu-bin/master/firmware/esp32/tronferno-mcu.bin"
#define OTA_FWURL_BETA "https://raw.githubusercontent.com/zwiebert/tronferno-mcu-bin/beta/firmware/esp32/tronferno-mcu.bin"
#endif

#define OTA_FWURL_TAG_HEAD "https://raw.githubusercontent.com/zwiebert/tronferno-mcu-bin/"
#define OTA_FWURL_TAG_TAIL "/firmware/esp32/tronferno-mcu.bin"
#define OTA_FWURL_TAG_COOKIE "tag:"

// downloads firmwareUpdate into OTA partion
// if success you should can call esp_restart() to run the new firwmare
bool ota_doUpdate(const char *firmwareURL);

typedef enum { ota_NONE, ota_RUN, ota_FAIL, ota_DONE} ota_state_T;
ota_state_T ota_getState(void);


