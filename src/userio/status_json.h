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

void so_json_set_x(const char *tag);
void so_json_x_reply(const char *key, const char *val, bool is_number);

extern void (*s_json_config_out)(const char *s);
void sj_timer2json_buf(char *dst, u16 dst_size, u8 g, u8 m, bool wildcard);
const char *sj_timer2json(u8 g, u8 m);
char *sj_gmp2json_buf(char *dst, u16 dst_size, so_arg_gmp_t *gmp);
const char *sj_gmp2json(so_arg_gmp_t *gmp);
int sj_config2json_buf(char *dst, u16 dst_size, so_msg_t key);


// set json buf to dst, so we can read back our json when created or an empty string if nothing was created
// if dst is NULL, reset json buf to defaul
void sj_set_buf(char *dst, u16 dst_size);

#endif /* SRC_USERIO_STATUS_JSON_H_ */
