#pragma once
#include "cli/cli.h"


extern const char cli_help_parmSend[];
extern const char cli_help_parmTimer[];
extern const char cli_help_parmConfig[];
extern const char cli_help_parmMcu[];
extern const char cli_help_parmPair[];
extern const char cli_help_parmShpref[];
extern const char cli_help_parmHelp[];

int process_parmSend(clpar p[], int len, const struct TargetDesc &td);
int process_parmConfig(clpar p[], int len, const struct TargetDesc &td);
int process_parmMcu(clpar p[], int len, const struct TargetDesc &td);
int process_parmTimer(clpar p[], int len, const struct TargetDesc &td);
int process_parmHelp(clpar p[], int len, const struct TargetDesc &td);
int process_parmPair(clpar p[], int len, const struct TargetDesc &td);
int process_parmShpref(clpar p[], int len, const struct TargetDesc &td);



extern void (*mcu_restart_cb)();

void cliApp_setup();
