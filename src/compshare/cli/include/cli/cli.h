/*
 * cli.h
 *
 * Created: 24.08.2017 21:46:06
 *  Author: bertw
 */


#ifndef CLI_H_
#define CLI_H_

#include "userio/status_output.h"
#include "stdint.h"
#include "stdbool.h"

// user interface
char *get_commandline(void);
char *set_commandline(const char *src, uint8_t len); // copy src  (e.g. from mqtt event) to writable buffer
void cli_process_cmdline(char *line, so_target_bits tgt);
void cli_process_json(char *json, so_target_bits tgt);
void cli_loop(void);

// implementation interface
typedef struct {
  char *key;
  char *val;
} clpar;

struct parm_handler {
  const char *parm;
  int (*process_parmX)(clpar p[], int len);
  const char *help;
};

struct parm_handlers {
  struct parm_handler *handlers;
  int count;
};

extern const struct parm_handlers parm_handlers; // defined by app

extern uint16_t cli_msgid;
#define MAX_PAR 20
extern clpar cli_par[MAX_PAR];
extern bool cli_isJson;

#define CMD_BUF_SIZE 128
extern char cmd_buf[CMD_BUF_SIZE];

void print_enr(void);
void msg_print(const char *msg, const char *tag);
void cli_reply_print(const char *tag);
void reply_message(const char *tag, const char *msg);
void reply_id_message(uint16_t id, const char *tag, const char *msg);

void cli_out_set_x(const char *obj_tag);
void cli_out_close(void);
void cli_out_x_reply_entry(const char *key, const char *val, int len);


/* cli.c */
void cli_warning_optionUnknown(const char *key);
void cli_msg_ready(void);
char *get_commandline(void);
int asc2bool(const char *s);
int cli_parseCommandline(char *cl);
void cli_replySuccess(void);
int cli_replyFailure(void);
bool cli_replyResult(bool success);
bool  asc2uint8_t(const char *s, uint8_t *n, uint8_t limit);
int cli_processParameters(clpar p[], int len);
void cli_loop(void);
void cli_print_json(const char *json); //FIXME



#endif /* CLI_H_ */
