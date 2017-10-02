/*
* config.c
*
* Created: 24.08.2017 20:59:42
*  Author: bertw
*/


#include "../main/config.h"
#include <avr/eeprom.h>


config C;

struct ee_config {
  uint32_t zero;
  config c;
  uint8_t reserve[64];
  };
  
  
  config  ee_cfg = {MY_FER_CENTRAL_UNIT_ID, MY_MCU_ATMEGA328_BAUD_RATE,
   MY_GEO_LONGITUDE, MY_GEO_LATITUDE, MY_GEO_TIMEZONE, MY_GEO_DST, 0, recvTick, transmTick, rtcAvrTime  };
  //FIXME: transmTick does not make it into the eep file

struct ee_config tmp;

void read_config() {
  memcpy(&C, &ee_cfg, sizeof (config));
 //eeprom_read_block(&C, &ee_cfg, sizeof(config));
}

void save_config() {
  eeprom_update_block(&C, &ee_cfg, sizeof(config));
}

#if TEST_MODULE_CONFIG

bool testModule_config()
{

  return true;
}
#endif
