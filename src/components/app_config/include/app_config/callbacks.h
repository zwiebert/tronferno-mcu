#pragma once

#include "proj_app_cfg.h"

#ifndef TEST_HOST

#ifdef MCU_ESP32
#include "esp32/esp32_callbacks.h"
#endif

#ifdef MCU_ESP8266
#include "esp8266/esp8266_callbacks.h"
#endif

#endif

