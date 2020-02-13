#include "user_config.h"

#include <string.h>

#include "../shutter_positions/shutter_state.h"
#include "cli/cli_imp.h"
#include "fernotron/fer_code.h"
#include "main/common.h"
#include "main/pairings.h"
#include "userio/status_output.h"


#if defined DISTRIBUTION || 0
#define D(x) x
#define DP(x) (ets_printf("db: %s\n", x))
#define DL (ets_printf("db:line: %d\n", (int) __LINE__))
#else
#define D(x)
#define DP(x)
#define DL
#endif

#define PRAS_TIMEOUT 15

const char help_parmPair[] = ""
    "a=(?|ID) 0  controller to pair. '?' starts auto-scan\n"
    "g=[0-7]   0  group number\n"
    "m=[0-7]   0  group member number\n"
    "gpoutN=(up|down|switch)   \n"
    "gpinN=(up|down|stop|rain|toggle)\n"
    "c=(pair|unpair|read)\n";

int ICACHE_FLASH_ATTR
process_parmPair(clpar p[], int len) {
  int arg_idx;

  u32 addr = 0;
  u8 g = 0, m = 0, c = 0;
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
      } else if (strcmp(val, "pair") == 0) {
        pair = true;
        c = PC_pair;
      } else if (strcmp(val, "read") == 0) {
        read = true;
        c = PC_read;
#if 0
      } else if (strncmp(key, "gpin", 4) == 0) {
        int gpio_number = atoi(key + 4);
      } else if (strncmp(key, "gpout", 5) == 0) {
        int gpio_number = atoi(key + 5);
#endif
      }

    } else {
      reply_failure();
    }
  }

  if (scan) {
    pair_auto_set(g, m, c, msgid, PRAS_TIMEOUT);
  }

  if (addr && (((pair || unpair) && g && m) || (read && !g && !m))) {

    if (pair || unpair) {
      reply(pair_controller(addr, g, m, unpair));
    }

    if (read) {
      gm_bitmask_t gm;
      if (read_pairings(&gm, addr)) {
        so_arg_amm_t amm = {addr, &gm[0] };
        so_output_message(SO_PAIR_PRINT_AMM, &amm);
      }
    }
  }
  return 0;
}
