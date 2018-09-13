// User configuration
#ifndef USER_CONFIG_H_
#define USER_CONFIG_H_
#include "main/common.h"



// ESP8266 configuration
#ifdef MCU_ESP8266

// SPIFFS is currently unused
#define ENABLE_SPIFFS 1

#define LEGACY_STORAGE_READ 1
#define LEGACY_STORAGE_WRITE 0
#define LEGACY_STORAGE_2_READ 1
#define LEGACY_STORAGE_2_WRITE 0

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

#define ENABLE_SET_ENDPOS 1

#endif /* USER_CONFIG_H_ */
