// User configuration
#ifndef USER_CONFIG_H_
#define USER_CONFIG_H_
#include "main/common.h"
#include "main/timer_data_fs.h"


#define APP_VERSION "0.5.1"

// ESP8266 configuration
#if defined MCU_ESP8266
#include "esp8266/user_config.h"
#elif defined MCU_ESP32
#include "esp32/main/user_config.h"
#elif defined  MCU_ATMEGA328
#include "atmega328/user_config.h"
#endif

#define ENABLE_SET_ENDPOS 1

#endif /* USER_CONFIG_H_ */
