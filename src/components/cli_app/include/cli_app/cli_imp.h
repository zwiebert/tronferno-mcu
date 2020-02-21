#ifndef cli_imp_h_
#define cli_imp_h_
#include "cli/cli.h"


#define CMD_BUF_SIZE 128
extern char cmd_buf[CMD_BUF_SIZE];

// implementation interface for  cli*.c files
extern const char pin_state_args[];
extern bool cli_isJson;


#define MAX_PAR 20
extern clpar par[MAX_PAR];

extern const char help_parmSend[] ;
extern const char help_parmTimer[] ;
extern const char help_parmConfig[] ;
extern const char help_parmMcu[] ;
extern const char help_parmPair[] ;
extern const char help_parmHelp[] ;

extern uint16_t msgid;


#define NODEFAULT() if (val==0) return reply_failure()


void cli_out_timer_reply_entry(const char *key, const char *val, int len);
void cli_out_config_reply_entry(const char *key, const char *val, int len);
void cli_out_mcu_reply_entry(const char *key, const char *val, int len);
void cli_out_set_x(const char *obj_tag);
void cli_out_close(void);
void cli_out_x_reply_entry(const char *key, const char *val, int len);


void cli_msg_ready(void);
bool config_receiver(const char *val);
bool config_transmitter(const char *val);


void cli_print_json(const char *json); //FIXME

#endif
