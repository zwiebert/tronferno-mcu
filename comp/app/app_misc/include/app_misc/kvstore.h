/*
 * kvstore.h
 *
 *  Created on: 12.06.2019
 *      Author: bertw
 */

#ifndef MAIN_KVSTORE_H_
#define MAIN_KVSTORE_H_

#include <stdbool.h>
#include "utils_misc/int_types.h"

bool kvs_store_string(const char *key, const char *val);
bool kvs_get_string(const char *key, char *buf, size_t buf_size);

bool kvs_store_int32(const char *key, i32 val);
bool kvs_get_int32(const char *key, i32 *dest);

#endif /* MAIN_KVSTORE_H_ */
