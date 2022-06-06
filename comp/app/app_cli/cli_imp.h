#pragma once
#include "cli/cli.h"
#include "app_cli/cli_app.h"
#include <uout/cli_out.h>



int process_parmSend(clpar p[], int len, const struct TargetDesc &td);
int process_parmConfig(clpar p[], int len, const struct TargetDesc &td);
int process_parmMcu(clpar p[], int len, const struct TargetDesc &td);
int process_parmTimer(clpar p[], int len, const struct TargetDesc &td);
int process_parmHelp(clpar p[], int len, const struct TargetDesc &td);
int process_parmPair(clpar p[], int len, const struct TargetDesc &td);
int process_parmSep(clpar p[], int len, const struct TargetDesc &td);
int process_parmShpref(clpar p[], int len, const struct TargetDesc &td);




#define NODEFAULT() if (val==0) return cli_replyFailure(td)



bool config_receiver(const char *val);
bool config_transmitter(const char *val);

int asc2bool(const char *s);
bool asc2u8(const char *s, uint8_t *n, uint8_t limit);

