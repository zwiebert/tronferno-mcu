/*
 * config_flash.h
 *
 *  Created on: 21.09.2017
 *      Author: bertw
 */

#ifndef USER_ESP8266_DATA_FLASH_H_
#define USER_ESP8266_DATA_FLASH_H_

// configure which data to save/restore  ///////////////////

// type of the data type (usually a struct)
// cannot be bigger than 4092 bytes
#define DATA_TYPE config

// pointer to the data (a global pointer or function returning a pointer)
#define DATA_PTR (&C)




// configure wear protection /////////////////////////////////

// how many 4096 byte flash sectors we want to use (the more sectors, the less wear for the flash ROM)
#define FLASH_SECTORS 4




// tries to copy saved data to DATA_PTR, or does nothing if not data was saved
 // note: read_data must be called at least once before any call to save_data in the program,
 // otherwise the wear leveling does not work
void read_data(void);

// stores data to flash ROM. does nothing if data hasn't been actually changed
void save_data(void);


#endif /* USER_ESP8266_DATA_FLASH_H_ */
