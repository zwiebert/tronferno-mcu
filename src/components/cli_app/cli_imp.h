#pragma once
#include "cli/cli.h"
#include "cli_app/cli_app.h"
#include <uout/cli_out.h>




#define NODEFAULT() if (val==0) return cli_replyFailure()



bool config_receiver(const char *val);
bool config_transmitter(const char *val);

int asc2bool(const char *s);
bool asc2u8(const char *s, uint8_t *n, uint8_t limit);

