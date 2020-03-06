#include <fernotron/fer_msg_basic.h>
#include "app/proj_app_cfg.h"

#include <string.h>

#include "fernotron_pos/shutter_pct.h"
#include "cli_imp.h"
#include "cli_fer.h"
#include "main/common.h"
#include "fernotron_alias/pairings.h"
#include "userio_app/status_output.h"

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

int 
process_parmPair(clpar p[], int len) {
  int arg_idx;
  int i;

  so_output_message(SO_PAIR_begin, NULL);

  u32 addr = 0;
  const char *addr_as_string = "";
  u8 g = 0, m = 0, c = 0;
  gm_bitmask_t mm = {0,};
  bool has_mm = false;
  bool pair = false, unpair = false, read = false, read_all = false,  scan = false, store = false;

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL) {
      return -1;
    } else if (strcmp(key, "a") == 0) {
      if (*val == '?')
        scan = true;
      else {
        addr = val ? strtol(val, NULL, 16) : 0;
        if (val)
          addr_as_string = val;
      }
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
    } else if (strcmp(key, "mm") == 0) {
      uint64_t n = strtoll(val, 0, 16);
      for (i=0;n; ++i, (n >>= 8)) {
        mm[i] = n & 0xff;
      }
       has_mm = true;
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
      } else if (strcmp(val, "read_all") == 0) {
        read_all = true;
        c = PC_read;
      } else if (strcmp(val, "store") == 0) {
        store = true;

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

  if (store && has_mm) {
    if (pair_setControllerPairings(addr, &mm)) {
      so_arg_kmm_t kmm = {addr_as_string, mm };
      so_output_message(SO_PAIR_PRINT_KMM_SINGLE, &kmm);
    } else {
      read = true;
    }
  }

  if (scan) {
    pair_auto_set(g, m, c, msgid, PRAS_TIMEOUT);
  }

  if (read_all) {
    pair_so_output_all_pairings();
  }

  if (addr && (((pair || unpair) && g && m) || (read && !g && !m))) {

    if (pair || unpair) {
      reply(pair_controller(addr, g, m, unpair));
    }

    if (read) {
      gm_bitmask_t gm;
      if (pair_getControllerPairings(addr, &gm)) {
        so_arg_amm_t amm = {addr, &gm[0] };
        so_output_message(SO_PAIR_PRINT_AMM, &amm);
      }
    }

  }

  so_output_message(SO_PAIR_end, NULL);

  return 0;
}
