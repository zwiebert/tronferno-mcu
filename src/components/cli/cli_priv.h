/*
 * cli_priv.h
 *
 *  Created on: 20.02.2020
 *      Author: bertw
 */

#ifndef COMPONENTS_CLI_CLI_PRIV_H_
#define COMPONENTS_CLI_CLI_PRIV_H_

#include "cli.h"
#include "stdbool.h"

#define CMD_BUF_SIZE 128
extern char cmd_buf[CMD_BUF_SIZE];

// implementation interface for  cli*.c files
extern const char pin_state_args[];
extern bool cli_isJson;

#define MAX_PAR 20
extern clpar par[MAX_PAR];

extern uint16_t msgid;


#define NODEFAULT() if (val==0) return reply_failure()

/* cli.c */
void print_enr(void);
void msg_print(const char *msg, const char *tag);
void reply_print(const char *tag);
void reply_message(const char *tag, const char *msg);
void reply_id_message(uint16_t id, const char *tag, const char *msg);

void cli_out_timer_reply_entry(const char *key, const char *val, int len);
void cli_out_config_reply_entry(const char *key, const char *val, int len);
void cli_out_mcu_reply_entry(const char *key, const char *val, int len);
void cli_out_set_x(const char *obj_tag);
void cli_out_close(void);
void cli_out_x_reply_entry(const char *key, const char *val, int len);


/* cli.c */
void warning_unknown_option(const char *key);
void cli_msg_ready(void);
char *get_commandline(void);
int asc2bool(const char *s);
int parse_commandline(char *cl);
void reply_success(void);
int reply_failure(void);
bool config_receiver(const char *val);
bool config_transmitter(const char *val);
bool reply(bool success);
bool  asc2uint8_t(const char *s, uint8_t *n, uint8_t limit);
int process_parm(clpar p[], int len);
void cli_process_cmdline(char *line);
void cli_loop(void);

void cli_print_json(const char *json); //FIXME

#endif /* COMPONENTS_CLI_CLI_PRIV_H_ */
