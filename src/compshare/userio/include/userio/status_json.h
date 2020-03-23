/*
 * status_json.h
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */

#ifndef SRC_USERIO_STATUS_JSON_H_
#define SRC_USERIO_STATUS_JSON_H_

#include <stdbool.h>
#include <stdlib.h>

bool sj_open_root_object(const char *id);
bool sj_add_object(const char *key);
bool sj_add_key_value_pair_s(const char *key, const char *val);
bool sj_add_key_value_pair_d(const char *key, int val);
bool sj_add_key_value_pair_f(const char *key, float val);
void sj_close_object();
void sj_close_root_object();

// copy/cat some json int buf
bool sj_copy_to_buf(const char *s);
bool sj_cat_to_buf(const char *s);

extern void (*sj_callback_onClose_ifNotEmpty)(const char *json);

bool sj_realloc_buffer(size_t buf_size);
char *sj_get_json();
void sj_free_buffer();

#endif /* SRC_USERIO_STATUS_JSON_H_ */
