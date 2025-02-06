

#include <string.h>

#include "fernotron/pos/shutter_pct.h"
#include "cli_internal.hh"
#include "cli_fernotron.hh"
#include "stdint.h"
#include "fernotron/alias/pairings.h"
#include "app_uout/status_output.h"
#include "app_misc/opt_map.hh"

#ifdef CONFIG_TF_DEBUG
#define DEBUG
#define D(x) x
#define DP(x) (ets_printf("%s: %s\n", logtag, x))
#define DL (ets_printf("%s:line: %d\n", logtag, (int) __LINE__))
#else
#define D(x)
#define DP(x)
#define DL
#endif
#define logtag "tf.cli.parm_alias"

#define PRAS_TIMEOUT 15

//#define PAIR_GPIO

const char cli_help_parmPair[] = "'pair' stores information about transmitter to receiver registration"
    "a=(?|ID) 0  transmitter-ID to pair. '?' starts RF-auto-scan\n"
    "g=[0-7]   0  group number\n"
    "m=[0-7]   0  receiver number\n"
#ifdef PAIR_GPIO
    "gpoutN=(up|down|switch)   \n"
    "gpinN=(up|down|stop|rain|toggle)\n"
#endif
    "c=(pair|unpair|read)\n";

#define is_kt(k) (kt == otok:: k)
#define is_key(k) (strcmp(key, k) == 0)
#define is_val(k) (strcmp(val, k) == 0)

int 
process_parmPair(clpar p[], int len, class UoutWriter &td) {
  int arg_idx;
  int i;

  so_object<void> cfgObj(&soMsg_pair_begin, &soMsg_pair_end, td);

  uint32_t addr = 0;
  const char *addr_as_string = "";
  uint8_t g = 0, m = 0;
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
        if (is_val("?"))
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
        if (is_val("unpair")) {
          unpair = true;
          c = PC_unpair;
        } else if (is_val("pair")) {
          pair = true;
          c = PC_pair;
        } else if (is_val("read")) {
          read = true;
          c = PC_read;
        } else if (is_val("read_all")) {
          read_all = true;
          c = PC_read;
        } else if (is_val("store")) {
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
    fer_alias_auto_set(g, m, c, PRAS_TIMEOUT);
  }

  if (read_all) {
    fer_alias_so_output_all_pairings(td);
  }

  if (addr && (((pair || unpair) && g && m) || (read && !g && !m))) {

    if (pair || unpair) {
      cli_replyResult(td, fer_alias_controller(addr, g, m, unpair));
    }

    if (read) {
      Fer_GmSet mm;
      if (fer_alias_getControllerPairings(addr, &mm)) {
        so_arg_kmm_t kmm = {addr_as_string, &mm };
        soMsg_pair_print_kmm_single(td, kmm);
      }
    }

  }

  return 0;
}
