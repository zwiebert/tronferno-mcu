/**
 * \file  app_config/proj_app_cfg.h
 * \brief application configuration
 */

#pragma once

#if defined TEST_HOST
#include "app_config/host/proj_app_cfg.h"

#elif defined MCU_ESP32
#include "app_config/esp32/proj_app_cfg.h"

#else
#include "app_config/none/proj_app_cfg.h"
#error "no supported MCU"
#define MCU_ESP32
#endif



