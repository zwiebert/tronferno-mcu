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




const char help_parPair[] PROGMEM =
  "a=(0|ID)  0  hex ID of controller to pair\n"
  "g=[0-7]   0  group number\n"
  "m=[0-7]   0  group member number\n"
  ;

int ICACHE_FLASH_ATTR
process_parmPair(clpar p[], int len) {
  int arg_idx;

  uint32_t addr = 0;
  uint8_t g = 0, m = 0;
  bool pair = false, unpair = false, read = false;
  
  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL) {
      return -1;
    } else if (strcmp(key, "a") == 0) {
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
      } else  if (strcmp(val, "pair") == 0) {
	pair = true;
      } else if (strcmp(val, "read") == 0) {
	read = true;
      } 
    } else {
      reply_failure();
    }
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
