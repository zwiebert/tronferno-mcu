/*
 * storage.c
 *
 *  Created on: 13.09.2018
 *      Author: bertw
 */


/*
 * timer_data_spiffs.c
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"


#include "../../user_config.h"


#include "main/all.h"
#include "main/config.h"
#include "user_config.h"
#include "main/storage.h"
#include "main/debug.h"

#define TEST_THIS_MODULE 0

#if TEST_THIS_MODULE
#define DB(x) x
#define DB2(x)
#else
#define DB(x) ((C.app_verboseOutput >= vrbDebug) && (x),1)
#define DB2(x) DB(x)
#endif



///////////// implement read/ write from storage.h ////////////////////////

bool ICACHE_FLASH_ATTR
write_to_file(const char *path, const void *src, size_t len) {


  return true;

}

bool ICACHE_FLASH_ATTR
read_from_file(const char *path, void *dst, size_t len) {

   return true;
}

bool ICACHE_FLASH_ATTR
delete_file(const char *path) {

}


