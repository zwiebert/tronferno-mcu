/*
 * cli.h
 *
 * Created: 24.08.2017 21:46:06
 *  Author: bertw
 */


#ifndef CLI_H_
#define CLI_H_
#include "fernotron/fer_code.h"


// user interface
char *get_commandline(void);
char *set_commandline(const char *src, uint8_t len); // copy src  (e.g. from mqtt event) to writable buffer
void process_cmdline(char *line);
void cli_loop(void);

extern fer_sender_basic senders[10];
extern fer_sender_basic default_sender, last_received_sender;

#endif /* CLI_H_ */
