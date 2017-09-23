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

#include "all.h"

fer_sender_basic senders[10];
fer_sender_basic default_sender;
fer_sender_basic last_received_sender;

char * ICACHE_FLASH_ATTR
get_commandline() {
	char *result = NULL;
#define CMD_BUF_SIZE 64
	static char cmd_buf[CMD_BUF_SIZE];
	static int cmd_buf_idx;
	static bool error;
	int c;

	while ((c = io_getc()) != -1) {
		if (c == '\r' || c == '\n')
			continue;
		// to throw away rest of a too long command line from earlier
		if (error) {
			if (c == ';')
				error = false;
			continue;
		}
		// to make sure there is at least 1 free byte available in CMD_BUF
		if (!((cmd_buf_idx + 1) < CMD_BUF_SIZE)) {
			goto err;
		}

		// ';" is used to terminate a command line
		if (c == ';') {
			cmd_buf[cmd_buf_idx] = '\0';
			result = cmd_buf;
			goto succ;
		} else {
			cmd_buf[cmd_buf_idx++] = (char) c;
		}
	}

	goto cont;

	err: error = true;
	succ: cmd_buf_idx = 0;
	cont: return result;
}

typedef struct {
	char *opt;
	char *arg;
} clpar;

#define MAX_PAR 10
clpar par[MAX_PAR];

static char * ICACHE_FLASH_ATTR
skip_leading_whitespace(char *s) {
	while (*s == ' ')
		*s++ = '\0';

	return s;
}

static char * ICACHE_FLASH_ATTR
find_next_whitespace_or_eq(char *s) {
	while (*s != '\0' && *s != ' ' && *s != '=') {
		s++;
	}
	return s;
}

int ICACHE_FLASH_ATTR
parse_commandline(char *cl) {
	int p;

	for (p = 0; p < MAX_PAR; ++p) {
		cl = skip_leading_whitespace(cl);
		if (*cl == '\0') {
			return p;
		}
		par[p].opt = cl;
		cl = find_next_whitespace_or_eq(cl);
		switch (*cl) {
		case '\0':
			par[p].arg = 0;
			return p;

		case ' ':
			continue;

		case '=':
			*cl++ = '\0';
			cl = skip_leading_whitespace(cl);

			if (*cl == '\0') {
				return -1;
			}
			par[p].arg = cl;
			cl = find_next_whitespace_or_eq(cl);
			if (*cl == '=')
				return -1;

			break;
		}
	}

	return -1;

}

fer_cmd ICACHE_FLASH_ATTR
cli_parm_to_ferCMD(const char *token) {
	dbg_trace();

	return ((strcmp(token, "down") == 0) ? fer_cmd_DOWN : (strcmp(token, "up") == 0) ? fer_cmd_UP : (strcmp(token, "stop") == 0) ? fer_cmd_STOP :
			(strcmp(token, "sun-down") == 0) ? fer_cmd_SunDOWN : (strcmp(token, "sun-up") == 0) ? fer_cmd_SunUP :
			(strcmp(token, "sun-inst") == 0) ? fer_cmd_SunINST : (strcmp(token, "sun-test") == 0) ? fer_cmd_Program : fer_cmd_None);
}

void ICACHE_FLASH_ATTR
reply_success() {
	io_puts("reply: ok\n");
}

void ICACHE_FLASH_ATTR
reply_failure() {
	io_puts("reply: error\n");
}

bool ICACHE_FLASH_ATTR config_receiver(const char *val) {
	if (strcmp(val, "on") == 0) {
		C.app_recv = recvTick;
	} else if (strcmp(val, "off") == 0) {
		C.app_recv = recvNone;
	} else {
		return false;
	}
	save_config();
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
	save_config();
	return true;
}

void ICACHE_FLASH_ATTR
reply(bool success) {
	if (success)
		reply_success();
	else
		reply_failure();
}

fer_sender_basic * ICACHE_FLASH_ATTR
get_sender_by_addr(long addr) {
	if (addr == 0) {
		return &default_sender;
	} else if (addr > 0 && 10 > addr) {
		return &senders[addr];
	}
	return NULL ;
}

const char help_parmSend[] =
		  "a=(0|SenderID) address of the sender or 0 for the configured CentralUnit\n"
		  "g=[0-7]  group number. 0 is for broadcast\n"
		  "m=[0-7]  group member. 0 is for broadcast all groups members\n"
		  "c=(up|down|stop|sun-down|time) command to send\n";

static int ICACHE_FLASH_ATTR
process_parmSend(clpar p[], int len) {
	int i;

	long addr = 0;
	int group = 0, memb = 0;
	fer_cmd cmd = fer_cmd_None;
	const char *cmdStr = 0;
	const char *rtc = 0;

	for (i = 1; i < len; ++i) {
		const char *key = p[i].opt, *val = p[i].arg;

		if (key == NULL || val == NULL) {
			return -1;
		} else if (strcmp(key, "a") == 0) {
			addr = strtol(val, NULL, 0);
		} else if (strcmp(key, "g") == 0) {
			group = atoi(val);
		} else if (strcmp(key, "m") == 0) {
			memb = atoi(val);
		} else if (strcmp(key, "c") == 0) {
			cmd = cli_parm_to_ferCMD(val);
			cmdStr = val;
		} else if (strcmp(key, "t") == 0) {
			rtc = val;
		} else {
			reply(false); // unknown parameter
			return -1;
		}
	}

	fer_sender_basic *fsb = get_sender_by_addr(addr);
	if (!fsb) {
		reply(false);
		return -1;
	}

	FSB_PUT_GRP(fsb, group);

	FSB_PUT_MEMB(fsb, (memb == 0) ? fer_memb_Broadcast : fer_memb_M1 + memb - 1);

	if (cmd != fer_cmd_None) {
		FSB_PUT_CMD(fsb, cmd);
		fer_update_tglNibble(fsb);
		reply(fer_send_cmd(fsb));

	} else if (cmdStr != NULL && strcmp(cmdStr, "time") == 0) {
		FSB_PUT_CMD(fsb, fer_cmd_Program);
		fer_update_tglNibble(fsb);
		if (!rtc || rtc_set_by_string(rtc)) {
			write_rtc(get_sendPrgBufLine(FPR_RTC_START_ROW), true);
			reply(fer_send_prg(fsb));
			io_puts("S:"), print_array_8(get_sendPrgBufLine(FPR_RTC_START_ROW),
			FPR_ASTRO_WIDTH), io_puts("\n");
		} else {
			reply(false);
		}
	} else {
		reply(false);
	}

	return 0;
}

const char help_parmConfig[] =
		  "cu=(CentralUnitID|auto)\n"
		  "rtc=ISO_TIME_STRING  like 2017-12-31T23:59:59\n"
		  "baud=serial_baud_rate\n"
		  "wlan_ssid=\"your_wlan_ssid\"\n"
		  "wlan_password=\"your_wlan_password\"\n";

static int ICACHE_FLASH_ATTR
process_parmConfig(clpar p[], int len) {
	int i;

	for (i = 1; i < len; ++i) {
		const char *key = p[i].opt, *val = p[i].arg;

		if (key == NULL || val == NULL) {
			reply(false);
			return -1;
		} else if (strcmp(key, "rtc") == 0) {
			reply(rtc_set_by_string(val));

		} else if (strcmp(key, "cu") == 0) {
			uint32_t cu = (strcmp(val, "auto") == 0) ? FSB_GET_DEVID(&last_received_sender) : strtoul(val, NULL, 16);

			if (!(GET_BYTE_2(cu) == FER_ADDR_TYPE_CentralUnit && GET_BYTE_3(cu) == 0)) {
				reply_failure();
				return -1;
			}
			FSB_PUT_DEVID(&default_sender, cu);
			C.fer_centralUnitID = cu;
			save_config();
			reply_success();
		} else if (strcmp(key, "baud") == 0) {
			uint32_t baud = strtoul(val, NULL, 10);
			C.mcu_serialBaud = baud;
			save_config();
			reply_success();
            		} else if (strcmp(key, "wlan_ssid") == 0) {
            		uint32_t baud = strtoul(val, NULL, 10);
            		C.mcu_serialBaud = baud;
            		save_config();
            		reply_success();
                    		} else if (strcmp(key, "wlan_password") == 0) {
                    		uint32_t baud = strtoul(val, NULL, 10);
                    		C.mcu_serialBaud = baud;
                    		save_config();
                    		reply_success();
		} else if (strcmp(key, "receiver") == 0) {
			reply(config_receiver(val));
		} else if (strcmp(key, "transmitter") == 0) {
			reply(config_transmitter(val));
		} else {
			reply(false); // unknown parameter
			return -1;
		}
	}
	return 0;
}

extern uint32_t data_clock_ticks, pre_len, pre_phase;

const char help_parmDbg[] =
		"print=(rtc|cu)\n";


static int ICACHE_FLASH_ATTR
process_parmDbg(clpar p[], int len) {
	int i;

	for (i = 1; i < len; ++i) {
		const char *key = p[i].opt, *val = p[i].arg;

		if (key == NULL || val == NULL) {
			return -1;
		} else if (strcmp(key, "print") == 0) {

			if (strcmp(val, "rtc") == 0) {
				char buf[20];
				if (rtc_get_by_string(buf)) {
					io_puts(buf);
					io_puts("\n");
				}
#ifndef AVR
				setup_notImplemented();
#endif
			} else if (strcmp(val, "cu") == 0) {
				io_puts("central unit address: ");
				io_putl(C.fer_centralUnitID, 16);
				io_puts("\n");
			} else if (strcmp(val, "clk") == 0) {
#ifdef FER_RECEIVER
				io_puts("data_clock_ticks: ");
				io_putl(data_clock_ticks, 10);
				io_puts(", pre_len: ");
				io_putl(pre_len, 10);
				io_puts(", pre_phase: ");
				io_putl(pre_phase, 10);

				io_puts("\n");
#endif
			}

		} else if (strcmp(key, "test") == 0) {
			if (strcmp(val, "sendprg") == 0) {
				extern bool dbg_test_sendprg();
				dbg_test_sendprg();

			}

		} else {
			reply(false); // unknown parameter
			return -1;
		}

	}

	return 0;
}

static int process_parmHelp(clpar p[], int len);

struct {
  const char *parm;
  int (*process_parmX)(clpar p[], int len);
  const char *help;
  } parm_handlers[] = {
    {"send", process_parmSend, help_parmSend},
    {"config", process_parmConfig, help_parmConfig},
    {"dbg", process_parmDbg, help_parmDbg},
    {"help", process_parmHelp, "none"},
    };  
      

int ICACHE_FLASH_ATTR
process_parm(clpar p[], int len) {
	int i;
    for (i=0; i < (sizeof (parm_handlers) / sizeof (parm_handlers[0])); ++i) {
    	if (strcmp(p[0].opt, parm_handlers[i].parm) == 0)
           parm_handlers[i].process_parmX(p, len);
    }
 return 0;
}

void ICACHE_FLASH_ATTR
process_cmdline(char *line) {
	int n = parse_commandline(line);
	process_parm(par, n);

}



static int ICACHE_FLASH_ATTR
process_parmHelp(clpar p[], int len) {
	int i;

  const char *usage = "syntax: command option=value ...;\n"
  "commands are: ";
  io_puts(usage);

  for (i=0; i < (sizeof (parm_handlers) / sizeof (parm_handlers[0])); ++i) {
	  io_puts(parm_handlers[i].parm), io_puts(", ");
  }
  io_puts("\n");

  for (i=0; i < (sizeof (parm_handlers) / sizeof (parm_handlers[0])); ++i) {
	  io_puts(parm_handlers[i].parm), io_puts(" options:\n");
	  io_puts(parm_handlers[i].help), io_puts("\n");
  }

   return 0;
}

#if TEST_MODULE_CLI
bool
testModule_cli()
{
	char cl[] = "send a=2 g=3 m=2 c=up";
	int n = parse_commandline(cl);
	if (n > 0)
	process_parm(par, n);

	return n != -1;
}
#endif

#if 1
#include "fer_code.h"

bool ICACHE_FLASH_ATTR
dbg_test_sendprg() {
	static int first;
	if (first == 0) {
		rtc_set_by_string("2017-09-08T00:46:44");
		++first;
	}

	static fer_sender_basic fsb_, *fsb = &fsb_;

	FSB_PUT_DEVID(fsb, C.fer_centralUnitID);
	FSB_PUT_GRP(fsb, fer_grp_G2);
	FSB_PUT_MEMB(fsb, fer_memb_M2);
	FSB_PUT_CMD(fsb, fer_cmd_Program);
	fer_update_tglNibble(fsb);

	init_prgPacket(dtSendPrgFrame);
	write_rtc(get_sendPrgBufLine(FPR_RTC_START_ROW), false);
	write_lastline(fsb, get_sendPrgBufLine(FPR_LAST_START_ROW));
	fer_send_prg(fsb);
	io_puts("ok\n");
	return true;
}
#endif
