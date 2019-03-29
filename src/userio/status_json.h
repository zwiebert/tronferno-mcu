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

void so_json_config_reply(const char *key, const char *val, bool is_number);
extern void (*s_json_config_out)(const char *s);


void sj_timer2json_buf(char *dst, uint8_t dst_size, uint8_t g, uint8_t m, bool wildcard);
const char *sj_timer2json(uint8_t g, uint8_t m);

int sj_config2json_buf(char *dst, uint16_t dst_size, so_msg_t key);

#endif /* SRC_USERIO_STATUS_JSON_H_ */
