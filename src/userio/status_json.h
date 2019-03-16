/*
 * status_json.h
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */

#ifndef SRC_USERIO_STATUS_JSON_H_
#define SRC_USERIO_STATUS_JSON_H_

#include <stdbool.h>

void so_json_config_reply(const char *key, const char *val, bool is_number);
const char *sj_timer2json(uint8_t g, uint8_t m);

extern void (*s_json_config_out)(const char *s);

#endif /* SRC_USERIO_STATUS_JSON_H_ */
