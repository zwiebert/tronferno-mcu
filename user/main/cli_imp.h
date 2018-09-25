#ifndef cli_imp_h_
#define cli_imp_h_

#include "cli.h"



// implementation interface for  cli_xxx.c files
extern const char pin_state_args[];


typedef struct {
  char *key;
  char *val;
} clpar;

extern const char help_parmSend[] PROGMEM;
extern const char help_parmTimer[] PROGMEM;
extern const char help_parmConfig[] PROGMEM;
extern const char help_parmMcu[] PROGMEM;
extern const char help_parmHelp[] PROGMEM;
extern const char help_None[] PROGMEM;

extern fer_sender_basic default_sender, last_received_sender;
extern uint16_t msgid;
extern clpar par[];

#define NODEFAULT() if (val==0) return reply_failure()


/* cli.c */
/*static*/ void print_enr(void);
/*static*/ void msg_print(const char *msg, const char *tag);
/*static*/ void reply_print(const char *tag);
/*static*/ void reply_message(const char *tag, const char *msg);
/*static*/ char *skip_leading_whitespace(char *s);
/*static*/ char *find_next_space_eq_eol(char *s);
/*static*/ char *find_next_whitespace_or_eol(char *s);
/*static*/ int process_parmSend(clpar p[], int len);
/*static*/ int process_parmConfig(clpar p[], int len);
/*static*/ int process_parmMcu(clpar p[], int len);
/*static*/ int process_parmTimer(clpar p[], int len);
/*static*/ int process_parmHelp(clpar p[], int len);
/* cli.c */
void warning_unknown_option(const char *key);
void cli_msg_ready(void);
char *get_commandline(void);
int asc2bool(const char *s);
int parse_commandline(char *cl);
bool cli_parm_to_ferCMD(const char *token, fer_cmd *cmd);
void reply_success(void);
int reply_failure(void);
bool config_receiver(const char *val);
bool config_transmitter(const char *val);
bool reply(bool success);
fer_sender_basic *get_sender_by_addr(long addr);
bool cu_auto_set(unsigned init_seconds);
bool asc2group(const char *s, fer_grp *grp);
bool asc2memb(const char *s, fer_memb *memb);
bool string2bcdArray(const char *src, uint8_t *dst, uint16_t size_dst);
int process_parm(clpar p[], int len);
void process_cmdline(char *line);
void cli_loop(void);

#endif
