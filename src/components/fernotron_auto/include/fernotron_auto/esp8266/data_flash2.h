/*
 * config_flash.h
 *
 *  Created on: 21.09.2017
 *      Author: bertw
 */



#ifndef USER_ESP8266_DATA_FLASH2_H_
#define USER_ESP8266_DATA_FLASH2_H_

#include "fernotron_auto/timer_data.h"
////////////////////////////////////////////////////////////////////////////////////////
// configuration start
//
// type of the data to save and restore. note: max size is 4092 bytes
#define DATA_TYPE timer_data_t
//
// reduce wear by use more than one flash sector
// more sectors mean more usable blocks, so each block is written to less often
#define FLASH_SECTORS 3
//
// configuration end
/////////////////////////////////////////////////////////////////////////////////////////


// save and read timer data on flash memory
// group  0...7
// member 0...7
// wildcard  if true, find the best matching data my return timer g=1 m=0 data on a search for g=1 m=1
//
// when saving data with group=0 or menber=0, all matching old data will be deleted. So g=0 m=0 will delete all other data, because it addresses all receivers
bool save_timer_data_old(DATA_TYPE *p, uint8_t group, uint8_t member);
bool read_timer_data_old(DATA_TYPE *p, uint8_t *group, uint8_t *member, bool wildcard);

#endif /* USER_ESP8266_DATA_FLASH_H_ */
