/*
 * storage.h
 *
 *  Created on: 13.09.2018
 *      Author: bertw
 */

#ifndef USER_MAIN_STORAGE_H_
#define USER_MAIN_STORAGE_H_

#include "stdbool.h"
#include "stddef.h"

bool stor_fileWrite(const char *path, const void *src, size_t len);
bool stor_fileRead(const char *path, void *src, size_t len);
bool stor_fileDelete(const char *path);

void stor_setup(void);

#endif /* USER_MAIN_STORAGE_H_ */
