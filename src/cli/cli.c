/*
 * cli.c
 *
 * Created: 24.08.2017 21:45:48
 *  Author: bertw
 */
#include "user_config.h"


#include <string.h>

#include "../motor_setup/set_endpos.h"
#include "../shutter_positions/current_state.h"
#include "config/config.h"
#include "automatic/timer_state.h"
#include "misc/bcd.h"
#include "cli_imp.h"
#include "userio/status_output.h"
#include "automatic/timer_data.h"
#include "mutex.h"

#define ENABLE_RESTART 1 // allow software reset
#define ENABLE_TIMER_WDAY_KEYS 0  // allow timer mon=T tue=T sun=T  additional to weekly=TTTTTTT  (a waste of resources)
#define FSB_PLAIN_REPEATS 2  // send plain commands 1+N times (if 0, send only once without repeating)

fer_sender_basic default_sender;
fer_sender_basic last_received_sender;
u16 msgid;

const char *Obj_tag="";
#define SET_OBJ_TAG(tag) Obj_tag=(tag)
#define OBJ_TAG (Obj_tag+0)
#define OBJ_TAG_TIMER "timer"
#define OBJ_TAG_CONFIG "config"
#define OBJ_TAG_MCU "mcu"
#define OBJ_TAG_SEND "send"


static void  cli_out_top_tag(void) {
    io_puts("tf: ");
}

static void  cli_out_reply_tag(void) {
  if (msgid) {
    io_puts("reply="), io_putd(msgid), io_puts(": ");
  } else {
    io_puts("reply: ");
  }
}

static void  cli_out_obj_tag(void) {
    io_puts(OBJ_TAG), io_puts(":");
}

static void  cli_out_start_reply(void) {
  cli_out_top_tag();
  cli_out_reply_tag();
  cli_out_obj_tag();
}


static void  cli_out_start_timer_reply(void) {
  SET_OBJ_TAG(OBJ_TAG_TIMER);
  cli_out_start_reply();
}


static void  cli_out_start_config_reply(void) {
  SET_OBJ_TAG(OBJ_TAG_CONFIG);
  cli_out_start_reply();
}

static void  cli_out_start_mcu_reply(void) {
  SET_OBJ_TAG(OBJ_TAG_MCU);
  cli_out_start_reply();
}


typedef void (*void_fun_ptr)(void);
const int OUT_MAX_LEN = 80;

static void  cli_out_entry(void_fun_ptr tag, const char *key, const char *val, int len) {
  static int length;

  if (!key || len == -1) {
    if (length) {
      io_puts(";\n");
      length = 0;
    }
  } else {
    if (key)
      len += strlen(key) + 1;
    if (val)
      len += strlen(val);

    if ((length + len) > OUT_MAX_LEN && length > 0) {
      io_puts(";\n");
      length = 0;
    }
    if (length == 0) {
      tag();
    }
    length += len;
    if (key) {
      io_putc(' '), io_puts(key), io_putc('=');
    }
    if (val) {
      io_puts(val);
    }
  }
}

void   cli_out_set_config(void) {
  SET_OBJ_TAG(OBJ_TAG_CONFIG);
}

void   cli_out_set_x(const char *obj_tag) {
  SET_OBJ_TAG(obj_tag);
}
void cli_out_close() {
  cli_out_x_reply_entry(0, 0, 0);
}
void  cli_out_x_reply_entry(const char *key, const char *val, int len) {
  if (!so_tgt_test_cli_text())
    return;
  cli_out_entry(cli_out_start_reply, key, val, len);
}

void  cli_out_timer_reply_entry(const char *key, const char *val, int len) {
  if (!so_tgt_test_cli_text())
    return;
  cli_out_entry(cli_out_start_timer_reply, key, val, len);
}

void  cli_out_config_reply_entry(const char *key, const char *val, int len) {
  if (!so_tgt_test_cli_text())
    return;
  cli_out_entry(cli_out_start_config_reply, key, val, len);
}

void  cli_out_mcu_reply_entry(const char *key, const char *val, int len) {
  if (!so_tgt_test_cli_text())
    return;
  cli_out_entry(cli_out_start_mcu_reply, key, val, len);
}

int ENR; // error number
void  print_enr(void) {
  io_puts("enr: "), io_putd(ENR), io_putlf();
}

void  msg_print(const char *msg, const char *tag) {
  if (!so_tgt_test_cli_text())
    return;
  if (msg)
    io_puts(msg);
  if (msgid) {
    io_putc('@');
    io_putd(msgid);
  }
  if (tag) {
    io_putc(':');
    io_puts(tag);
  }
  io_puts(": ");
}

void  warning_unknown_option(const char *key) {
  if (!so_tgt_test_cli_text())
    return;
  msg_print("warning", "unknown-option"), io_puts(key), io_putc('\n');
}

void  reply_print(const char *tag) {
  if (!so_tgt_test_cli_text())
    return;
  msg_print("reply", tag);
}

void  reply_message(const char *tag, const char *msg) {
  if (!so_tgt_test_cli_text())
    return;
  reply_print(tag);
  if (msg)
    io_puts(msg);
  io_putlf();
}

void  cli_msg_ready(void) {
  if (!so_tgt_test_cli_text())
    return;
  io_puts("\nready:\n");
}

void  reply_id_message(u16 id, const char *tag, const char *msg) {
  u16 old_id = msgid;
  if (!so_tgt_test_cli_text())
    return;

  msgid = id;
  reply_print(tag);
  if (msg)
    io_puts(msg);
  io_putlf();
  msgid = old_id;
}






int 
asc2bool(const char *s) {
  if (!s)
    return 1; // default value for key without value

  switch (*s) {
  case '0':
    return 0;
  case '1':
    return 1;
  default:
    return -1;
  }

}

struct c_map {
  const char *fs;
  fer_cmd fc;
};

struct c_map const fc_map[] = { { "down", fer_cmd_DOWN }, { "up", fer_cmd_UP },
                                { "stop", fer_cmd_STOP }, { "sun-down", fer_cmd_SunDOWN },
                                { "sun-up", fer_cmd_SunUP }, { "sun-inst", fer_cmd_SunINST },
                                //		{"sun-test", fer_cmd_Program},
                                { "set", fer_cmd_SET }, };

bool 
cli_parm_to_ferCMD(const char *token, fer_cmd *cmd) {
  int i;
  dbg_trace();

  for (i = 0; i < (sizeof(fc_map) / sizeof(fc_map[0])); ++i) {
    if (strcmp(token, fc_map[i].fs) == 0) {
      *cmd = fc_map[i].fc;
      return true;
    }
  }
  return false;
}

void 
reply_success() {
  reply_message(0, "ok");
}

int 
reply_failure() {
  reply_message(0, "error");
  return -1;
}

bool  config_receiver(const char *val) {
  if (strcmp(val, "on") == 0) {
    C.app_recv = recvTick;
  } else if (strcmp(val, "off") == 0) {
    C.app_recv = recvNone;
  } else {
    return false;
  }
  save_config(CONFIG_RECV);
  return true;
}

bool 
config_transmitter(const char *val) {
  if (strcmp(val, "on") == 0) {
    C.app_transm = transmTick;
  } else if (strcmp(val, "off") == 0) {
    C.app_transm = transmNone;
  } else {
    return false;
  }
  save_config(CONFIG_TRANSM);
  return true;
}

bool 
reply(bool success) {
  if (success)
    reply_success();
  else
    reply_failure();
  return success;
}

fer_sender_basic * 
get_sender_by_addr(long addr) {
  if (addr == 0) {
    return &default_sender;
  } else if (addr > 0 && 10 > addr) {
    //return &senders[addr];
  }
  return NULL ;
}


bool  asc2group(const char *s, fer_grp *grp) {
  if (s) {
    int g = atoi(s);
    if (0 <= g && g <= 7) {
      *grp = g;
      return true;
    }
  }
  return false;

  return (fer_grp) atoi(s);
}

bool  asc2memb(const char *s, fer_memb *memb) {
  if (s) {

    int m = atoi(s);
    if (0 <= m && m <= 7) {
      *memb = (m == 0 ? fer_memb_Broadcast : (fer_memb) (m - 1) + fer_memb_M1);
      return true;
    }
  }
  return false;
}



bool 
timerString2bcd(const char *src, u8 *dst, u16 size_dst) {
  char buf[3];
  int i;

  buf[2] = 0;

  for (i = 0; i < size_dst && src[0]; ++i) {
    switch (*src) {
    case '-': // timer off
      dst[i] = (i % 2 == 0) ? 0xff : 0x0f;
      if (!(++i < size_dst))
        return false;
      ;
      dst[i] = (i % 2 == 0) ? 0xff : 0x0f;
      ++src;
      break;
    case '+': // copy previous day on+off timer
      if (i < FPR_TIMER_STAMP_WIDTH || i + FPR_TIMER_STAMP_WIDTH > size_dst)
        return false;
      dst[i] = dst[i - FPR_TIMER_STAMP_WIDTH], ++i;
      dst[i] = dst[i - FPR_TIMER_STAMP_WIDTH], ++i;
      dst[i] = dst[i - FPR_TIMER_STAMP_WIDTH], ++i;
      dst[i] = dst[i - FPR_TIMER_STAMP_WIDTH];
      ++src;
      break;

    default:
      buf[0] = *src++;
      buf[1] = *src++;
      if (!(++i < size_dst))
        return false;
      ;
      dst[i] = dec2bcd(atoi(buf));
      buf[0] = *src++;
      buf[1] = *src++;
      dst[i - 1] = dec2bcd(atoi(buf));

    }

  }

  return i == size_dst;
}


int process_parmHelp(clpar p[], int len);

const char help_None[]  = "none";

const char help_parmHelp[]  =
"type 'help command;'  or 'help all;'\ncommands are: ";

struct {
  const char *parm;
  int (*process_parmX)(clpar p[], int len);
  const char *help;
} parm_handlers[] = { { "send", process_parmSend, help_parmSend },
                      { "cmd", process_parmSend, help_parmSend }, // alias
                      { "config", process_parmConfig, help_parmConfig },
                      { "mcu", process_parmMcu, help_parmMcu },
                      { "timer", process_parmTimer, help_parmTimer },
                      { "auto", process_parmTimer, help_parmTimer }, // alias
                      { "help", process_parmHelp, help_parmHelp },
#ifdef USE_PAIRINGS
                      { "pair", process_parmPair, help_parmPair},
#endif
  };

int 
process_parm(clpar p[], int len) {
  int i;
  int result = -1;

  if (mutex_cliTake()) {

    // if in sep mode, don't accept commands FIXME
    if (sep_is_enabled()) {
      sep_disable();
      reply_message(0, "error: CLI is disabled in set-endposition-mode\n");
    } else {
      for (i = 0; i < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++i) {
        if (strcmp(p[0].key, parm_handlers[i].parm) == 0) {
          result = parm_handlers[i].process_parmX(p, len);
          break;
        }
      }
    }
    mutex_cliGive();
  }

  return result;
}

void 
cli_process_cmdline(char *line) {
  dbg_vpf(db_printf("process_cmdline: %s\n", line));
  int n = parse_commandline(line);
  if (n < 0) {
    reply_failure();
  } else {
    process_parm(par, n);
  }
}

int 
process_parmHelp(clpar p[], int len) {
  int i;

  static const char usage[]  = "syntax: command option=value ...;\n"
      "commands are: ";

  io_putlf();

  // print help for help;
  if (len == 1) {
    io_puts(help_parmHelp);
    for (i = 0; i < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++i) {
      io_puts(parm_handlers[i].parm), io_puts(", ");
    }
    io_putlf();
    return 0;
  }

  // print help for help command; or help all;
  for (i = 1; i < len; ++i) {
    int k;
    const char *key = p[i].key; //, *val = p[i].val;
    bool keyProcessed = false;

    if (strcmp(key, "all") == 0) {
      io_puts(usage);
      for (i = 0; i < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++i) {
        io_puts(parm_handlers[i].parm), io_puts(", ");
      }
      io_putlf();

      for (i = 0; i < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++i) {
        io_puts(parm_handlers[i].parm), io_puts(" options:\n");
        io_puts(parm_handlers[i].help), io_putlf();
      }

    } else {

      for (k = 0; k < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++k) {
        if (strcmp(parm_handlers[k].parm, key) == 0) {
          io_puts(parm_handlers[k].help), io_putlf();
          keyProcessed = true;
          break;
        }
      }

      if (!keyProcessed) {
        warning_unknown_option(key);
      }
    }
  }

  io_puts("\ncommon options:\n"
      "mid=N  N is used as an ID in the reply\n");

  return 0;
}

void  cli_loop(void) {
  char *cmdline;
  static bool ready;
  if ((cmdline = get_commandline())) {
    so_tgt_set(SO_TGT_CLI);
    if (cmdline[0] == '{') {
     cli_process_json(cmdline);
    } else {
      io_putlf();
      cli_process_cmdline(cmdline);
      cli_msg_ready();
    }
    so_tgt_default();
  } else if (!ready) {
    cli_msg_ready();
    ready = true;
  }
}

#if TEST_MODULE_CLI
bool 
testModule_cli()
{
  char cl[] = "timer g=5 astro=0;";  //"timer g=2 m=2 weekly=08222000++++10552134+";
  int n = parse_commandline(cl);
  if (n > 0)
  process_parm(par, n);

  return n != -1;
}
#endif
