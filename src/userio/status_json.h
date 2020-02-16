/*
 * status_json.h
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */

#ifndef SRC_USERIO_STATUS_JSON_H_
#define SRC_USERIO_STATUS_JSON_H_

#include <stdbool.h>
#include "status_output.h"

void sj_open_dictionary(const char *name);
void sj_append_to_dictionary(const char *key, const char *val, bool is_number);
void sj_close_dictionary();

extern void (*sj_callback_onClose_ifNotEmpty)(const char *json);

void sj_fillBuf_with_automaticData(char *dst, u16 dst_size, u8 g, u8 m, bool wildcard);
const char *sj_json_from_automaticData(u8 g, u8 m);

char *sj_fillBuf_with_positionData(char *dst, u16 dst_size, so_arg_gmp_t *gmp);
const char *sj_json_from_postionData(so_arg_gmp_t *gmp);

int sj_fillBuf_with_allConfigData(char *dst, u16 dst_size);


// set json buf to dst, so we can read back our json when created or an empty string if nothing was created
// if dst is NULL, reset json buf to defaul
void sj_set_buf(char *dst, u16 dst_size);

#endif /* SRC_USERIO_STATUS_JSON_H_ */
