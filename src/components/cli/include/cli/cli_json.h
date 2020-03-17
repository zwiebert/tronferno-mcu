/*
 * cli_json.h
 *
 *  Created on: 17.03.2020
 *      Author: bertw
 */

#ifndef COMPONENTS_CLI_APP_INCLUDE_CLI_APP_CLI_JSON_H_
#define COMPONENTS_CLI_APP_INCLUDE_CLI_APP_CLI_JSON_H_

#include "userio/status_output.h"

char *json_get_command_object(char *s, char **ret_name, char **next);
int parse_json(char *name, char *s);
void cli_process_json(char *json, so_target_bits tgt);
void cli_print_json(const char *json);

#endif /* COMPONENTS_CLI_APP_INCLUDE_CLI_APP_CLI_JSON_H_ */
