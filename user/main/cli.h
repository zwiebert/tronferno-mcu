/*
 * cli.h
 *
 * Created: 24.08.2017 21:46:06
 *  Author: bertw
 */ 


#ifndef CLI_H_
#define CLI_H_

#include "fer_code.h"


// user interface
char *get_commandline(void);
void process_cmdline(char *line);
void cli_loop(void);

extern fer_sender_basic senders[10];


bool cu_auto_set(unsigned init_seconds);
bool pair_auto_set(unsigned init_seconds);

void cli_out_timer_reply_entry(const char *key, const char *val, int len);
void cli_out_config_reply_entry(const char *key, const char *val, int len);

#endif /* CLI_H_ */

