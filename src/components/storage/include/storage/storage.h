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

bool write_to_file(const char *path, const void *src, size_t len);
bool read_from_file(const char *path, void *src, size_t len);
bool delete_file(const char *path);

void storage_setup(void);

#endif /* USER_MAIN_STORAGE_H_ */
