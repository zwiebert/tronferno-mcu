/*
 * config_flash.h
 *
 *  Created on: 21.09.2017
 *      Author: bertw
 */

#ifndef USER_ESP8266_DATA_FLASH_H_
#define USER_ESP8266_DATA_FLASH_H_



////////////////////////////////////////////////////////////////////////////////////////
// configuration start
//
// type of the data to save and restore. note: max size is 4092 bytes
#define DATA_TYPE config
//
// pointer to the data (global data or function returning a pointer)
#define DATA_PTR (&C)
//
// reduce wear by use more than one flash sector
// more sectors mean more usable blocks, so each block is written to less often
#define FLASH_SECTORS 4
//
// configuration end
/////////////////////////////////////////////////////////////////////////////////////////






// tries to copy saved data to DATA_PTR, or does nothing if no saved data exists
// note: read_data() must be called at least once in the program before calling save_data()
void read_data(void);


// stores data to flash ROM.
// call read_data() at least once in the program before the first calling of save_data()!
void save_data(void);


#endif /* USER_ESP8266_DATA_FLASH_H_ */