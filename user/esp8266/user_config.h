#ifndef esp8266_user_config_h
#define esp8266_user_config_h

#include "../user_config.h"

#define MCU_TYPE "esp8266"

extern const char *const build_time;
#define ISO_BUILD_TIME build_time

#ifndef DEBUG
#define FER_TRANSMITTER
#define FER_RECEIVER
#define USE_NTP
#define USE_WLAN
#define CONFIG_BLOB 1
#define CONFIG_GPIO_SIZE 17
#endif

#define CONFIG_BLOB 1


// SPIFFS is currently unused
#define ENABLE_SPIFFS 0
#define POSIX_TIME 0

#if ENABLE_SPIFFS
#define SAVE_TIMER_DATA_FUN save_timer_data_fs
#define READ_TIMER_DATA_FUN read_timer_data_fs

#define LEGACY_STORAGE_READ 1
#define LEGACY_STORAGE_WRITE 0
#define LEGACY_STORAGE_2_READ 1
#define LEGACY_STORAGE_2_WRITE 0

#else
#define SAVE_TIMER_DATA_FUN save_timer_data_old
#define READ_TIMER_DATA_FUN read_timer_data_old
#endif

// define addresses with empty flash to use for flash data files
#define C_START_FREE_FLASH  0x100000UL   //start our data here (1MB)

// storage for MCU configuration
#define C_DATA_FLASH_ADDR  C_START_FREE_FLASH
#define C_DATA_FLASH_SIZE  0x10000  // 16 sectors (64KB)

// storage for timer data
#define C_DATA2_FLASH_ADDR  (C_DATA_FLASH_ADDR + C_DATA_FLASH_SIZE)
#define C_DATA2_FLASH_SIZE  0x10000 // 16 sectors (64KB)

// storage for SPIFFS, currently not used for anything
#define C_SPIFFS_FLASH_ADDR  (C_DATA2_FLASH_ADDR + C_DATA2_FLASH_SIZE)
#define C_SPIFFS_FLASH_SIZE   0x80000 // 128 sectors (500KB)

#endif
