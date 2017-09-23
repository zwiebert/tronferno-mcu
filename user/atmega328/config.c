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
  uint8_t zero;
  config c;
  uint8_t reserve[64];
  };
  
struct ee_config EEMEM ee_cfg = {0, {MY_FER_CENTRAL_UNIT_ID, MY_MCU_ATMEGA328_BAUD_RATE,
   MY_GEO_LONGITUDE, MY_GEO_LATITUDE, MY_GEO_TIMEZONE, MY_GEO_DST, recvTick, transmTick, rtcAvrTime  }, };
  //FIXME: transmTick does not make it into the eep file


void read_config() {
  eeprom_read_block(&C, &ee_cfg.c, sizeof(config));
}

void save_config() {
  eeprom_update_block(&C, &ee_cfg.c, sizeof(config));
}

#if TEST_MODULE_CONFIG

bool testModule_config()
{

  return true;
}
#endif
