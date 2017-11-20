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

#include "all.h"

#ifdef MCU_ESP8266
#include "../esp8266/data_flash2.h"
#define ENABLE_RSTD 1
#else
#define ENABLE_RSTD 0
#endif

#define ALLOW_ENDPOS 0   // dangerous, could damage your shutter
#define ENABLE_EXPERT 0 // work in progress
#define ENABLE_RESTART 1 // allow software reset
#define ENABLE_TIMER_WDAY_KEYS 0  // allow timer mon=T tue=T sun=T  additional to weekly=TTTTTTT  (a waste of resources)


#define NODEFAULT() if (val==0) return reply_failure()
//fer_sender_basic senders[10];
fer_sender_basic default_sender;
fer_sender_basic last_received_sender;
uint16_t msgid;


int ENR; // error number
static void ICACHE_FLASH_ATTR print_enr(void) {
	io_puts("enr: "), io_putd(ENR), io_putlf();
}

static void ICACHE_FLASH_ATTR msg_print(const char *msg, const char *tag) {
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

static void ICACHE_FLASH_ATTR reply_print(const char *tag) {
  msg_print("reply", tag);
}

static void ICACHE_FLASH_ATTR reply_message(const char *tag, const char *msg) {
	reply_print(tag);
	if (msg) io_puts(msg);
	io_putlf();
}

void ICACHE_FLASH_ATTR cli_msg_ready(void) {
	io_puts("\nready:\n");
}

float ICACHE_FLASH_ATTR stof(const char* s) {
	int point_seen;

	float rez = 0, fact = 1;
	if (*s == '-') {
		s++;
		fact = -1;
	};
	for (point_seen = 0; *s; s++) {
		if (*s == '.') {
			point_seen = 1;
			continue;
		};
		int d = *s - '0';
		if (d >= 0 && d <= 9) {
			if (point_seen)
				fact /= 10.0f;
			rez = rez * 10.0f + (float) d;
		};
	};
	return rez * fact;
}
//#define stof atof


char * ICACHE_FLASH_ATTR
get_commandline() {
	char *result = NULL;
#define CMD_BUF_SIZE 128
	static char cmd_buf[CMD_BUF_SIZE];
	static int cmd_buf_idx;
	static bool error;
	int c;
	static int quoteCount;

	while ((c = io_getc()) != -1) {

		// handle characters who don't get into the buffer
		if (c == '\r' || c == '\n') {
			quoteCount = 0;
			continue;
		}

		// backspace: remove last character from buffer
		if (c == '\b') {
			if (cmd_buf_idx == 0)
				continue;
			if (cmd_buf[--cmd_buf_idx] == '\"')
				--quoteCount;
			continue;
		}

		// count the quotes, so we now if we are inside or outside a quoted word
		if (c=='\"') {
			++quoteCount;
		}

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

		// handle special characters, if outside a quoted word
		if ((quoteCount & 1) == 0) {
			// ';' is used to terminate a command line
			if (c == ';' && (quoteCount & 1) == 0) {
				if (cmd_buf_idx == 0)
				goto succ;
				cmd_buf[cmd_buf_idx] = '\0';
				result = cmd_buf;
				goto succ;
			}
		}

		// store char to buffer
		cmd_buf[cmd_buf_idx++] = (char) c;
	}

	goto cont;

	err: error = true;
	succ: cmd_buf_idx = 0;
	quoteCount = 0;
	cont: return result;
}

typedef struct {
	char *key;
	char *val;
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
find_next_space_eq_eol(char *s) {
	while (*s != '\0' && *s != ' ' && *s != '=') {
		s++;
	}
	return s;
}
static char * ICACHE_FLASH_ATTR
find_next_whitespace_or_eol(char *s) {
	while (*s != '\0' && *s != ' ' && *s != '=') {
		s++;
	}
	return s;
}

int ICACHE_FLASH_ATTR
asc2bool(const char *s)
{
   if (!s)
	 return 1; // default value for key without value

   switch(*s) {
     case '0':
      return 0;
     case '1':
     return 1;
     default:
     return -1;
   }
  
}  

int ICACHE_FLASH_ATTR
parse_commandline(char *cl) {
	int p;
	msgid = 0;

	for (p = 0; p < MAX_PAR; ++p) {
		bool isValQuoted=false;

		// trim
		cl = skip_leading_whitespace(cl);

		// eol
		if (*cl == '\0') {
			return p;
		}

		// first word is the key
		par[p].key = cl;

		// skip the key
		cl = find_next_space_eq_eol(cl);


		switch (*cl) {

		// no val follows the key. set val to NULL (interpreted as default)
		case '\0':
		case ' ':
			par[p].val = NULL;
			continue;

	    // got val for the key
		case '=':
			// add null terminator to key
			*cl++ = '\0';

			if (*cl == '\"') {
				isValQuoted = true;
				++cl;
			}

			// eol - error
			if (*cl == '\0') {
				return -1;
			}

			// start of val
			par[p].val = cl;

			// end of val
			if (isValQuoted) {
				char *q = strchr(cl, '\"');
				// quote unbalanced
				if (q == 0) {
					return -1;
				}
				cl = q;

			} else {
				cl = find_next_whitespace_or_eol(cl);
			}

			// terminate val if not already terminated by eol
			if (*cl != '\0') {
			   *cl++ = '\0';
			}

			// found global option
			// process it here and remove the key/val
			if (strcmp(par[p].key, "mid") == 0) {
				msgid = atoi(par[p].val);
				--p;
			}

			break;
		}
	}

	return -1;

}

struct c_map {
	const char *fs;
	fer_cmd fc;
};

struct c_map const fc_map[]  = {
		{"down", fer_cmd_DOWN},
		{"up", fer_cmd_UP},
		{"stop", fer_cmd_STOP},
		{"sun-down", fer_cmd_SunDOWN},
		{"sun-up", fer_cmd_SunUP},
		{"sun-inst", fer_cmd_SunINST},
//		{"sun-test", fer_cmd_Program},
        {"set", fer_cmd_SET},
#if ALLOW_ENDPOS
		{"end-pos-down", fer_cmd_EndPosDOWN},  // dangerous, could damage shutter
		{"end-pos-up", fer_cmd_EndPosUP},      // dangerous, could damage shutter
#endif
};

fer_cmd ICACHE_FLASH_ATTR
cli_parm_to_ferCMD(const char *token) {
  int i;
	dbg_trace();

    for (i=0; i < (sizeof (fc_map) / sizeof (fc_map[0])); ++i) {
      if (strcmp(token, fc_map[i].fs) == 0)
    	  return fc_map[i].fc;
    }
    return fer_cmd_None;
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


bool ICACHE_FLASH_ATTR cu_auto_set(unsigned init_seconds) {
	static time_t end_time;
	static uint16_t cuas_msgid;

	if (init_seconds > 0) {
		end_time = time(NULL) + init_seconds;
		last_received_sender.data[0] = 0;
		cuas_msgid = msgid;
	} else if (end_time == 0) {

	} else if (end_time < time (NULL)){
		uint16_t global_msgid = msgid;
		end_time = 0;
		io_puts("U: Nothing received\n");
		msgid = cuas_msgid;
		reply_message("cuas=time-out", 0);
		msgid = global_msgid;
	} else if (FSB_MODEL_IS_CENTRAL(&last_received_sender)) {
		uint32_t cu = FSB_GET_DEVID(&last_received_sender);

		FSB_PUT_DEVID(&default_sender, cu);
		C.fer_centralUnitID = cu;
		end_time = 0;
		io_puts("U: Central Unit received and stored\n");
		reply_message("cuas=ok", 0);
		save_config();
		return true;
	}

	return false;
}

fer_grp asc2group(const char *s) { return (fer_grp) atoi(s); }
fer_memb asc2memb(const char *s) { int m = atoi(s); return m == 0 ? fer_memb_Broadcast : (fer_memb) (m - 1) + fer_memb_M1; }
  
const char help_parmSend[] PROGMEM =
		  "a=(0|ID)    hex ID of sender or receiver. 0 (default) uses 'cu' in config\n"
		  "g=[0-7]     group number. 0 (default) addresses all groups\n"
		  "m=[0-7]     group member. 0 (default) addresses all members\n"
		  "c=command   Command to send. One of: up, down, stop, sun-down, sun-inst, set\n"
#if ALLOW_ENDPOS
         "potentially harmful: c=(limit-up|limit-down):  command to set end-position ... it will not stop and end position until you send STOP.\n"
#endif
		;

static int ICACHE_FLASH_ATTR
process_parmSend(clpar p[], int len) {
	int i;

	uint32_t addr = 0;
	fer_grp group = fer_grp_Broadcast;
    fer_memb memb = fer_memb_Broadcast;
	fer_cmd cmd = fer_cmd_None;

	for (i = 1; i < len; ++i) {
		const char *key = p[i].key, *val = p[i].val;

		if (key == NULL) {
			return -1;
		} else if (strcmp(key, "a") == 0) {
			addr = val ? strtol(val, NULL, 16) : 0;
		} else if (strcmp(key, "g") == 0) {
			group = val ? asc2group(val) : 0;
		} else if (strcmp(key, "m") == 0) {
			memb = val ? asc2memb(val) : 0;
		} else if (strcmp(key, "c") == 0) {
			NODEFAULT();
			cmd = cli_parm_to_ferCMD(val);
		} else {
			warning_unknown_option(key);
		}
	}

	fer_sender_basic *fsb = get_sender_by_addr(addr);
	if (!fsb) {
      static fer_sender_basic fsb_direct; // FIXME: or was senders[0] meant for this?
      fsb = &fsb_direct;
      if (FSB_GET_DEVID(fsb) != addr) {
        fer_init_sender(fsb, addr);
      }        
	}

	if (FSB_MODEL_IS_CENTRAL(fsb)) {
      FSB_PUT_GRP(fsb, group);
 	  FSB_PUT_MEMB(fsb, memb);  // only set this on central unit!
	}

	if (cmd != fer_cmd_None) {
		FSB_PUT_CMD(fsb, cmd);
		fer_update_tglNibble(fsb);
		reply(fer_send_cmd(fsb));
	} else {
		reply_failure();
	}

	return 0;
}

const char help_parmConfig[] PROGMEM =
		  "cu=(ID|auto)     6-digit hex ID of Central-Unit. auto: capture ID with connected RF receiver\n"
		  "rtc=ISO_TIME     (rtc=2017-12-31T23:59:59)\n"
		  "baud=N           serial baud rate (baud=38400)\n"
#ifdef USE_WLAN
		  "wlan-ssid=SSID\n"
		  "wlan-password=PW  example: config wlan-ssid=\"1234\" wlan-password=\"abcd\" restart;\n"
#endif
		  "longitude=DEC\n"
		  "latitude=DEC\n"
		  "time-zone=N       example: config  longitude=-13.23452 latitude=+52.34234 time-zone=+1.0;\n"
		  "dst=(eu|0|1)      daylight saving time: automatic: eu=europe. manually: 0=off, 1=on\n"
		  "verbose=(0..5)    set text output verbosity level: 0 for none ... 5 for max)\n"
		  "set-config-password=PW  set a config password. if set every config commands needs the pw option\n"
		  "pw=PW             example: config pw=my_passw dst=eu;\n"
#if ENABLE_RESTART
		  "restart           restart MCU\n"
#endif
		//  "set-expert-password=\n"
		;

static int ICACHE_FLASH_ATTR
process_parmConfig(clpar p[], int len) {
	int i;
	bool pw_ok = strlen (C.app_configPassword) == 0;

	for (i = 1; i < len; ++i) {
		const char *key = p[i].key, *val = p[i].val;

		if (key == NULL || val == NULL) {  // don't allow any default values in config
			return reply_failure();
		} else if (strcmp(key, "pw") == 0) {
			if (val && strcmp(C.app_configPassword, val) == 0) {
				pw_ok = true;
				io_puts("password ok\n");

			} else {
				io_puts("wrong config password\n");
				return reply_failure();
			}
		} else if (!pw_ok) {
			io_puts("missing config password\n");
			return reply_failure();
#if ENABLE_RESTART
		} else if (strcmp(key, "restart") == 0) {
			mcu_restart();
#endif
		} else if (strcmp(key, "rtc") == 0) {
			reply(val ? rtc_set_by_string(val) : false);

		} else if (strcmp(key, "cu") == 0) {
			if (strcmp(val, "auto") == 0) {
				cu_auto_set(60);
				io_puts("U: Press Stop on the Fernotron central unit (60 seconds remaining)\n");
				reply_success();
			} else {
			uint32_t cu = (strcmp(val, "auto-old") == 0) ? FSB_GET_DEVID(&last_received_sender) : strtoul(val, NULL, 16);

			if (!(GET_BYTE_2(cu) == FER_ADDR_TYPE_CentralUnit && GET_BYTE_3(cu) == 0)) {
				return reply_failure();
			}
			FSB_PUT_DEVID(&default_sender, cu);
			C.fer_centralUnitID = cu;
			save_config();
			reply_success();
			}
		} else if (strcmp(key, "gm-used") == 0) {
			uint32_t gmu = strtoul(val, NULL, 16);
			C.fer_usedMembers = gmu;
			save_config();
			reply_success();

		} else if (strcmp(key, "baud") == 0) {
			uint32_t baud = strtoul(val, NULL, 10);
			C.mcu_serialBaud = baud;
			save_config();
			reply_success();
		} else if (strcmp(key, "verbose") == 0) {
			NODEFAULT();
			enum verbosity level = atoi(val);
			C.app_verboseOutput = level;
			save_config();
			reply_success();
#ifdef USE_WLAN
		} else if (strcmp(key, "wlan-ssid") == 0) {
			if (strlen(val) < sizeof (C.wifi_SSID)) {
			strcpy (C.wifi_SSID, val);
			save_config();
			reply_success();
			} else {
				reply_failure();
			}
		} else if (strcmp(key, "wlan-password") == 0) {
			if (strlen(val) < sizeof (C.wifi_password)) {
			strcpy (C.wifi_password, val);
			save_config();
			reply_success();
			} else {
				reply_failure();
			}
#endif // USE_WLAN
		} else if (strcmp(key, "set-config-password") == 0) {
			if (strlen(val) < sizeof (C.app_configPassword)) {
			strcpy (C.app_configPassword, val);
			save_config();
			reply_success();
			} else {
				reply_failure();
			}
		} else if (strcmp(key, "receiver") == 0) {
			reply(config_receiver(val));
		} else if (strcmp(key, "transmitter") == 0) {
			reply(config_transmitter(val));
		} else if (strcmp(key, "longitude") == 0) {
			float longitude = stof(val);
			C.geo_longitude = longitude;
			rtc_setup();
			save_config();
			reply_success();
		} else if (strcmp(key, "latitude") == 0) {
			float latitude = stof(val);
			C.geo_latitude = latitude;
			rtc_setup();
			save_config();
			reply_success();
		} else if (strcmp(key, "time-zone") == 0) {
			float timezone = stof(val);
			C.geo_timezone = timezone;
			rtc_setup();
			save_config();
			reply_success();
		} else if (strcmp(key, "dst") == 0) {
			if (strcmp(val, "eu")) {
				C.geo_dST = dstEU;
			}
			if (strcmp(val, "0")) {
				C.geo_dST = dstNone;
			}
			if (strcmp(val, "1")) {
				C.geo_dST = dstAlways;
			} else {
				warning_unknown_option(key);
			}

			rtc_setup();
			save_config();
			reply_success();

		} else {
			warning_unknown_option(key);
		}
	}
	return 0;
}

const char help_parmDbg[] PROGMEM =
		"print=(rtc|cu)\n";


static int ICACHE_FLASH_ATTR
process_parmDbg(clpar p[], int len) {
	int i;

	for (i = 1; i < len; ++i) {
		const char *key = p[i].key, *val = p[i].val;

		if (key == NULL || val == NULL) {
			return -1;
		} else if (strcmp(key, "print") == 0) {

			if (strcmp(val, "rtc") == 0) {
				char buf[20];
				if (rtc_get_by_string(buf)) {
					io_puts(buf);
					io_putlf();
				}
			} else if (strcmp(val, "cu") == 0) {
				io_puts("central unit address: ");
				io_putl(C.fer_centralUnitID, 16);
				io_putlf();
			}
#if ENABLE_SPIFFS
		} else if (strcmp(key, "spiffs") == 0) {
			if (strcmp(val, "format") == 0) {
				spiffs_format();
			} else if (strcmp(val, "test") == 0) {
				spiffs_test();
			}
#endif

		} else {
			warning_unknown_option(key);
		}

	}

	return 0;
}

enum { TIMER_KEY_WEEKLY, TIMER_KEY_DAILY, TIMER_KEY_ASTRO, TIMER_KEY_RTC_ONLY, TIMER_KEY_FLAG_RANDOM, TIMER_KEY_FLAG_SUN_AUTO};

const char *const timer_keys[]  = {
  "weekly", "daily", "astro", "rtc-only", "random", "sun-auto"
};

#if ENABLE_TIMER_WDAY_KEYS
#ifdef WEEK_STARTS_AT_SUNDAY
const char *const timer_wdays[]  = { "sun", "mon", "tue", "wed", "thu", "fri", "sat", "sun" };
#else
const char *const timer_wdays[]  = { "mon", "tue", "wed", "thu", "fri", "sat", "sun" };
#endif
  
int asc2wday(const char *s) {
  int i;
      for (i=0; i < (sizeof (timer_wdays) / sizeof (timer_wdays[0])); ++i) {
        if (strcmp(s, timer_wdays[i]) == 0)
        return i;
      }
    return -1;      
}
#endif

bool ICACHE_FLASH_ATTR  
string2bcdArray(const char *src, uint8_t *dst, uint16_t size_dst) {
  char buf[3];
  int i;
  
  buf[2] = 0;
  
  for (i=0; i < size_dst && src[0]; ++i) { 
    switch (*src) {
      case '-': // timer off
        dst[i] = (i % 2 == 0) ? 0xff : 0x0f;
        if (!(++i < size_dst)) return false;;
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
         if (!(++i < size_dst)) return false;;
         dst[i] = dec2bcd(atoi(buf));
         buf[0] = *src++;
         buf[1] = *src++;
         dst[i-1] = dec2bcd(atoi(buf));

    }

  }
  
  return i == size_dst;
}

#define FLAG_NONE -2
#define FLAG_ERROR -1
#define FLAG_FALSE 0
#define FLAG_TRUE 1
#define HAS_FLAG(v) (v >= 0)

const char help_parmTimer[] PROGMEM =
		  "daily=T        enables daily timer. T is up/down like 0730- or 07302000 or -2000  for up 07:30 and/or down 20:00\n"
		  "weekly=TTTTTTT enables weekly timer. T like with 'daily' or '+' to copy the T on the left. (weekly=0730-++++0900-+)\n"
		  "astro[=N]      enables astro automatic. N is the offset to civil dusk in minutes. Can be postive or negative.\n"
		  "sun-auto       enables sun automatic\n"
		  "random         enables random automatic. shutter opens and closes at random times.\n"
		  "rtc-only       don't change timers. only update internal real time clock\n"
		  "a, g and m:    like in send command\n"
		  "rs=(0|1|2)     read back saved timer data. if set to 2, return any data matching g and m e.g. m=0 (any member) instead of m=2\n"
		;

static int ICACHE_FLASH_ATTR
process_parmTimer(clpar p[], int len) {
	int i;
	bool has_daily = false, has_weekly = false, has_astro = false;
	int16_t astro_offset = 0;
	uint8_t weekly_data[FPR_TIMER_STAMP_WIDTH * 7];
	uint8_t daily_data[FPR_TIMER_STAMP_WIDTH];
	fer_sender_basic *fsb = &default_sender;
	fer_grp group = fer_grp_Broadcast;
	fer_memb memb = fer_memb_Broadcast;
	uint32_t addr = 0;
	int wday = -1;
	uint8_t fpr0_flags = 0, fpr0_mask = 0;
	int8_t flag_rtc_only = FLAG_NONE;

#if ENABLE_RSTD
	const char *weeklyVal, *dailyVal;
	uint8_t mn = 0;
	uint8_t rs = 0;
	timer_data_t td = { 20000, 0, "", "" };
#endif
	// init data
	for (i = 0; i + 1 < sizeof(weekly_data) / sizeof(weekly_data[0]); i += 2) {
		weekly_data[i] = 0xff;
		weekly_data[i + 1] = 0x0f;
	}
	for (i = 0; i + 1 < sizeof(daily_data) / sizeof(daily_data[0]); i += 2) {
		daily_data[i] = 0xff;
		daily_data[i + 1] = 0x0f;
	}

	for (i = 1; i < len; ++i) {
		const char *key = p[i].key, *val = p[i].val;

		if (key == NULL) {
			return -1;
		} else if (strcmp(key, timer_keys[TIMER_KEY_WEEKLY]) == 0) {
			NODEFAULT();
			has_weekly = string2bcdArray(val, weekly_data, sizeof weekly_data);
#if ENABLE_RSTD
			weeklyVal = val;
#endif
		} else if (strcmp(key, timer_keys[TIMER_KEY_DAILY]) == 0) {
			NODEFAULT();
			has_daily = string2bcdArray(val, daily_data, sizeof daily_data);
#if ENABLE_RSTD
			dailyVal = val;
#endif
		} else if (strcmp(key, timer_keys[TIMER_KEY_ASTRO]) == 0) {
			astro_offset = val ? atoi(val) : 0;
			has_astro = true;
		} else if (strcmp(key, timer_keys[TIMER_KEY_FLAG_RANDOM]) == 0) {
			int flag = asc2bool(val);
			if (flag >= 0) {
				fpr0_mask |= (1 << flag_Random);
				if (flag)
					fpr0_flags |= (1 << flag_Random);
				else
					fpr0_flags &= ~(1 << flag_Random);
			}
		} else if (strcmp(key, timer_keys[TIMER_KEY_FLAG_SUN_AUTO]) == 0) {
			int flag = asc2bool(val);
			if (flag >= 0) {
				fpr0_mask |= (1 << flag_SunAuto);
				if (flag)
					fpr0_flags |= (1 << flag_SunAuto);
				else
					fpr0_flags &= ~(1 << flag_SunAuto);
			}

		} else if (strcmp(key, timer_keys[TIMER_KEY_RTC_ONLY]) == 0) {
			flag_rtc_only = asc2bool(val);
		} else if (strcmp(key, "a") == 0) {
			addr = val ? strtol(val, NULL, 16) : 0;
		} else if (strcmp(key, "g") == 0) {
			group = val ? asc2group(val) : 0;
		} else if (strcmp(key, "m") == 0) {
			memb = val ? asc2memb(val) : 0;
#if ENABLE_RSTD
			mn = memb ? (memb - 7) : 0;
		} else if (strcmp(key, "rs") == 0) {
			NODEFAULT();
			rs = atoi(val);
#endif
#if ENABLE_TIMER_WDAY_KEYS
		} else if ((wday = asc2wday(key)) >= 0) {
			NODEFAULTS();
			io_puts(val), io_putlf();
			has_weekly = string2bcdArray(val, &weekly_data[FPR_TIMER_STAMP_WIDTH * wday], FPR_TIMER_STAMP_WIDTH);
#endif
		} else {
			warning_unknown_option(key);
		}
	}

#if ENABLE_RSTD
	if (rs) {
		uint8_t g = group, m = mn;

		reply_print("rs=data");
		if (read_timer_data(&td, &g, &m, rs==2)) {
			io_puts("timer");
			io_puts(" g="), io_putd(g);
			io_puts(" m="), io_putd(m);
			if (td_is_daily(&td)) {
				io_puts(" daily="), io_puts(td.daily);
			}
			if (td_is_weekly(&td)) {
				io_puts(" weekly="), io_puts(td.weekly);
			}
			if (td_is_astro(&td)) {
				io_puts(" astro="), io_putd(td.astro);
			}
			if (td_is_random(&td)) {
				io_puts(" random=1");
			}
			if (td_is_sun_auto(&td)) {
				io_puts(" sun-auto=1");
			}

			io_puts(";\n");
			reply_success();
		} else {
			io_puts("none\n");
		}
		return 0;
	}
#endif

	if (addr != 0) {
		fsb = get_sender_by_addr(addr);
		if (!fsb) {
			static fer_sender_basic fsb_direct; // FIXME: or was senders[0] meant for this?
			fsb = &fsb_direct;
			if (FSB_GET_DEVID(fsb) != addr) {
				fer_init_sender(fsb, addr);
			}
		}
	}

	if (FSB_MODEL_IS_CENTRAL(fsb)) {
		FSB_PUT_GRP(fsb, group);
		FSB_PUT_MEMB(fsb, memb);
	}

	FSB_PUT_CMD(fsb, fer_cmd_Program);
	FSB_TOGGLE(fsb);

	if (recv_lockBuffer(true)) {
		init_prgPacket(getMsgData(tbuf));
		if (flag_rtc_only == FLAG_TRUE) {
			txbuf_write_rtc(true);
			txbuf_write_flags(fpr0_flags, fpr0_mask); // the flags are ignored for RTC-only frames, even for non-broadcast
		} else {
			if (has_weekly)
				txbuf_write_wtimer(weekly_data);
			if (has_daily)
				txbuf_write_dtimer(daily_data);
			if (has_astro)
				txbuf_write_astro(astro_offset);
			txbuf_write_rtc(false);
			txbuf_write_flags(fpr0_flags, fpr0_mask);
			txbuf_write_lastline(fsb);
		}

		if (reply(fer_send_prg(fsb))) {
#if ENABLE_RSTD
			if (FSB_MODEL_IS_CENTRAL(fsb) && flag_rtc_only != FLAG_TRUE) {  // FIXME: or better test for default cu?
				if (has_astro) {
					td.astro = astro_offset;
				}
				td.bf = fpr0_flags;
				if (has_weekly) {
					strncpy(td.weekly, weeklyVal, sizeof(td.weekly) - 1);
				}

				if (has_daily) {
					strncpy(td.daily, dailyVal, sizeof(td.daily) - 1);
				}

				if (save_timer_data(&td, group, mn)) {
					reply_message("rs", "saved");
				} else {
					reply_message("bug", "rs not saved");
					print_enr();
				}
			}
#endif
		}

		recv_lockBuffer(false);
	}
	return 0;
}



#if ENABLE_EXPERT

struct c_map const ec_map[]  = {
		{"down", fer_cmd_DOWN},
		{"up", fer_cmd_UP},
		{"stop", fer_cmd_STOP},
		{"sun-down", fer_cmd_SunDOWN},
		{"sun-up", fer_cmd_SunUP},
		{"sun-inst", fer_cmd_SunINST},
//		{"sun-test", fer_cmd_Program},
        {"set", fer_cmd_SET},
		{"end-pos-down", fer_cmd_EndPosDOWN},  // dangerous, could damage shutter
		{"end-pos-up", fer_cmd_EndPosUP},      // dangerous, could damage shutter
};


#define create_expert_key() ("a(42)")  // FIXME: create random key
#define test_expert_key(k) (k == 84)
static int ICACHE_FLASH_ATTR
process_parmExpert(clpar p[], int len) {
	int i;
// FIXME: function body copied from parmSend ... now modifiy it

	bool reply_exp_key = false, match_exp_key = false;
	uint32_t exp_key = 0;
	uint32_t addr = 0;
	fer_grp group = fer_grp_Broadcast;
    fer_memb memb = fer_memb_Broadcast;
	fer_cmd cmd = fer_cmd_None;

	for (i = 1; i < len; ++i) {
		const char *key = p[i].key, *val = p[i].val;

		if (key == NULL || val == NULL) {
			return -1;
		} else if (strcmp(key, "req-exp-key") == 0) {
             reply_exp_key = asc2bool(val);
		} else if (strcmp(key, "exp-key") == 0) {
			 match_exp_key = test_expert_key(strtol(val, NULL, 0));
		} else if (strcmp(key, "a") == 0) {
			addr = strtol(val, NULL, 16);
		} else if (strcmp(key, "g") == 0) {
			group = asc2group(val);
		} else if (strcmp(key, "m") == 0) {
			memb = asc2memb(val);
		} else if (strcmp(key, "c") == 0) {
			cmd = cli_parm_to_ferCMD(val);
		} else {
			reply_failure(); // unknown parameter
			return -1;
		}
	}

    if (reply_exp_key) {
   	    io_puts("reply: key="), io_puts(create_expert_key()), io_putlf();
   	    reply_success();
        return 0;
    } else if (!match_exp_key) {
    	reply_failure();
    	return -1;
    } else {
    	io_puts("dbg: key matched\n");
    }

	fer_sender_basic *fsb = get_sender_by_addr(addr);
	if (!fsb) {
      static fer_sender_basic fsb_direct; // FIXME: or was senders[0] meant for this?
      fsb = &fsb_direct;
      if (FSB_GET_DEVID(fsb) != addr) {
        fer_init_sender(fsb, addr);
      }
	}

	if (FSB_MODEL_IS_CENTRAL(fsb)) {
      FSB_PUT_GRP(fsb, group);
 	  FSB_PUT_MEMB(fsb, memb);  // only set this on central unit!
	}

	if (cmd != fer_cmd_None) {
		FSB_PUT_CMD(fsb, cmd);
		fer_update_tglNibble(fsb);
		reply(fer_send_cmd(fsb));
	} else {
		reply_failure();
	}

	return 0;
}
#endif

static int process_parmHelp(clpar p[], int len);

const char help_None[] PROGMEM = "none";


const char help_parmHelp[] PROGMEM =
		"help command;  or help all;\n";



struct {
  const char *parm;
  int (*process_parmX)(clpar p[], int len);
  const char *help;
  } parm_handlers[] = {
    {"send", process_parmSend, help_parmSend},
    {"config", process_parmConfig, help_parmConfig},
    {"dbg", process_parmDbg, help_parmDbg},
    {"timer", process_parmTimer, help_parmTimer },
    {"help", process_parmHelp, help_parmHelp },
#if ENABLE_EXPERT
    {"expert", process_parmExpert, help_None},
#endif
    };  
      

int ICACHE_FLASH_ATTR
process_parm(clpar p[], int len) {
	int i;
    for (i=0; i < (sizeof (parm_handlers) / sizeof (parm_handlers[0])); ++i) {
    	if (strcmp(p[0].key, parm_handlers[i].parm) == 0)
           return parm_handlers[i].process_parmX(p, len);
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


	static const char usage[] PROGMEM = "syntax: command option=value ...;\n"
			"commands are: ";

	if (len == 1) {
		io_puts_P(help_parmHelp), io_putlf();
		return 0;
	}


	for (i = 1; i < len; ++i) {
		int k;
		const char *key = p[i].key, *val = p[i].val;
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


#if TEST_MODULE_CLI
bool ICACHE_FLASH_ATTR
testModule_cli()
{
	char cl[] = "timer g=5 astro=0;";//"timer g=2 m=2 weekly=08222000++++10552134+";
	int n = parse_commandline(cl);
	if (n > 0)
	process_parm(par, n);

	return n != -1;
}
#endif


