/*
 * spiffs_fs.h
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */

#ifndef USER_MAIN_SPIFFS_FS_H_
#define USER_MAIN_SPIFFS_FS_H_

#if ENABLE_SPIFFS

#include "spiffs_config.h"
#include "spiffs.h"

extern spiffs fs;
#define fs_A (&fs + 0)  // fs_A - our flash file system. sizes is configured in fer_app_config.h

bool spiffs_format_fs(spiffs *fs);  // unmout, format, mount. returns success

#endif /* ENABLE_SPIFFS */
#endif /* USER_MAIN_SPIFFS_FS_H_ */
