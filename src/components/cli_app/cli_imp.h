#pragma once
#include "cli/cli.h"
#include "cli_app/cli_app.h"





#define NODEFAULT() if (val==0) return cli_replyFailure()


void cli_out_config_reply_entry(const char *key, const char *val, int len);
void cli_out_mcu_reply_entry(const char *key, const char *val, int len);


bool config_receiver(const char *val);
bool config_transmitter(const char *val);

int asc2bool(const char *s);
bool asc2u8(const char *s, uint8_t *n, uint8_t limit);

