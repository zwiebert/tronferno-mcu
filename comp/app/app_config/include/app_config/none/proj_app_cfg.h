/**
 * \file app_config/none/proj_app_cfg.h
 * \brief Provide defines for conditional compiling to make all code visible in IDE
 * \note  This will never be part of an actual compile.
 */
#define MCU_TYPE "none"

#include "./sdkconfig.h"
///////////////////////////////////////////////

#define IRAM_ATTR
#undef CONFIG_BLOB
//#undef CONFIG_DICT
#undef ACCESS_GPIO
#undef MDR_TIME

#define NO_SPIFFS

#undef MDR_TIME
#define MY_RFOUT_GPIO -1
#define MY_RFIN_GPIO -1
#define MY_SETBUTTON_GPIO -1
#define ISO_BUILD_TIME (__DATE__ "T" __TIME__)

