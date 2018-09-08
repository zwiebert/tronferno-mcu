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
  if (0 < m && m < 8)
    return -1;
  else if (m > 7 )
    m -= 7;
  if (m > 7)
    return -1;

  return set_state(a, g, m, -1);
}

int ICACHE_FLASH_ATTR
set_shutter_state(uint32_t a, fer_grp g, fer_memb m, fer_cmd cmd) {
  int position = -1;

  if (0 < m && m < 8)
    return -1;
  else if (m > 7)
    m -= 7;
  if (m > 7)
    return -1;

  if (cmd == fer_cmd_UP)
    position = 100;
  else if (cmd == fer_cmd_DOWN)
    position = 0;

  if (0 <= position && position <= 100) {
    io_puts("U:position: a="), io_print_hex_32(a, false), io_puts(" g="), io_putd(g), io_puts(" m="), io_putd(m), io_puts(" p="), io_putd(position), io_puts(";\n");
  }

  return set_state(a, g, m, position);
}
