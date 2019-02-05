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

#define FSB_PLAIN_REPEATS 2  // send plain commands 1+N times (if 0, send only once without repeating)



const char help_parmSend[] PROGMEM =
    "a=(0|ID)  0  hex ID of sender or receiver. 0 uses 'cu' in config\n"
    "g=[0-7]   0  group number\n"
    "m=[0-7]   0  group member number\n"
    "c=(up|down|stop|sun-down|sun-inst|set|?)\n"
   // "p=(?|100|0)"
    "r=N       2  repeat command 1+N times\n"
    "SEP[=0|1]    Enter end-position adjustment mode (needs hardware button)"
// "TROT        Toggle rotation direction"
;

int ICACHE_FLASH_ATTR
process_parmSend(clpar p[], int len) {
  int arg_idx;

  uint32_t addr = 0;
  fer_grp group = fer_grp_Broadcast;
  fer_memb memb = fer_memb_Broadcast;
  fer_cmd cmd = fer_cmd_None;
  int set_end_pos = -1;
  uint8_t repeats = FSB_PLAIN_REPEATS;
  bool read_state = false;
  bool do_send = false;


  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL) {
      return -1;
    } else if (strcmp(key, "a") == 0) {
      addr = val ? strtol(val, NULL, 16) : 0;
    } else if (strcmp(key, "g") == 0) {
      if (!asc2group(val, &group))
        return reply_failure();
    } else if (strcmp(key, "m") == 0) {
      if (!asc2memb(val, &memb))
        return reply_failure();
    } else if (strcmp(key, "r") == 0) {
      NODEFAULT();
      repeats = atoi(val);
      if (!(repeats <= 10)) {
        return reply_failure();
      }
   } else if (strcmp(key, "p") == 0) {
     NODEFAULT();
     if (*val == '?') {
       read_state = true;
     } else {
       return reply_failure();
     }
   } else if (strcmp(key, "c") == 0) {
      NODEFAULT();
      if (*val == '?') {
        read_state = true;
      } else if (!cli_parm_to_ferCMD(val, &cmd)) {
        return reply_failure();
      }
    } else if (strcmp(key, "SEP") == 0) {
      set_end_pos = asc2bool(val);
      if (set_end_pos != 1)
      set_end_pos = 0;  // force disable
    } else {
      warning_unknown_option(key);
    }
  }

  if (read_state) {
    uint8_t g = group;
    uint8_t m = memb == 0 ? 0 : memb - 7;
    if (g != 0 && m != 0) {
      int pos = get_shutter_state(addr, g, m);
      if (pos >= 0) {
        io_puts("A:position:"), io_puts(" g="), io_putd(g), io_puts(" m="), io_putd(m),
            io_puts(" p="), io_putd(pos), io_puts(";\n");
      }
    } else {
      print_shutter_positions();
    }
  } else {
    fer_sender_basic *fsb = get_sender_by_addr(addr);
    if (!fsb) {
      static fer_sender_basic fsb_direct; // FIXME: or was senders[0] meant for this?
      fsb = &fsb_direct;
      if (FSB_GET_DEVID(fsb) != addr) {
        fer_init_sender(fsb, addr);
      }
    }

    if (FSB_ADDR_IS_CENTRAL(fsb)) {
      FSB_PUT_GRP(fsb, group);
      FSB_PUT_MEMB(fsb, memb);  // only set this on central unit!
      //assert(group == FSB_GET_GRP(fsb));
      //assert(memb == FSB_GET_MEMB(fsb));
    }

    if (set_end_pos >= 0) { // enable hardware buttons to set end position
      FSB_PUT_CMD(fsb, cmd);
      if (set_end_pos)
        sep_enable(fsb);
      else
        sep_disable();
    } else if (cmd != fer_cmd_None) {
      FSB_PUT_CMD(fsb, cmd);
      fer_update_tglNibble(fsb);
      fsb->repeats = repeats;
      if(reply(fer_send_msg(fsb, MSG_TYPE_PLAIN))) {
	uint8_t g = group;
	uint8_t m = memb == 0 ? 0 : memb - 7;
        set_shutter_state(FSB_GET_DEVID(fsb), g, m, FSB_GET_CMD(fsb));
      }
    } else {
      reply_failure();
    }
  }
  return 0;
}
