#pragma once

#include <cli/cli_types.h>
#include "cli/cli_json.h"

extern const char pin_state_args[];
extern const char pin_mode_args[];

extern const char cli_help_parmSend[];
extern const char cli_help_parmTimer[];
extern const char cli_help_parmConfig[];
extern const char cli_help_parmMcu[];
extern const char cli_help_parmPair[];
extern const char cli_help_parmShpref[];
extern const char cli_help_parmHelp[];

int process_parmSend(clpar p[], int len);
int process_parmConfig(clpar p[], int len);
int process_parmMcu(clpar p[], int len);
int process_parmTimer(clpar p[], int len);
int process_parmHelp(clpar p[], int len);
int process_parmPair(clpar p[], int len);
int process_parmShpref(clpar p[], int len);

void cli_out_set_x(const char *obj_tag);
void cli_out_close(void);
void cli_out_x_reply_entry(const char *key, const char *val, int len);
void cli_out_timer_reply_entry(const char *key, const char *val, int len);

