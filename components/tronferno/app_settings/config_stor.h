/*
 * config_spiffs.h
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */

#pragma once


#include <stdbool.h>

// tries to copy saved data to DATA_PTR, or does nothing if no saved data exists
// note: read_data() must be called at least once in the program before calling save_data()
bool config_read(void);


// stores data to flash ROM.
// call read_data() at least once in the program before the first calling of save_data()!
bool config_save(void);



