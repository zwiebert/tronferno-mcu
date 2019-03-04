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
#include "main/pairings.h"

#if defined DISTRIBUTION || 0
#define D(x) x
#define DP(x) (ets_printf("db: %s\n", x))
#define DL (ets_printf("db:line: %d\n", (int) __LINE__))
#else
#define D(x)
#define DP(x)
#define DL
#endif


enum pair_cmds { PC_none, PC_pair, PC_unpair, PC_read };
static uint8_t pras_g, pras_m, pras_c;
#define PRAS_TIMEOUT 15


const char help_parmPair[]  =
  "a=(?|ID) 0  controller to pair. '?' starts auto-scan\n"
  "g=[0-7]   0  group number\n"
  "m=[0-7]   0  group member number\n"
  "gpoutN=(up|down|switch)   \n"
  "gpinN=(up|down|stop|rain|toggle)\n"
  "c=(pair|unpair|read)\n"
  ;

int ICACHE_FLASH_ATTR
process_parmPair(clpar p[], int len) {
  int arg_idx;

  uint32_t addr = 0;
  uint8_t g = 0, m = 0, c= 0;
  bool pair = false, unpair = false, read = false, scan = false;
  
  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL) {
      return -1;
    } else if (strcmp(key, "a") == 0) {
      if (*val == '?')
	scan = true;
      else
	addr = val ? strtol(val, NULL, 16) : 0;
    } else if (strcmp(key, "g") == 0) {
      fer_grp group;
      if (!asc2group(val, &group) || group == 0)
        return reply_failure();
      g = group;
    } else if (strcmp(key, "m") == 0) {
      fer_memb memb;
      if (!asc2memb(val, &memb) || memb == 0)
        return reply_failure();
      m = memb - 7;
    } else if (strcmp(key, "c") == 0) {
      if (strcmp(val, "unpair") == 0) {
	unpair = true;
	c = PC_unpair;
      } else  if (strcmp(val, "pair") == 0) {
	pair = true;
	c = PC_pair;
      } else if (strcmp(val, "read") == 0) {
	read = true;
	c = PC_read;
      } else if (strncmp(key, "gpin", 4) == 0) {
	int gpio_number = atoi(key + 4);
      } else if (strncmp(key, "gpout", 5) == 0) {
	int gpio_number = atoi(key + 5);
      } 
    } else {
      reply_failure();
    }
  }

  if (scan) {
    pras_g = g;
    pras_m = m;
    pras_c = c;
    pair_auto_set(PRAS_TIMEOUT);
  }

  if (addr
      && (((pair || unpair) && g && m)
	  || (read && !g && !m))) {

    if (pair || unpair) {
      reply(pair_controller(addr, g, m, unpair));
    }

    if (read) {
      gm_bitmask_t gm;
      if (read_pairings(&gm, addr)) {
	io_puts("pair a="), io_print_hex_32(addr, false),
	  io_puts(" mm="), io_putd(g);
	for (g=0; g < 8; ++g) {
	  io_putx8(gm[g]);
	  if (g < 7)
	    io_putc(',');
	}
	io_puts(";\n");
      }
    }
  }
  return 0;
}




bool pras_active;
bool ICACHE_FLASH_ATTR pair_auto_set(unsigned init_seconds) {
  static time_t end_time;
  static uint16_t pras_msgid;

  if (init_seconds > 0) {
    end_time = run_time(NULL) + init_seconds;
    last_received_sender.data[0] = 0;
    pras_msgid = msgid;
    pras_active = true;
    io_puts("U:pras: start listening at RF. timout="),
      io_putd(init_seconds), io_putlf();
  } else if (end_time == 0) {

  } else if (end_time < run_time(NULL)) {
    uint16_t global_msgid = msgid;
    end_time = 0;
    io_puts("U:pras: stop listening (timeout)\n");
    msgid = pras_msgid;
    reply_message("pras=time-out", 0);
    msgid = global_msgid;
    pras_active = false;
  } else if (!FSB_ADDR_IS_CENTRAL(&last_received_sender)) {
    uint16_t global_msgid = msgid;
    uint32_t a = FSB_GET_DEVID(&last_received_sender);
    bool success = false;
    
    if (pras_c == PC_pair || pras_c == PC_unpair) {
      DL;
      if(pair_controller(a, pras_g, pras_m, pras_c == PC_unpair)) {
	DL;
	success = true;
      }
    }

    io_puts("U:pras: controller was ");
    if (!success)
      io_puts("NOT ");
    else if (pras_c == PC_unpair)
      io_puts("un");
    io_puts("paired\n");


    end_time = 0;

    msgid = pras_msgid;
    reply_message("pras=ok", 0);
    msgid = global_msgid;

    pras_active = false;
    return true;
  }

  return false;
}
