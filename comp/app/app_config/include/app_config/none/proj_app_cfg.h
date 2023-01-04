/**
 * \file app_config/none/proj_app_cfg.h
 * \brief Provide defines for conditional compiling to make all code visible in IDE
 * \note  This will never be part of an actual compile.
 */
#define MCU_TYPE "none"

#include "./sdkconfig.h"
///////////////////////////////////////////////

#define IRAM_ATTR
#undef CONFIG_APP_USE_GPIO_PINS
