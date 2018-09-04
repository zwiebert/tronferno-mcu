#include "current_state.h"


uint8_t groups[7];

static int ICACHE_FLASH_ATTR
set_state(uint32_t a, int g, int m, int state) {
  if (1 <= g && g <= 7 && 1 <= m && m <= 7) {
    if (state >= 0) {
      bool s = state != 0;
      if (s)
        groups[g - 1] |= (1 << (m - 1));
      else
        groups[g - 1] &= ~(1 << (m - 1));

    } else {
      return (groups[g - 1] & (1 << (m - 1))) ? 100 : 0;
    }
  }
  return -1;
}



int ICACHE_FLASH_ATTR
get_shutter_state(uint32_t a, fer_grp g, fer_memb m) {
  return set_state(a, g, m-7, -1);
}

int ICACHE_FLASH_ATTR
set_shutter_state(uint32_t a, fer_grp g, fer_memb m, fer_cmd cmd) {
  int state = -1;

  if (cmd == fer_cmd_UP)
    state = 100;
  else if (cmd == fer_cmd_DOWN)
    state = 0;

  return set_state(a, g, m-7, state);
}
