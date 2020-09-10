#include <fernotron/fer_msg_plain.h>
#include "app_config/proj_app_cfg.h"

#include <string.h>

#include "fernotron/pos/shutter_pct.h"
#include "cli_imp.h"
#include "cli_app/cli_fer.h"
#include "app/common.h"
#include "fernotron/alias/pairings.h"
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

const char cli_help_parmPair[] = ""
    "a=(?|ID) 0  controller to pair. '?' starts auto-scan\n"
    "g=[0-7]   0  group number\n"
    "m=[0-7]   0  group member number\n"
    "gpoutN=(up|down|switch)   \n"
    "gpinN=(up|down|stop|rain|toggle)\n"
    "c=(pair|unpair|read)\n";

#define is_key(k) (strcmp(key, #k) == 0)
#define is_val(k) (strcmp(val, #k) == 0)

#include <misc/allocator_malloc.hh>
#include <map>
#include <utility>


enum {
  key_none = -1, key_a, key_g, key_m, key_mm, key_c, key_SIZE
};

typedef i8 keyTokT;

const char *const parmPair_keys[key_SIZE] = {
    "a", "g", "m", "mm", "c",
};

using keyMapT = std::map<const char*, keyTokT, std::less<const char *>, AllocatorMalloc<std::pair<const char *const, keyTokT>>>;

class OptMap {
public:
  OptMap(const char *const key_names[], int len) {
    for (keyTokT i=0; i < len; ++i) {
      mKeyMap.emplace(std::make_pair(key_names[i], i));
    }
  }
  keyTokT get(const char *const key) const {
    auto it =  mKeyMap.find(key);
    if (it == mKeyMap.end())
      return -1;
    return it->second;
  }
private:
  keyMapT mKeyMap;
};

static const OptMap opt_map(parmPair_keys, key_SIZE);

#undef is_key
#define is_key(k) (key_##k == opt_map.get(key))

int 
process_parmPair(clpar p[], int len) {
  int arg_idx;
  int i;

  so_object cfgObj(SO_PAIR_begin, nullptr, SO_PAIR_end);

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
    } else if (is_key(a)) {
      if (is_val(?))
        scan = true;
      else {
        addr = val ? strtol(val, NULL, 16) : 0;
        if (val)
          addr_as_string = val;
      }
    } else if (is_key(g)) {
      fer_grp group;
      if (!asc2group(val, &group) || group == 0)
        return cli_replyFailure();
      g = group;
    } else if (is_key(m)) {
      fer_memb memb;
      if (!asc2memb(val, &memb) || memb == 0)
        return cli_replyFailure();
      m = memb - 7;
    } else if (is_key(mm)) {
      uint64_t n = strtoll(val, 0, 16);
      for (i=0;n; ++i, (n >>= 8)) {
        mm[i] = n & 0xff;
      }
       has_mm = true;
    } else if (is_key(c)) {
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

#if 0
      } else if (strncmp(key, "gpin", 4) == 0) {
        int gpio_number = atoi(key + 4);
      } else if (strncmp(key, "gpout", 5) == 0) {
        int gpio_number = atoi(key + 5);
#endif
      }

    } else {
      cli_replyFailure();
    }
  }

  if (store && has_mm) {
    if (pair_setControllerPairings(addr, &mm)) {
      so_arg_kmm_t kmm = {addr_as_string, &mm };
      so_output_message(SO_PAIR_PRINT_KMM_SINGLE, &kmm);
    } else {
      read = true;
    }
  }

  if (scan) {
    pair_auto_set(g, m, c, cli_msgid, PRAS_TIMEOUT);
  }

  if (read_all) {
    pair_so_output_all_pairings();
  }

  if (addr && (((pair || unpair) && g && m) || (read && !g && !m))) {

    if (pair || unpair) {
      cli_replyResult(pair_controller(addr, g, m, unpair));
    }

    if (read) {
      gm_bitmask_t gm;
      if (pair_getControllerPairings(addr, &gm)) {
        so_arg_amm_t amm = {addr, &gm };
        so_output_message(SO_PAIR_PRINT_AMM, &amm);
      }
    }

  }

  return 0;
}
