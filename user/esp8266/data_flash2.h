/*
 * config_flash.h
 *
 *  Created on: 21.09.2017
 *      Author: bertw
 */



#ifndef USER_ESP8266_DATA_FLASH2_H_
#define USER_ESP8266_DATA_FLASH2_H_

#include "../main/fer_code.h"

typedef struct {
	int16_t astro;
	uint8_t bf;
	char daily[8+1];
	char weekly[7*4+1];
} timer_data_t;

#define td_is_astro(td) ((td)->astro < 20000)
#define td_is_daily(td) ((td)->daily[0] != '\0')
#define td_is_weekly(td) ((td)->weekly[0] != '\0')
#define td_is_random(td) ((td)->bf & (1 << flag_Random))
#define td_is_sun_auto(td) ((td)->bf & (1 << flag_SunAuto))

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
bool save_timer_data(DATA_TYPE *p, uint8_t group, uint8_t member);
bool read_timer_data(DATA_TYPE *p, uint8_t *group, uint8_t *member, bool wildcard);

#endif /* USER_ESP8266_DATA_FLASH_H_ */
