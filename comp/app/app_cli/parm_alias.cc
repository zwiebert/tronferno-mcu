#include "app/config/proj_app_cfg.h"

#include <string.h>

#include "fernotron/pos/shutter_pct.h"
#include "cli_imp.h"
#include "cli_fer.h"
#include "app/common.h"
#include "fernotron/alias/pairings.h"
#include "app/uout/status_output.h"
#include "app/opt_map.hh"

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

const char cli_help_parmPair[] = ""
    "a=(?|ID) 0  controller to pair. '?' starts auto-scan\n"
    "g=[0-7]   0  group number\n"
    "m=[0-7]   0  group member number\n"
    "gpoutN=(up|down|switch)   \n"
    "gpinN=(up|down|stop|rain|toggle)\n"
    "c=(pair|unpair|read)\n";

#define is_kt(k) (kt == otok:: k)
#define is_key(k) (strcmp(key, #k) == 0)
#define is_val(k) (strcmp(val, #k) == 0)

int 
process_parmPair(clpar p[], int len, const struct TargetDesc &td) {
  int arg_idx;
  int i;

  so_object<void> cfgObj(&soMsg_pair_begin, &soMsg_pair_end, td);

  u32 addr = 0;
  const char *addr_as_string = "";
  u8 g = 0, m = 0;
  fer_alias_cmds c = PC_none;
  Fer_GmSet mm ;
  bool has_mm = false;
  bool pair = false, unpair = false, read = false, read_all = false,  scan = false, store = false;

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL)
      return -1;

    otok kt = optMap_findToken(key);

    if (kt != otok::NONE) {
      switch (kt) {

      case otok::k_a: {
        if (is_val(?))
          scan = true;
        else {
          addr = val ? strtol(val, NULL, 16) : 0;
          if (val)
            addr_as_string = val;
        }
      }
        break;

      case otok::k_g: {
        if (!asc2u8(val, &g, 7))
        return cli_replyFailure(td);
      }
      break;
      case otok::k_m: {
      if (!asc2u8(val, &m, 7))
      return cli_replyFailure(td);
      }
      break;

      case otok::k_mm: {
        uint64_t n = strtoll(val, 0, 16);
        for (i = 0; n; ++i, (n >>= 8)) {
          mm[i] = n & 0xff;
        }
        has_mm = true;
      }
        break;

      case otok::k_c: {
        if (is_val(unpair)) {
          unpair = true;
          c = PC_unpair;
        } else if (is_val(pair)) {
          pair = true;
          c = PC_pair;
        } else if (is_val(read)) {
          read = true;
          c = PC_read;
        } else if (is_val(read_all)) {
          read_all = true;
          c = PC_read;
        } else if (is_val(store)) {
          store = true;
        }
      }
        break;



      default:
        break;
      }
    } else {
#if 0
      if (strncmp(key, "gpin", 4) == 0) {
        int gpio_number = atoi(key + 4);
      } else if (strncmp(key, "gpout", 5) == 0) {
        int gpio_number = atoi(key + 5);
      } else {
        cli_replyFailure(td);
      }
#else
      cli_replyFailure(td);
#endif
    }
  }

  if (store && has_mm) {
    if (fer_alias_setControllerPairings(addr, &mm)) {
      so_arg_kmm_t kmm = {addr_as_string, &mm };
      soMsg_pair_print_kmm_single(td, kmm);
    } else {
      read = true;
    }
  }

  if (scan) {
    fer_alias_auto_set(td, g, m, c, cli_msgid, PRAS_TIMEOUT);
  }

  if (read_all) {
    fer_alias_so_output_all_pairings(td);
  }

  if (addr && (((pair || unpair) && g && m) || (read && !g && !m))) {

    if (pair || unpair) {
      cli_replyResult(td, fer_alias_controller(addr, g, m, unpair));
    }

    if (read) {
      Fer_GmSet gm;
      if (fer_alias_getControllerPairings(addr, &gm)) {
        so_arg_amm_t amm = {addr, &gm };
        soMsg_pair_print_amm(td, amm);
      }
    }

  }

  return 0;
}
