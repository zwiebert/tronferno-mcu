/*
 * cli.h
 *
 * Created: 24.08.2017 21:46:06
 *  Author: bertw
 */ 


#ifndef CLI_H_
#define CLI_H_

#include "fer_code.h"

char *get_commandline(void);
void process_cmdline(char *line);

extern fer_sender_basic senders[10];

#endif /* CLI_H_ */