/*
 * cli.c
 *
 * Created: 24.08.2017 21:45:48
 *  Author: bertw
 */
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../user_config.h"
#include "current_state.h"
#include "timer_state.h"

#include "all.h"
#include "set_endpos.h"
#include "misc/bcd.h"
#include "cli_imp.h"

#include "timer_data.h"

#define ENABLE_RESTART 1 // allow software reset
#define ENABLE_TIMER_WDAY_KEYS 0  // allow timer mon=T tue=T sun=T  additional to weekly=TTTTTTT  (a waste of resources)
#define FSB_PLAIN_REPEATS 2  // send plain commands 1+N times (if 0, send only once without repeating)

fer_sender_basic default_sender;
fer_sender_basic last_received_sender;
uint16_t msgid;

int ENR; // error number
void ICACHE_FLASH_ATTR print_enr(void) {
  io_puts("enr: "), io_putd(ENR), io_putlf();
}

void ICACHE_FLASH_ATTR msg_print(const char *msg, const char *tag) {
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

void ICACHE_FLASH_ATTR warning_unknown_option(const char *key) {
  msg_print("warning", "unknown-option"), io_puts(key), io_putc('\n');
}

void ICACHE_FLASH_ATTR reply_print(const char *tag) {
  msg_print("reply", tag);
}

void ICACHE_FLASH_ATTR reply_message(const char *tag, const char *msg) {
  reply_print(tag);
  if (msg)
    io_puts(msg);
  io_putlf();
}

void ICACHE_FLASH_ATTR cli_msg_ready(void) {
  io_puts("\nready:\n");
}




int ICACHE_FLASH_ATTR
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

struct c_map const fc_map[] = { { "down", fer_cmd_DOWN }, { "up", fer_cmd_UP }, { "stop", fer_cmd_STOP }, { "sun-down", fer_cmd_SunDOWN }, { "sun-up",
    fer_cmd_SunUP }, { "sun-inst", fer_cmd_SunINST },
//		{"sun-test", fer_cmd_Program},
    { "set", fer_cmd_SET }, };

bool ICACHE_FLASH_ATTR
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

void ICACHE_FLASH_ATTR
reply_success() {
  reply_message(0, "ok");
}

int ICACHE_FLASH_ATTR
reply_failure() {
  reply_message(0, "error");
  return -1;
}

bool ICACHE_FLASH_ATTR config_receiver(const char *val) {
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

bool ICACHE_FLASH_ATTR
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

bool ICACHE_FLASH_ATTR
reply(bool success) {
  if (success)
    reply_success();
  else
    reply_failure();
  return success;
}

fer_sender_basic * ICACHE_FLASH_ATTR
get_sender_by_addr(long addr) {
  if (addr == 0) {
    return &default_sender;
  } else if (addr > 0 && 10 > addr) {
    //return &senders[addr];
  }
  return NULL ;
}


bool ICACHE_FLASH_ATTR asc2group(const char *s, fer_grp *grp) {
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

bool ICACHE_FLASH_ATTR asc2memb(const char *s, fer_memb *memb) {
  if (s) {

    int m = atoi(s);
    if (0 <= m && m <= 7) {
      *memb = (m == 0 ? fer_memb_Broadcast : (fer_memb) (m - 1) + fer_memb_M1);
      return true;
    }
  }
  return false;
}



bool ICACHE_FLASH_ATTR
timerString2bcd(const char *src, uint8_t *dst, uint16_t size_dst) {
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

const char help_None[] PROGMEM = "none";

const char help_parmHelp[] PROGMEM =
"type 'help command;'  or 'help all;'\ncommands are: ";

struct {
  const char *parm;
  int (*process_parmX)(clpar p[], int len);
  const char *help;
} parm_handlers[] = { { "send", process_parmSend, help_parmSend }, { "config", process_parmConfig, help_parmConfig }, { "mcu", process_parmMcu, help_parmMcu },
    { "timer", process_parmTimer, help_parmTimer }, { "help", process_parmHelp, help_parmHelp },
#if ENABLE_EXPERT
    { "expert", process_parmExpert, help_None},
#endif
#ifdef USE_PAIRINGS
    { "pair", process_parmPair, help_parmPair},
#endif
  };

int ICACHE_FLASH_ATTR
process_parm(clpar p[], int len) {
  int i;

  // if in sep mode, don't accept commands FIXME
  if (sep_is_enabled()) {
    sep_disable();
    reply_message(0, "error: CLI is disabled in set-endposition-mode\n");
    return -1;
  }

  for (i = 0; i < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++i) {
    if (strcmp(p[0].key, parm_handlers[i].parm) == 0)
      return parm_handlers[i].process_parmX(p, len);
  }
  return 0;
}

void ICACHE_FLASH_ATTR
process_cmdline(char *line) {
  dbg_vpf(db_printf("process_cmdline: %s\n", line));
  int n = parse_commandline(line);
  if (n < 0) {
    reply_failure();
  } else {
    process_parm(par, n);
  }
}

int ICACHE_FLASH_ATTR
process_parmHelp(clpar p[], int len) {
  int i;

  static const char usage[] PROGMEM = "syntax: command option=value ...;\n"
      "commands are: ";

  io_putlf();

  // print help for help;
  if (len == 1) {
    io_puts_P(help_parmHelp);
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
      io_puts_P(usage);
      for (i = 0; i < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++i) {
        io_puts(parm_handlers[i].parm), io_puts(", ");
      }
      io_putlf();

      for (i = 0; i < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++i) {
        io_puts(parm_handlers[i].parm), io_puts(" options:\n");
        io_puts_P(parm_handlers[i].help), io_putlf();
      }

    } else {

      for (k = 0; k < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++k) {
        if (strcmp(parm_handlers[k].parm, key) == 0) {
          io_puts_P(parm_handlers[k].help), io_putlf();
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

void ICACHE_FLASH_ATTR cli_loop(void) {
  char *cmdline;
  static bool ready;
  if ((cmdline = get_commandline())) {
    //io_putlf();
    process_cmdline(cmdline);
    cli_msg_ready();
  } else if (!ready) {
    cli_msg_ready();
    ready = true;
  }
}

#if TEST_MODULE_CLI
bool ICACHE_FLASH_ATTR
testModule_cli()
{
  char cl[] = "timer g=5 astro=0;";  //"timer g=2 m=2 weekly=08222000++++10552134+";
  int n = parse_commandline(cl);
  if (n > 0)
  process_parm(par, n);

  return n != -1;
}
#endif

