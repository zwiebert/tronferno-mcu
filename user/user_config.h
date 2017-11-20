// User configuration
#ifndef USER_CONFIG_H_
#define USER_CONFIG_H_
#include "main/common.h"

// ESP8266 configuration
#ifdef MCU_ESP8266

#define ENABLE_SPIFFS 0

#define C_BASE_FLASH      0x100000UL   //1MB

#define C_DATA_FLASH_ADDR  C_BASE_FLASH
#define C_DATA_FLASH_SIZE  0x10000  // 16 sectors (64KB)

#define C_DATA2_FLASH_ADDR  (C_DATA_FLASH_ADDR + C_DATA_FLASH_SIZE)
#define C_DATA2_FLASH_SIZE  0x10000 // 16 sectors (64KB)

#define C_SPIFFS_FLASH_ADDR  (C_DATA2_FLASH_ADDR + C_DATA2_FLASH_SIZE)
#define C_SPIFFS_FLASH_SIZE   0x80000 // 128 sectors (500KB)


#endif




















#endif /* USER_CONFIG_H_ */
