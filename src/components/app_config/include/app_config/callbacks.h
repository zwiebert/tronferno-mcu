#ifndef COMPONENTS_APP_CONFIG_INCLUDE_APP_CONFIG_CALLBACKS_H_
#define COMPONENTS_APP_CONFIG_INCLUDE_APP_CONFIG_CALLBACKS_H_

#include "proj_app_cfg.h"

#ifdef MCU_ESP32
#include "esp32/esp32_callbacks.h"
#endif

#ifdef MCU_ESP8266
#include "esp8266/esp8266_callbacks.h"
#endif


#endif /* COMPONENTS_APP_CONFIG_INCLUDE_APP_CONFIG_CALLBACKS_H_ */
